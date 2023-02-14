#include "pch.h"
#include "FS_Camper_EndGame.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_EndGame.h"
#include "Slasher_Huntress.h"
#include "Camera_Camper.h"
#include "UI_Play_Panel_Manager.h"
#include "UI_Play_Gauge_Inter.h"
#include "Scene_GamePlay.h"
#include "Setting_BGM.h"

CFS_Camper_EndGame::CFS_Camper_EndGame(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_EndGame::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_EndGame::Init(_char* pModelName) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);

	typedef struct sEndGame {
		_int iCamperID;
		_float3 vPos;
	} ENDGAME;
	ENDGAME sEndGame;
	ZeroMemory(&sEndGame, sizeof(ENDGAME));

	sEndGame.iCamperID = m_pCamper_GamePlay->m_iId;
	sEndGame.vPos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);
	CInter_EndGame* pEndGame = dynamic_cast<CInter_EndGame*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Inter_EndGame"), pGame_Instance->Get_Scene(), TEXT("Inter_EndGame"), &sEndGame));

	if (m_pCamper_GamePlay->m_bControl) {
		CCamera_Camper* pCameraCamper = dynamic_cast<CCamera_Camper*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::GAMEPLAY), TEXT("Camera_Camper")));
		pCameraCamper->Set_Estate(CCamera_Camper::ECamState::ENDING_EXIT);
		m_pCamper_GamePlay->m_bPlayBGM = false;
		m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("mu_menu_defeated"), -1, 0.05f, FMOD_2D);
	}
	else {
		CScene_GamePlay* pScene = dynamic_cast<CScene_GamePlay*>(pGame_Instance->Get_Scene2());
		pScene->Plus_CamperKillCount();
	}

	return S_OK;
}

const _ubyte  CFS_Camper_EndGame::Update(const _double& dTimeDelta) {

	if (m_pCamper_GamePlay->m_bControl)
	{
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

			if (false == m_bCheck) {
				m_bCheck = true;
				CInfo* pInfo = CInfo::Get_Instance();
				pInfo->Add_CamperCount(-1);
				
				// EndGame Ending UI On
				dynamic_cast<CUI_End_Panel_First*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_End_Panel_First"))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ECamper::SACRIFICE));
			}

			CUI_Play_Panel_Manager* pPanelMgr = dynamic_cast<CUI_Play_Panel_Manager*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Panel_Manager"));

			m_dEndingTime += dTimeDelta;
			if (m_dEndingTime < 3.) {
				dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Active(false);
			}
			else if (m_dEndingTime < 6.) {
				if (false == m_bUI_FadeIn) {
					m_bUI_FadeIn = true;
					pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::FADE, true, CUI_Play_Panel_Manager::EPanel::PLAY2);

				}
			}
			else if (m_dEndingTime < 9.) {
				if (false == m_bUI_FadeOut) {
					m_bUI_FadeOut = true;
					CCamera* pCamera = CCamera::Get_Camera(TEXT("Ending"));
					pCamera->Set_Camera(0);
					m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(10.f, 0.f, 10.f, 1.f));

					pPanelMgr->Get_Panel(CUI_Play_Panel_Manager::EPanel::FADE)->Set_Render(false);
				}
			}
			else if (m_dEndingTime < 12.) {
				if (false == m_bUI_First) {
					m_bUI_First = true;
					m_pCamper_GamePlay->m_bPlayBGM = false;
					m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("mu_menu_defeated"), -1, 0.05f, FMOD_2D);
					pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END1, true);
				}
			}
			else if (m_dEndingTime < 15.) {
				if (false == m_bUI_Second) {
					m_bUI_Second = true;
					CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Setting_BGM")));
					pBGM->Play_BGM(TEXT("mu_menu_tallyscreen"), 0.05f, 2.f);
					pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END2, true, CUI_Play_Panel_Manager::EPanel::END);


				}
			}
		}

	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(10.f, 0.f, 10.f, 0.f));

		}

	}

	return CFM_Camper::EState::END;
}
