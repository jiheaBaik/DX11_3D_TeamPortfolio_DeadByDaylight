#include "pch.h"
#include "Scene_GamePlay.h"
#include "Loading.h"
#include "UI_Image.h"
#include "Inven.h"
#include "Ending_Camper.h"
#include "Ending_PlayerPos.h"
#include "Ending_Slasher.h"
#include "Setting_BGM.h"
#include "UI_Play_Panel_Manager.h"
#include "Setting_BGM.h"

CScene_GamePlay* const CScene_GamePlay::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CScene_GamePlay* pInstnace = new CScene_GamePlay(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CScene_GamePlay::CScene_GamePlay(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CScene(pDevice, pContext, pNetwork) {
}

void CScene_GamePlay::Delete(void) {
	__super::Delete();
	Safe_Release(m_pAudio);
}

const HRESULT CScene_GamePlay::Init(void) {
	OutputDebugString(TEXT("Scene : GamePlay\n"));
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	HRESULT hr = S_OK;

	m_pAudio = dynamic_cast<CAudio*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio")));

	if (CEdit_Mgr::EMode::EDIT == pGame_Instance->Get_Edit_Mode()) {
		CCamera::SDesc sCamera;
		ZeroMemory(&sCamera, sizeof(CCamera::SDesc));
		sCamera.pKey = TEXT("Edit");
		sCamera.fFovy = XMConvertToRadians(60.0f);
		sCamera.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
		sCamera.fNear = 0.2f;
		sCamera.fFar = 500.f;
		hr |= pGame_Instance->Create_Object_Clone(TEXT("Camera_Edit"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camera_Edit"), &sCamera);
	}
	hr |= pGame_Instance->Create_Object_Clone(TEXT("Inter_Totem"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Totem"));
	//Object
	ifstream ifs(TEXT("../Bin/Resource/03.GamePlay/Object.txt"));
	Json::Value root;
	ifs >> root;
	ifs.close();
	for (_uint i = 0; i < root.size(); ++i) {
		string str = root[i]["Name"].asString();
		wstring wstr(str.begin(), str.end());
		CObject* pObject = pGame_Instance->CreateGet_Object_Clone(wstr.c_str(), static_cast<_ubyte>(EScene::GAMEPLAY), wstr.c_str());
		pObject->Load_Data(root[i]);
	}

	//Network
	CInfo* pInfo = CInfo::Get_Instance();
	switch (pInfo->Get_Player()) {
	case false:
		Update_Network("Create_Camper_GamePlay");
		break;
	case true:
		Update_Network("Create_Slasher_Huntress");
		break;
	}

	CRenderer* pRenderer = dynamic_cast<CRenderer*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer")));
	pRenderer->Set_Bake(true);
	CNavi* pNavi = dynamic_cast<CNavi*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Navi")));
	pNavi->Set_DivCell(3.f);

	pInfo->Init_CamperCount();
	m_iCamperCount = pInfo->Get_CamperCount();

	//hwang - create scoreUI
	//hr |= pGame_Instance->Create_Object_Clone(TEXT("UI_Play_Score"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score"));
	CCompute::Get_Instance()->Set_FogEnd(0.07f);

	return hr;
}

void CScene_GamePlay::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CInfo* pInfo = CInfo::Get_Instance();
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F7)) {
		m_bClear = true;
	}
	// murder ui set
	if (m_bClear)
		if (true == pInfo->Get_Player()) {
			if (0 == pInfo->Get_CamperCount()) {
				m_fEndingTime += static_cast<_float>(dTimeDelta);
				CUI_Play_Panel_Manager* pPanelMgr = dynamic_cast<CUI_Play_Panel_Manager*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Panel_Manager"));
				if (m_fEndingTime < 3.f) {
					pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::PLAY2, false);
					if (m_iCamperKillCount >= (m_iCamperCount / 2) + (m_iCamperCount % 2))
						dynamic_cast<CUI_End_Panel_First*>(pPanelMgr->Get_Panel(CUI_Play_Panel_Manager::EPanel::END1))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ESlasher::MURDER));
					else
						dynamic_cast<CUI_End_Panel_First*>(pPanelMgr->Get_Panel(CUI_Play_Panel_Manager::EPanel::END1))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ESlasher::PEACEFUL));

				}
				else if (m_fEndingTime < 5.f) {
					if (false == m_bUI_FadeIn) {
						m_bUI_FadeIn = true;
						pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::FADE, true);
					}
				}
				else if (m_fEndingTime < 8.f) {
					if (false == m_bUI_FadeOut) {
						m_bUI_FadeOut = true;
						CCamera::Get_Camera(TEXT("Ending"))->Set_Camera(0);
						m_pAudio->Play_Sound(TEXT("mu_menu_victory"), -1, 0.05f, FMOD_2D);
						pPanelMgr->Get_Panel(CUI_Play_Panel_Manager::EPanel::FADE)->Set_Render(false);
					}
				}
				else if (m_fEndingTime < 11.f) {
					if (false == m_bUI_First) {
						m_bUI_First = true;

						pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END1, true);
					}
				}
				else if (m_fEndingTime < 15.f) {
					if (false == m_bUI_Second) {
						m_bUI_Second = true;
						CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Setting_BGM")));
						pBGM->Play_BGM(TEXT("mu_menu_tallyscreen"), 0.05f, 2.f);
						pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END2, true, CUI_Play_Panel_Manager::EPanel::END);
					}
				}


			}
		}

	Update_Network();
}

