#include "pch.h"
#include "Network.h"

int main(int argc, char* argv[]) {
    CNetwork* pNetwork = new CNetwork;
    HRESULT hr = S_OK;
    hr |= pNetwork->Init();

	ifstream ifs(TEXT("../Bin/Resource/Network.txt"));
	Json::Value root;
	ifs >> root;
	ifs.close();
	string ServerIp = root["ServerIp"].asString();
	int Ip = root["Ip"].asInt();
	int Id = root["Id"].asInt();
	int Port = root["Port"].asInt();

	hr |= pNetwork->Open_Server(Port, Id);

    while (true) {
		pNetwork->Convert_Msg();
    }

    delete pNetwork;
    return 0;
}