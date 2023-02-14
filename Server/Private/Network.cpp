#include "pch.h"
#include "Network.h"

CNetwork::CNetwork(void){
}

CNetwork::~CNetwork(void) {
	Close();
	WSACleanup();
}

const HRESULT CNetwork::Init(void) {
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
		return E_FAIL;
	return S_OK;
}

void CNetwork::Update(void) {
	Convert_Msg();
}

void CNetwork::Convert_Msg(void) {
	if (!m_listSend2.empty()) {
		m_mutex.lock();
		m_listSend.insert(m_listSend.end(), m_listSend2.begin(), m_listSend2.end());
		m_mutex.unlock();
		m_listSend2.clear();
	}

	if (!m_listRecv.empty()) {
		m_mutex.lock();
		m_listRecv2.insert(m_listRecv2.end(), m_listRecv.begin(), m_listRecv.end());
		m_listRecv.clear();
		m_mutex.unlock();
	}

	for (auto& iter : m_listRecv2) {
		Json::Value root;
		Json::Reader reader;
		reader.parse(iter, root);
		if (true == m_bIsServer && true == root["Echo"].asBool())
			m_listSend2.emplace_back(iter);
		//m_arrRecv[root["Id"].asInt()].emplace_back(root);
	}
	m_listRecv2.clear();
}

const HRESULT CNetwork::Open_Server(int port, int id) {
	m_bIsServer = true;

	SOCKET _socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN sockaddr_in{};
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_port = htons(port);
	sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	if (SOCKET_ERROR == ::bind(_socket, reinterpret_cast<SOCKADDR*>(&sockaddr_in), sizeof(sockaddr_in)))
		return E_FAIL;
	if (SOCKET_ERROR == listen(_socket, SOMAXCONN))
		return E_FAIL;

	m_socket = _socket;
	m_vecThread.emplace_back(thread{ &CNetwork::Accept, this, _socket });
	m_vecThread.emplace_back(thread{ &CNetwork::Send, this });

	m_iId = id;
	return S_OK;
}

const HRESULT CNetwork::Open_Client(const char* serverip, int port, int ip, int id) {
	m_iIp = ip;
	m_iId = id;
	SOCKET _socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN socket_addr_in;
	socket_addr_in.sin_family = AF_INET;
	socket_addr_in.sin_port = htons(port);
	socket_addr_in.sin_addr.s_addr = inet_addr(serverip);

	if (SOCKET_ERROR == connect(_socket, reinterpret_cast<SOCKADDR*>(&socket_addr_in), sizeof(socket_addr_in)))
		return E_FAIL;
	char send_arr[sizeof(int)];
	ZeroMemory(send_arr, sizeof(int));
	memcpy(&send_arr[0], &m_iIp, sizeof(int));
	send(_socket, send_arr, static_cast<int>(sizeof(int)), 0);

	m_mapSocket.emplace(0, _socket);
	m_vecThread.emplace_back(thread(&CNetwork::Send, this));
	m_vecThread.emplace_back(thread(&CNetwork::Recv, this, _socket));

	return S_OK;
}

void CNetwork::Close(void) {
	m_bThread = false;

	for (auto& pair : m_mapSocket)
		closesocket(pair.second);
	m_mapSocket.clear();

	closesocket(m_socket);

	for (auto& iter : m_vecThread)
		iter.join();
	m_vecThread.clear();
}

void CNetwork::Accept(SOCKET _socket) {
	while (true) {
		SOCKADDR_IN sockaddr_in{};
		int sockaddr_in_len = sizeof(sockaddr_in);
		SOCKET __socket = accept(_socket, (SOCKADDR*)&sockaddr_in, &sockaddr_in_len);
		if (INVALID_SOCKET == __socket)
			break;
		char buf[sizeof(int)];
		ZeroMemory(buf, sizeof(int));
		recv(__socket, buf, sizeof(int), 0);
		int Ip = 0;
		memcpy(&Ip, buf, sizeof(int));

		m_mapSocket.emplace(Ip, __socket);
		m_vecThread.emplace_back(thread{ &CNetwork::Recv, this, __socket });
	}
	closesocket(_socket);
}

