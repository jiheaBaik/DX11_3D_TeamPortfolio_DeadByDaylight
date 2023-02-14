#include "pch.h"
#include "FS_Camper_Ending.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Image.h"
#include "Camera_Camper.h"
#include "UI_End_Panel_First.h"
#include "UI_Play_Panel_Manager.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Score.h"
#include "Setting_BGM.h"

CFS_Camper_Ending::CFS_Camper_Ending(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Ending::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Ending::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	if (m_pCamper_GamePlay->m_bControl) {
		if (false == m_bSoundInit) {
			m_bSoundInit = true;
			m_pCamper_GamePlay->m_bPlayBGM = false;
			m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("mu_menu_victory"), -1, 0.05f, FMOD_2D);
			//hwang - score
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::SURVIVAL_SURVIVED));
				pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::SURVIVAL), 5000);
			}
		}
		m_pCamper_GamePlay->Set_End(true);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		// end state
		dynamic_cast<CUI_End_Panel_First*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_End_Panel_First"))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ECamper::ESCAPE));
		CCamera_Camper* pCameraCamper = dynamic_cast<CCamera_Camper*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::GAMEPLAY), TEXT("Camera_Camper")));
		pCameraCamper->Set_Estate(CCamera_Camper::ECamState::ENDING_EXIT);
	}

	m_pCamper_GamePlay->m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	return S_OK;
}

const _ubyte  CFS_Camper_Ending::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();


	if (m_pCamper_GamePlay->m_bControl) {
		CUI_Play_Panel_Manager* pPanelMgr = dynamic_cast<CUI_Play_Panel_Manager*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Panel_Manager"));

		m_EndingTime += dTimeDelta;
		if (m_EndingTime < 3.f) {
			dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Active(false);
			m_pCamper_GamePlay->m_pTransform->Translate(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.f, dTimeDelta);
		}
		else if (m_EndingTime < 5.f) {
			if (false == m_bUI_First) {
				m_bUI_First = true;
				pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END1, true, CUI_Play_Panel_Manager::EPanel::PLAY2);

				CInfo* pInfo = CInfo::Get_Instance();
				pInfo->Add_CamperCount(-1);
			}
			m_pCamper_GamePlay->m_pTransform->Translate(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.f, dTimeDelta);
		}
		else if (m_EndingTime < 8.f) {
			if (false == m_bUI_FadeIn) {
				m_bUI_FadeIn = true;
				pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::FADE, true);
			}
			m_pCamper_GamePlay->m_pTransform->Translate(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.f, dTimeDelta);
		}
		else if (m_EndingTime < 13.f) {
			if (false == m_bUI_FadeOut) {
				m_bUI_FadeOut = true;
				CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Setting_BGM")));
				pBGM->Play_BGM(TEXT("mu_menu_tallyscreen"), 0.05f, 2.f);
				CCamera::Get_Camera(TEXT("Ending"))->Set_Camera(0);
				pPanelMgr->Get_Panel(CUI_Play_Panel_Manager::EPanel::FADE)->Set_Render(false);
			}
		}
		else if (m_EndingTime < 18.f) {
			if (false == m_bUI_Second) {
				m_bUI_Second = true;
				pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END2, true, CUI_Play_Panel_Manager::EPanel::END);
			}
		}
	}
	else {
		m_EndingTime += dTimeDelta;
		if (m_EndingTime < 3.f) {
			m_pCamper_GamePlay->m_pTransform->Translate(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.f, dTimeDelta);
		}
		else if (m_EndingTime < 5.f) {
			if (false == m_bUI_First) {
				m_bUI_First = true;
			}
			m_pCamper_GamePlay->m_pTransform->Translate(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.f, dTimeDelta);
		}
		else if (m_EndingTime < 8.f) {
			if (false == m_bUI_FadeIn) {
				m_bUI_FadeIn = true;
			}
			m_pCamper_GamePlay->m_pTransform->Translate(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.f, dTimeDelta);
		}
		else if (m_EndingTime < 13.f) {
			if (false == m_bUI_FadeOut) {
				m_bUI_FadeOut = true;
			}
		}
		else if (m_EndingTime < 18.f) {
			if (false == m_bUI_Second) {
				m_bUI_Second = true;
			}
		}
	}
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
