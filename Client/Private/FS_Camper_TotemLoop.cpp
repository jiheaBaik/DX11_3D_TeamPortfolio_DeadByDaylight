#include "pch.h"
#include "FS_Camper_TotemLoop.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Totem.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"

CFS_Camper_TotemLoop::CFS_Camper_TotemLoop(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_TotemLoop::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_TotemLoop::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_dTotemTime = 0.;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pTotem = static_cast<CInter_Totem*>(m_pCamper_GamePlay->m_pTarObject);
		// set hud
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
			Set_HudOn(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::CANCLE), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Inter(CUI_Play_Gauge_Inter::EInter::TOTEM, true);

	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Totem"), m_pCamper_GamePlay->m_iTarObjID);
		m_pTotem = dynamic_cast<CInter_Totem*>(m_pCamper_GamePlay->m_pTarObject);
	}
	return S_OK;
}

const _ubyte  CFS_Camper_TotemLoop::Update(const _double& dTimeDelta) {

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		// set hud
		CUI_Play_Icon_Hud* pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"));
		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));

		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			//토템정화 ui렌더 x
			pHud->Set_HudOff(CUI_Play_Icon_Hud::EDiv::CENTER, true);
			pGauge->Set_Inter(CUI_Play_Gauge_Inter::EInter::TOTEM, false);

			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}
		m_dTotemTime += dTimeDelta;//14
		if (m_dTotemTime > 1.) {
			m_pTotem->Add_Gauge(7.143f);
			m_dTotemTime = 0.;
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_totem_bones"), 6, -1, 0.5f, FMOD_2D);

		}

		if (m_pTotem->Get_Gauge() >= 100.) {
			//토템정화 ui렌더 x
			pHud->Set_HudOff(CUI_Play_Icon_Hud::EDiv::CENTER, true);
			pGauge->End_Gauge(CUI_Play_Gauge_Inter::EInter::TOTEM);

			return CFM_Camper::EState::TOTEM_OUT;
		}
	}
	else {
		m_dTotemTime += dTimeDelta;//14
		if (m_dTotemTime > 1.) {
			m_dTotemTime = 0.;
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_totem_bones"), 6, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		}

		if (m_pTotem->Get_Gauge() >= 100.) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_BlessTotem_Out");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_BlessTotem_Out");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_BlessTotem_Out");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_BlessTotem_Out");
				break;
			}
		}
	}


	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