void CScene_GamePlay::Late_Update(const _double& dTimeDelta) {
	//CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//if (g_bChangeScene) {
	//	pGame_Instance->Change_Scene(static_cast<_ubyte>(EScene::LOADING), CLoading::Create(m_pDevice, m_pContext, m_pNetwork, EScene::LOBBY));
	//	g_bChangeScene = false;
	//}
}

void CScene_GamePlay::Update_Network(const string& pKey) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//pGame_Instance->Getsce
	CInfo* pInfo = CInfo::Get_Instance();
	Json::Value root;
	if ("Create_Camper_GamePlay" == pKey) {
		CInfo::ECamper eCamper = pInfo->Get_Camper();

		CObject* pCamper = pGame_Instance->CreateGet_Object_Clone(TEXT("Camper_GamePlay"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), &eCamper);
		CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
		_uint iMyCamperSlot = pInfo->Get_MyCamperSlot();
		root["MyCamperSlot"] = iMyCamperSlot;
		CObject* plistSetting = pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Setting_CamperPos"), iMyCamperSlot);
		CTransform* pSettingTransform = dynamic_cast<CTransform*>(plistSetting->Get_Component(TEXT("Transform")));
		pCamperTransform->Set_Row(CTransform::ERow::POS, pSettingTransform->Get_Row(CTransform::ERow::POS));

		pCamper->Set_Control(true);
		pCamper->Set_Id(m_pNetwork->Get_Id());
		root["Camper_Id"] = pCamper->Get_Id();

		CCamera::SDesc sCamera;
		sCamera.pKey = TEXT("Camper");
		CCamera* pCamera = dynamic_cast<CCamera*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Camera_Camper"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camera_Camper"), &sCamera));
		if (CEdit_Mgr::EMode::PLAY == pGame_Instance->Get_Edit_Mode())
			pCamera->Set_Camera(0);

		root["ECamper"] = static_cast<_int>(eCamper);
		CEnding_Camper* pEndingCamper = dynamic_cast<CEnding_Camper*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Ending_Camper"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_Camper"), &eCamper));
		CTransform* pEndingCamperTransform = dynamic_cast<CTransform*>(pEndingCamper->Get_Component(TEXT("Transform")));
		list<CObject*>* plistPlayerPos = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_PlayerPos"));
		for (auto& iter : *plistPlayerPos) {
			CEnding_PlayerPos* pEndingPlayerPos = dynamic_cast<CEnding_PlayerPos*>(iter);
			if (false == pEndingPlayerPos->Get_Using()) {
				CTransform* pEndingPlayerPosTransform = dynamic_cast<CTransform*>(pEndingPlayerPos->Get_Component(TEXT("Transform")));
				pEndingCamperTransform->Set_Matrix(pEndingPlayerPosTransform->Get_Matrix());
				pEndingPlayerPos->Set_Using(true);
				break;
			}
		}

		CLight::SDesc sLight; //Light
		sLight.pKey = TEXT("Eye");
		sLight.eType = CLight::EType::SPOT;
		sLight.vDiffuse = _float4{ 1.f, 0.f, 0.f, 1.f };
		sLight.vAmbient = _float4{ 1.f, 0.f, 0.f, 1.f };
		sLight.vSpecular = _float4{ 0.f, 0.f, 0.f, 0.f };
		sLight.fRange = 5.f;
		sLight.fAngle = _float2{ 0.8f, 1.f };
		pGame_Instance->Create_Object_Clone(TEXT("Light_Slasher_Eye"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Light_Slasher_Eye"), &sLight);

		m_pNetwork->Set_Send(0, pKey, root);
	}
	if ("Create_Slasher_Huntress" == pKey) {
		CInfo::ESlasher eSlasher = pInfo->Get_Slasher();
		CObject* pSlasher = pGame_Instance->CreateGet_Object_Clone(TEXT("Slasher_Huntress"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), &eSlasher);
		pSlasher->Set_Control(true);
		pSlasher->Set_Id(m_pNetwork->Get_Id());
		root["Slasher_Id"] = pSlasher->Get_Id();
		root["ESlasher"] = static_cast<_int>(eSlasher);

		if (eSlasher == CInfo::ESlasher::HUNTRESS) {
			CObject* pSlasher_Weapon_Slash = pGame_Instance->CreateGet_Object_Clone(TEXT("Weapon_Slash"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Slash"));
			pSlasher_Weapon_Slash->Set_Control(true);
			pSlasher_Weapon_Slash->Set_Id(m_pNetwork->Get_Id());
			root["Slasher_Weapon_Slash_Id"] = pSlasher_Weapon_Slash->Get_Id();

			CObject* pSlasher_Weapon_Throw = pGame_Instance->CreateGet_Object_Clone(TEXT("Weapon_Throw"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Throw"));
			pSlasher_Weapon_Throw->Set_Control(true);
			pSlasher_Weapon_Throw->Set_Id(m_pNetwork->Get_Id());
			root["Slasher_Weapon_Throw_Id"] = pSlasher_Weapon_Throw->Get_Id();

			pGame_Instance->Create_Object_Clone(TEXT("Weapon_Hunt"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Hunt"));
		}
		else {
			CObject* pSlasher_Weapon_Spirit = pGame_Instance->CreateGet_Object_Clone(TEXT("Weapon_Spirit"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Spirit"));
			pSlasher_Weapon_Spirit->Set_Control(true);
			pSlasher_Weapon_Spirit->Set_Id(m_pNetwork->Get_Id());
			root["Slasher_Weapon_Spirit_Id"] = pSlasher_Weapon_Spirit->Get_Id();

			pGame_Instance->Create_Object_Clone(TEXT("Weapon_Spirit_Hand"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Spirit_Hand"));
		}

		CCamera::SDesc sCamera;
		sCamera.pKey = TEXT("Slasher");
		CCamera* pCamera = dynamic_cast<CCamera*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Camera_Slasher"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camera_Slasher"), &sCamera));
		if (CEdit_Mgr::EMode::PLAY == pGame_Instance->Get_Edit_Mode())
			pCamera->Set_Camera(0);

		CEnding_Slasher* pEndingSlasher = dynamic_cast<CEnding_Slasher*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Ending_Slasher"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_Slasher"), &eSlasher));
		CTransform* pEndingSlasherTransform = dynamic_cast<CTransform*>(pEndingSlasher->Get_Component(TEXT("Transform")));
		list<CObject*>* plistPlayerPos = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_PlayerPos"));
		for (auto& iter : *plistPlayerPos) {
			CEnding_PlayerPos* pEndingPlayerPos = dynamic_cast<CEnding_PlayerPos*>(iter);
			if (false == pEndingPlayerPos->Get_Using()) {
				CTransform* pEndingPlayerPosTransform = dynamic_cast<CTransform*>(pEndingPlayerPos->Get_Component(TEXT("Transform")));
				pEndingSlasherTransform->Set_Matrix(pEndingPlayerPosTransform->Get_Matrix());
				pEndingPlayerPos->Set_Using(true);
				break;
			}
		}

		m_pNetwork->Set_Send(0, pKey, root);
	}
}

void CScene_GamePlay::Update_Network(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<Json::Value>& root = m_pNetwork->Get_Recv(0);
	for (auto iter = root.begin(); iter != root.end();) {
		string key = (*iter)["Key"].asString();
		if ("Create_Camper_GamePlay" == key) {
			CInfo::ECamper eCamper = static_cast<CInfo::ECamper>((*iter)["ECamper"].asInt());

			CObject* pCamper = pGame_Instance->CreateGet_Object_Clone(TEXT("Camper_GamePlay"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), &eCamper);
			CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));

			_uint iMyCamperSlot = (*iter)["MyCamperSlot"].asInt();
			CObject* plistSetting = pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Setting_CamperPos"), iMyCamperSlot);
			CTransform* pSettingTransform = dynamic_cast<CTransform*>(plistSetting->Get_Component(TEXT("Transform")));
			pCamperTransform->Set_Row(CTransform::ERow::POS, pSettingTransform->Get_Row(CTransform::ERow::POS));

			pCamper->Set_Control(false);
			pCamper->Set_Id((*iter)["Camper_Id"].asInt());

			if (false == CInfo::Get_Instance()->Get_Player()) {
				CEnding_Camper* pEndingCamper = dynamic_cast<CEnding_Camper*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Ending_Camper"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_Camper"), &eCamper));
				CTransform* pEndingCamperTransform = dynamic_cast<CTransform*>(pEndingCamper->Get_Component(TEXT("Transform")));
				list<CObject*>* plistPlayerPos = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_PlayerPos"));
				for (auto& iter : *plistPlayerPos) {
					CEnding_PlayerPos* pEndingPlayerPos = dynamic_cast<CEnding_PlayerPos*>(iter);
					if (false == pEndingPlayerPos->Get_Using()) {
						CTransform* pEndingPlayerPosTransform = dynamic_cast<CTransform*>(pEndingPlayerPos->Get_Component(TEXT("Transform")));
						pEndingCamperTransform->Set_Matrix(pEndingPlayerPosTransform->Get_Matrix());
						pEndingPlayerPos->Set_Using(true);
						break;
					}
				}
			}
			iter = root.erase(iter);
		}
		else if ("Create_Slasher_Huntress" == key) {
			CInfo::ESlasher eSlasher = static_cast<CInfo::ESlasher>((*iter)["ESlasher"].asInt());
			CObject* pSlasher = pGame_Instance->CreateGet_Object_Clone(TEXT("Slasher_Huntress"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), &eSlasher);
			pSlasher->Set_Control(false);
			pSlasher->Set_Id((*iter)["Slasher_Id"].asInt());

			if (eSlasher == CInfo::ESlasher::HUNTRESS) {
				CObject* pSlasher_Weapon_Slash = pGame_Instance->CreateGet_Object_Clone(TEXT("Weapon_Slash"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Slash"));
				pSlasher_Weapon_Slash->Set_Control(false);
				pSlasher_Weapon_Slash->Set_Id((*iter)["Slasher_Weapon_Slash_Id"].asInt());

				CObject* pSlasher_Weapon_Throw = pGame_Instance->CreateGet_Object_Clone(TEXT("Weapon_Throw"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Throw"));
				pSlasher_Weapon_Throw->Set_Control(false);
				pSlasher_Weapon_Throw->Set_Id((*iter)["Slasher_Weapon_Throw_Id"].asInt());

				pGame_Instance->Create_Object_Clone(TEXT("Weapon_Hunt"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Hunt"));
			}
			else {
				CObject* pSlasher_Weapon_Spirit = pGame_Instance->CreateGet_Object_Clone(TEXT("Weapon_Spirit"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Spirit"));
				pSlasher_Weapon_Spirit->Set_Control(false);
				pSlasher_Weapon_Spirit->Set_Id((*iter)["Slasher_Weapon_Spirit_Id"].asInt());

				pGame_Instance->Create_Object_Clone(TEXT("Weapon_Spirit_Hand"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Weapon_Spirit_Hand"));
			}

			iter = root.erase(iter);
		}
		else {
			++iter;
		}
	}
}
