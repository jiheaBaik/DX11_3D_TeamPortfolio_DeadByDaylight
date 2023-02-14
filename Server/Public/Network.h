#pragma once

class CNetwork final {
public:
	explicit CNetwork(void);
	 ~CNetwork(void);
public:
	const HRESULT Init(void);
	void Update(void);
public:
	const HRESULT Open_Server(int port, int id);
	const HRESULT Open_Client(const char* serverip, int port, int ip, int id);
	void Close(void);
private:
	void Accept(SOCKET _socket);
	void Send(void);
	void Recv(SOCKET _socket);
public:
	void Convert_Msg(void);
	void Set_Send(const int & id, const string & key, Json::Value & root, const bool & echo = true);
	const bool Get_Recv(const int & id, const string & key, Json::Value & root);

	list<Json::Value>& Get_Recv(const int & id);
public:
	const int Get_Id(void);
	const bool& IsServer(void);
private:
	bool m_bThread = true;
	vector<thread> m_vecThread;
private:
	SOCKET m_socket = INVALID_SOCKET;
	map<int, SOCKET> m_mapSocket;
public:
	mutex m_mutex;
	list<string> m_listSend;
	list<string> m_listRecv;

	list<string> m_listSend2;
	list<string> m_listRecv2;
	list<Json::Value> m_arrRecv[300];
private:
	bool m_bIsServer = false;
	int m_iIp = 0;
	int m_iId = 0;
};
