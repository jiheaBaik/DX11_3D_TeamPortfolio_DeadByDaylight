#include "pch.h"
#include "FS_Camper_HookRescueIn.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_HookRescueIn::CFS_Camper_HookRescueIn(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HookRescueIn::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookRescueIn::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);

	if (m_pCamper_GamePlay->m_bControl) {
		m_pCamper_Target = static_cast<CCamper_GamePlay*>(m_pCamper_GamePlay->m_pTarObject);
		m_pHook = static_cast<CInter_Hook*>(m_pCamper_Target->m_pTarObject);
		
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 0.3f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 0.3f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 0.3f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 0.3f, FMOD_2D);
			break;
		}

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, -1);
		//dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Gauge_Inter")))->Set_Inter(CUI_Play_Gauge_Inter::EInter::RESQUE, true);
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), m_pCamper_GamePlay->m_iTarObjID);
		m_pCamper_Target = static_cast<CCamper_GamePlay*>(m_pCamper_GamePlay->m_pTarObject);
		m_pHook = static_cast<CInter_Hook*>(m_pCamper_Target->m_pTarObject);
		
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		}

	}

	return S_OK;
}

const _ubyte  CFS_Camper_HookRescueIn::Update(const _double& dTimeDelta) {
	
	_vector vOtherCampPos = m_pCamper_Target->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vCampPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLook = vCampPos - vOtherCampPos;
	_vector vLook2 = XMVectorSetY(vOtherCampPos - vCampPos, 0.f);
	_vector vDist = XMVector3Normalize(vLook) * 0.7f;
	_vector vLerpPos = (vOtherCampPos + vDist) * 0.2f + vCampPos * 0.8f;

	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);
	m_pCamper_GamePlay->m_pTransform->LookAt(vCampPos + vLook2);

	//m_pCamper_Target->m_pTransform->LookAt(vOtherCampPos + vLook2 * -1.f);

	if (m_pCamper_GamePlay->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
				m_pHook->Set_State(CInter_Hook::ESTATE::SPREAD);
				m_pCamper_Target->Get_FM_Camper()->Set_State(CFM_Camper::EState::HOOKIDLE);
				m_pCamper_Target->Get_FM_Camper()->Update_Network("Set_State");
		
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}
		_float fRescueGuage = 0.f;
		fRescueGuage = (_float)m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc() / (_float)m_pCamper_GamePlay->m_pModel->Get_Anim_Duration() * 100.f;

		//CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Gauge_Inter")));
		//pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::RESQUE, fRescueGuage);

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)){
			//pGauge->End_Gauge(CUI_Play_Gauge_Inter::EInter::RESQUE);

			m_pCamper_Target->m_pFM->Set_State(CFM_Camper::EState::HOOKRESCUEEND_BEEING);
			m_pCamper_Target->Get_FM_Camper()->Update_Network("Set_State");
			return CFM_Camper::EState::HOOKRESCUEEND;
		}
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_HookRescueCamperEnd");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_HookRescueCamperEnd");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_HookRescueCamperEnd");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_HookRescueCamperEnd");
				break;
			}
		}
	}

	
	return CFM_Camper::EState::END;
}
