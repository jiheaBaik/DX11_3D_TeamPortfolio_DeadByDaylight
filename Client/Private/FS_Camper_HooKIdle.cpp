#include "pch.h"
#include "FS_Camper_HookIdle.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Hook.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_HookIdle::CFS_Camper_HookIdle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HookIdle::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookIdle::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	//탈출시도 ui RenderO
	if (m_pCamper_GamePlay->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Icon_Hud")))->Set_AllHuds(false, -1);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Gauge_Inter")))->Set_RenderAll(false, false);
	}
	return S_OK;
}

const _ubyte  CFS_Camper_HookIdle::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {
		m_HookTime += dTimeDelta;
		if (dTimeDelta >= 1.) {
			m_HookTime = 0.;
			
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_hooked"), 33, -1, 0.3f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_hooked"), 21, -1, 0.3f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_hooked"), 16, -1, 0.3f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_hooked"), 28, -1, 0.3f, FMOD_2D);
				break;
			}

		}
		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			m_pHook->Set_State(CInter_Hook::ESTATE::TRY_ESCAPE);
			return CFM_Camper::EState::HOOKSTRUGGLE;
		}

		if (m_pCamper_GamePlay->m_fSpiderRound != 2) {//체력 반
			m_pCamper_GamePlay->m_fSpiderTime -= static_cast<_float>(dTimeDelta);
			m_pCamper_GamePlay->Update_Network("Set_SpiderTime");

			if (m_pCamper_GamePlay->m_fSpiderTime < 30.f) {
				m_pHook->Set_State(CInter_Hook::ESTATE::SPREAD);
				return CFM_Camper::EState::SPIDER_IN;
			}
		}


	}
	else {
		m_HookTime += dTimeDelta;
		if (dTimeDelta >= 1.) {
			m_HookTime = 0.;
			
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_hooked"), 33, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_hooked"), 21, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_hooked"), 16, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_hooked"), 28, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
				break;
			}

		}
	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;

}