void CNetwork::Send(void) {
	while (true) {
		if (false == m_bThread)
			return;
		if (true == m_listSend.empty())
			continue;

		m_mutex.lock();
		list<string> listSend;
		listSend.insert(listSend.end(), m_listSend.begin(), m_listSend.end());
		m_listSend.clear();
		m_mutex.unlock();

		for (auto& iter : listSend) {
			size_t send_size = iter.length();
			char* send_arr = new char[sizeof(size_t) + send_size];
			ZeroMemory(send_arr, sizeof(size_t) + send_size);

			memcpy(&send_arr[0], &send_size, sizeof(size_t));
			memcpy(&send_arr[sizeof(size_t)], iter.c_str(), send_size);

			string str = iter;

			Json::Value root;
			Json::Reader reader;
			reader.parse(str, root);
			int Ip = root["Ip"].asInt();

			int _send = 0;
			for (auto& pair : m_mapSocket) {
				if (Ip == pair.first)
					continue;
				_send = send(pair.second, send_arr, static_cast<int>(sizeof(size_t) + send_size), 0);
				if (SOCKET_ERROR == _send)
					break;
			}
			if (SOCKET_ERROR == _send) {
				delete[] send_arr;
				send_arr = nullptr;
				break;
			}
			delete[] send_arr;
			send_arr = nullptr;
		}

	}
}

void CNetwork::Recv(SOCKET _socket) {
	int recv_size = 0;
	char recv_arr[sizeof(size_t)];

	int buf_size = 0;
	size_t buf_size_total = 0;
	char* buf_arr = nullptr;

	list<string> listRecv;
	while (true) {
		char buf[2048] = {};
		int _return = recv(_socket, buf, 2048, 0);
		if (SOCKET_ERROR == _return)
			break;
		else if (0 == _return)
			break;
		else {
			int _index = 0;
			while (_index < _return) {
				int _extra = _return - _index;
				if (nullptr == buf_arr) {
					if (sizeof(size_t) > recv_size + _extra) {
						memcpy(&recv_arr[recv_size], &buf[_index], _extra);
						recv_size += _extra;
						_index += _extra;
					}
					else {
						memcpy(&recv_arr[recv_size], &buf[_index], sizeof(size_t) - recv_size);
						_index += sizeof(size_t) - recv_size;

						memcpy(&buf_size_total, &recv_arr, sizeof(size_t));
						buf_arr = new char[buf_size_total + 1];
						ZeroMemory(buf_arr, buf_size_total + 1);

						buf_size = 0;
					}
				}
				else {
					if (buf_size_total > _extra + buf_size) {
						memcpy(&buf_arr[buf_size], &buf[_index], _extra);
						buf_size += _extra;
						_index += _extra;
					}
					else {
						memcpy(&buf_arr[buf_size], &buf[_index], int(buf_size_total) - buf_size);
						_index += static_cast<int>(buf_size_total) - buf_size;

						listRecv.emplace_back(buf_arr);

						delete[] buf_arr;
						buf_arr = nullptr;

						recv_size = 0;
					}
				}
			}
			m_mutex.lock();
			m_listRecv.insert(m_listRecv.end(), listRecv.begin(), listRecv.end());
			m_mutex.unlock();
			listRecv.clear();
		}
	}
	closesocket(_socket);
}

void CNetwork::Set_Send(const int& id, const string& key, Json::Value& root, const bool& echo) {
	root["Ip"] = m_iIp;
	root["Id"] = id;
	root["Key"] = key;
	root["Echo"] = echo;
	string str = root.toStyledString();
	m_listSend2.emplace_back(str);
}

const bool CNetwork::Get_Recv(const int& id, const string& key, Json::Value& root) {
	Convert_Msg();
	auto iter = find_if(m_arrRecv[id].begin(), m_arrRecv[id].end(), [&](Json::Value iter) {
		if (key == iter["Key"].asString())
			return true;
		else
			return false;
		});
	if (m_arrRecv[id].end() == iter) {
		return false;
	}
	root = *iter;
	m_arrRecv[id].erase(iter);
	return true;
}

list<Json::Value>& CNetwork::Get_Recv(const int& id) {
	return m_arrRecv[id];
}

const int CNetwork::Get_Id(void) {
	return m_iId++;
}

const bool& CNetwork::IsServer(void) {
	return m_bIsServer;
}
