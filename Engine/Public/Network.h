#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CNetwork final : public CBase{
	DECLARE_SINGLETON(CNetwork)
private:
	explicit CNetwork(void);
	virtual ~CNetwork(void) override = default;
	virtual void Delete(void) override;
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
	void Set_Send(const _int & id, const string & key, Json::Value & root, const _bool & echo = true);
	const _bool Get_Recv(const _int& id, const string& key, Json::Value& root);

	list<Json::Value>& Get_Recv(const _int & id);
public:
	const _int Get_Id(void);
	const _bool& IsServer(void);
private:
	_bool m_bThread = true;
	vector<thread> m_vecThread;
private:
	SOCKET m_socket = INVALID_SOCKET;
	map<_int, SOCKET> m_mapSocket;
public:
	mutex m_mutex;
	list<string> m_listSend;
	list<string> m_listRecv;

	list<string> m_listSend2;
	list<string> m_listRecv2;
	list<Json::Value> m_arrRecv[300];
private:
	_bool m_bIsServer = false;
	_int m_iIp = -1;
	_int m_iId = -1;
};
END_NAMESPACE