#include "pch.h"
#include "Scene_Logo.h"
#include "Loading.h"
#include "Setting_BGM.h"

CScene_Logo* const CScene_Logo::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CScene_Logo* pInstnace = new CScene_Logo(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CScene_Logo::CScene_Logo(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CScene(pDevice, pContext, pNetwork) {
}

void CScene_Logo::Delete(void) {
	__super::Delete();
}

const HRESULT CScene_Logo::Init(void) {
	OutputDebugString(TEXT("Scene : Logo\n"));
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;

	if (CEdit_Mgr::EMode::EDIT == pGame_Instance->Get_Edit_Mode()) {
		CCamera::SDesc sCamera;
		ZeroMemory(&sCamera, sizeof(CCamera::SDesc));
		sCamera.pKey = TEXT("Edit");
		sCamera.fFovy = XMConvertToRadians(60.0f);
		sCamera.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
		sCamera.fNear = 0.2f;
		sCamera.fFar = 500.f;
		hr |= pGame_Instance->Create_Object_Clone(TEXT("Camera_Edit"), static_cast<_ubyte>(EScene::LOGO), TEXT("Camera_Edit"), &sCamera);
	}
	else {
		CCamera::SDesc sCamera;
		ZeroMemory(&sCamera, sizeof(CCamera::SDesc));
		sCamera.pKey = TEXT("Edit");
		sCamera.fFovy = XMConvertToRadians(60.0f);
		sCamera.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
		sCamera.fNear = 0.2f;
		sCamera.fFar = 500.f;
		hr |= pGame_Instance->Create_Object_Clone(TEXT("Camera_Edit"), static_cast<_ubyte>(EScene::LOGO), TEXT("Camera_Edit"), &sCamera);
	}

	ifstream ifs(TEXT("../Bin/Resource/01.Logo/Object.txt"));
	Json::Value root;
	ifs >> root;
	ifs.close();
	for (_uint i = 0; i < root.size(); ++i) {
		string str = root[i]["Name"].asString();
		wstring wstr(str.begin(), str.end());
		CObject* pObject = pGame_Instance->CreateGet_Object_Clone(wstr.c_str(), static_cast<_ubyte>(EScene::LOGO), wstr.c_str());
		pObject->Load_Data(root[i]);
	}

	ifstream ifs2(TEXT("../Bin/Resource/Network.txt"));
	Json::Value root2;
	ifs2 >> root2;
	ifs2.close();
	_bool Server = root2["Server"].asBool();
	string ServerIp = root2["ServerIp"].asString();
	_uint Ip = root2["Ip"].asInt();
	_uint Id = root2["Id"].asInt();
	_uint Port = root2["Port"].asInt();
	switch (Server) {
	case true: {
		m_pNetwork->Open_Server(Port, Id);
		break;
	}
	case false: {
		m_pNetwork->Open_Client(ServerIp.c_str(), Port, Ip, Id);
		break;
	}
	}

	CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Setting_BGM"), static_cast<_uint>(EScene::STATIC), TEXT("Setting_BGM")));
	pBGM->Play_BGM(TEXT("mu_menu_archives_02"), 0.1f, 3.f);
	return hr;
}

void CScene_Logo::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CInfo* pInfo = CInfo::Get_Instance();
}

void CScene_Logo::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (g_bChangeScene) {
		g_bChangeScene = false;

		CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::STATIC), TEXT("Setting_BGM")));
		pBGM->Play_BGM(nullptr, 1.f, 2.f);

		pGame_Instance->Change_Scene(static_cast<_ubyte>(EScene::LOADING), CLoading::Create(m_pDevice, m_pContext, m_pNetwork, EScene::LOBBY));
	}
}
