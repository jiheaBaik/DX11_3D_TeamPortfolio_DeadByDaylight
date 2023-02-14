#include "pch.h"
#include "FS_Camper_HatchOut.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Hatch.h"
#include "UI_Play_Panel_Manager.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Score.h"
#include "Setting_BGM.h"

CFS_Camper_HatchOut::CFS_Camper_HatchOut(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HatchOut::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HatchOut::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	if (m_pCamper_GamePlay->m_bControl) {

		m_pHatch = static_cast<CInter_Hatch*>(m_pCamper_GamePlay->m_pTarObject);
		//hwang - score
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
		if (pScore != nullptr) {
			pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::OBJECTIVE_HATCH_ESCAPE));
			pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::OBJECTIVE), 2000);
			pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::SURVIVAL_SURVIVED));
			pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::SURVIVAL), 5000);
		}
		m_pCamper_GamePlay->Set_End(true);
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Hatch"), m_pCamper_GamePlay->m_iTarObjID);
		m_pHatch = dynamic_cast<CInter_Hatch*>(m_pCamper_GamePlay->m_pTarObject);
	}

	m_pHatchTransform = dynamic_cast<CTransform*>(m_pHatch->Get_Component(TEXT("Transform")));
	return S_OK;
}

const _ubyte  CFS_Camper_HatchOut::Update(const _double& dTimeDelta) {
	if (!m_bAnimEnd) {
		XMMATRIX HatchMatrix = m_pHatchTransform->Get_Matrix();
		_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
		_vector vLerpPos = HatchMatrix.r[3] * 0.2f + vPos * 0.8f;
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

		_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
		_vector vLockerQaut = XMQuaternionRotationMatrix(HatchMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vLockerQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
	}
	if (true == m_pCamper_GamePlay->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_bAnimEnd = true;
			if (false == m_bCheck) {
				m_bCheck = true;
				CInfo* pInfo = CInfo::Get_Instance();
				pInfo->Add_CamperCount(-1);

				// Hatch Ending UI On
				dynamic_cast<CUI_End_Panel_First*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_End_Panel_First"))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ECamper::ESCAPE), true);
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
					m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("mu_menu_victory"), -1, 0.05f, FMOD_2D);
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
			m_bAnimEnd = true;
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(10.f, 0.f, 10.f, 0.f));
		}
	}
	return CFM_Camper::EState::END;
}
