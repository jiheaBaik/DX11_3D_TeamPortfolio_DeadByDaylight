#include "pch.h"
#include "FS_Camper_HealCamper.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"
#include <iostream>
#include "UI_Play_Score.h"
#include "FS_Camper_HealCamper_Craw.h"


CFS_Camper_HealCamper_Craw::CFS_Camper_HealCamper_Craw(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HealCamper_Craw::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HealCamper_Craw::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	m_pCamper_GamePlay->m_HealGauge = 0.f;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pCamper_Target = static_cast<CCamper_GamePlay*>(m_pCamper_GamePlay->m_pTarObject);
		// set hud
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
			Set_HudOn(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::CANCLE), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Inter(CUI_Play_Gauge_Inter::EInter::HEALOTH, true);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), m_pCamper_GamePlay->m_iTarObjID);
		m_pCamper_Target = static_cast<CCamper_GamePlay*>(m_pCamper_GamePlay->m_pTarObject);
	}
	m_dHealTime = 0.;

	m_fTargetGauge = m_pCamper_Target->m_HealGauge;

	return S_OK;
}

const _ubyte  CFS_Camper_HealCamper_Craw::Update(const _double& dTimeDelta) {

	_vector vOtherCampPos = m_pCamper_Target->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vCampPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLook = vCampPos - vOtherCampPos;
	_vector vLook2 = XMVectorSetY(vOtherCampPos - vCampPos, 0.f);
	_vector vDist = XMVector3Normalize(vLook) * 1.f;
	_vector vLerpPos = (vOtherCampPos + vDist) * 0.2f + vCampPos * 0.8f;

	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);
	m_pCamper_GamePlay->m_pTransform->LookAt(vCampPos + vLook2);

	if (m_pCamper_GamePlay->m_bControl) {

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(CUI_Play_Icon_Hud::EDiv::CENTER, true);

			
				m_pCamper_Target->Get_FM_Camper()->Set_State(CFM_Camper::EState::CRAWL_IDLE);
				m_pCamper_Target->Get_FM_Camper()->Update_Network("Set_State");
		
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;

		}

		/*if (m_pCamper_Target->m_iHP == 2)
		{
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(CUI_Play_Icon_Hud::EDiv::CENTER, true);

			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::Idle;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::Hit_Idle;
		}*/
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(CUI_Play_Icon_Hud::EDiv::CENTER, true);
			m_pCamper_Target->Get_FM_Camper()->Set_State(CFM_Camper::EState::HIT_IDLE);
			m_pCamper_Target->Get_FM_Camper()->Update_Network("Set_State");
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;

		}
	}
	else {

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_inj_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_inj_StandIdle");
					break;
				}
		}
	}
	return CFM_Camper::EState::END;
}

