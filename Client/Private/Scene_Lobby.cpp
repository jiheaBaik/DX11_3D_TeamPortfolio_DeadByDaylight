#include "pch.h"
#include "Scene_Lobby.h"
#include "Loading.h"
#include "Camper_Lobby.h"
#include "Inven.h"
#include "Setting_BGM.h"

CScene_Lobby* const CScene_Lobby::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CScene_Lobby* pInstnace = new CScene_Lobby(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CScene_Lobby::CScene_Lobby(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CScene(pDevice, pContext, pNetwork) {
}

void CScene_Lobby::Delete(void) {
	
	__super::Delete();
}

const HRESULT CScene_Lobby::Init(void) {
	OutputDebugString(TEXT("Scene : Lobby\n"));
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;

	CCamera::SDesc sCamera_Desc;
	sCamera_Desc.pKey = TEXT("Lobby");
	sCamera_Desc.fFovy = XMConvertToRadians(60.0f);
	sCamera_Desc.fAspect = 1280.f / 720.f;
	sCamera_Desc.fNear = 0.2f;
	sCamera_Desc.fFar = 500.f;
	hr |= pGame_Instance->Create_Object_Clone(TEXT("Camera_Lobby"), static_cast<_ubyte>(EScene::LOBBY), TEXT("Camera_Lobby"), &sCamera_Desc);

	//Object
	ifstream ifs(TEXT("../Bin/Resource/02.Lobby/Object.txt"));
	Json::Value root;
	ifs >> root;
	ifs.close();
	for (_uint i = 0; i < root.size(); ++i) {
		string str = root[i]["Name"].asString();
		wstring wstr(str.begin(), str.end());
		CObject* pObject = pGame_Instance->CreateGet_Object_Clone(wstr.c_str(), static_cast<_ubyte>(EScene::LOBBY), wstr.c_str());
		pObject->Load_Data(root[i]);
	}

	//Network
	Update_Network("Create_Camper_Lobby");

	//Component
	CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer")));
	pRenderer->Set_Bake(true);
	CNavi* pNavi = dynamic_cast<CNavi*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::LOBBY), TEXT("Navi")));
	pNavi->Set_DivCell(1);

	CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::STATIC), TEXT("Setting_BGM")));
	pBGM->Play_BGM(TEXT("mu_menu_lobby_k20"), 0.1f, 15.f);

	CCompute::Get_Instance()->Set_FogEnd(0.2f);
	return hr;
}

void CScene_Lobby::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = pGame_Instance->Get_Instance();
	CInfo* pInfo = CInfo::Get_Instance();
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F8)) {
		pInfo->Set_Ready5();

		CInven* pInven = CInven::Get_Instance();
		if (pInfo->Get_Player()) {
			pInven->Ready_Perk_Slasher();
		}
		else {
			pInven->Ready_Perk_Camper();
		}
	}

	if (5 == pInfo->Get_Ready()) {
		m_fStartTime += static_cast<_float>(dTimeDelta);
		//CInfo* pInfo = CInfo::Get_Instance();
		//if (5 == pInfo->Get_Ready()) {
		//	if (m_fSoundVolume == 0.f) {
		//		CAudio* pAudio = dynamic_cast<CAudio*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_uint>(EScene::STATIC), TEXT("Audio")));
		//		pAudio->Play_Sound(TEXT("sfx_ui_ready_all_01"), -1, 0.8f, FMOD_2D);
		//		pAudio->Play_Sound(TEXT("sfx_ui_heartbeat_01"), 0, 0.8f - m_fSoundVolume, FMOD_2D | FMOD_LOOP_NORMAL);
		//		pAudio->Set_Volume(0, 0.8f - m_fSoundVolume);
		//	}
		//	m_fSoundVolume += static_cast<_float>(dTimeDelta) * 0.2f;
		//}
		//else
		//	m_fSoundVolume = 0.f;
	}
	if (m_fStartTime > 3.f) {
		CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::STATIC), TEXT("Setting_BGM")));
		pBGM->Play_BGM(nullptr, 1.f, 2.f);

		pGame_Instance->Change_Scene(static_cast<_ubyte>(EScene::LOADING), CLoading::Create(m_pDevice, m_pContext, m_pNetwork, EScene::GAMEPLAY));
		return;
	}
	Update_Network();
}

void CScene_Lobby::Late_Update(const _double& dTimeDelta) {
}

void CScene_Lobby::Update_Network(const string& pKey) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	Json::Value root;
	if ("Create_Camper_Lobby" == pKey) {
		CObject* pCamper = pGame_Instance->CreateGet_Object_Clone(TEXT("Camper_Lobby"), static_cast<_ubyte>(EScene::LOBBY), TEXT("Camper_Lobby"));
		pCamper->Set_Control(true);
		pCamper->Set_Id(m_pNetwork->Get_Id());
		root["CamperId"] = pCamper->Get_Id();
		m_pNetwork->Set_Send(0, pKey, root);
	}
};

void CScene_Lobby::Update_Network(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<Json::Value>& list = m_pNetwork->Get_Recv(0);
	for (auto root = list.begin(); root != list.end();) {
		string Key = (*root)["Key"].asString();
		if ("Create_Camper_Lobby" == Key) {
			CObject* pCamper = pGame_Instance->CreateGet_Object_Clone(TEXT("Camper_Lobby"), static_cast<_ubyte>(EScene::LOBBY), TEXT("Camper_Lobby"));
			pCamper->Set_Control(false);
			pCamper->Set_Id((*root)["CamperId"].asInt());
			root = list.erase(root);
		}
		else
			++root;
	}
};