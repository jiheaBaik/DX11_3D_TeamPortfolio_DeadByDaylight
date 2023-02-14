#include "pch.h"
#include "FS_Camper_HealingSelf.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_HealingSelf::CFS_Camper_HealingSelf(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_HealingSelf::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HealingSelf::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_dHealTime = 0.;
	m_pCamper_GamePlay->m_bIsMedikitPerk = false;
	// set hud
	if (m_pCamper_GamePlay->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
			Set_HudOn(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::CANCLE), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::HEALSEL, true, true);
	}
	return S_OK;
}

const _ubyte  CFS_Camper_HealingSelf::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl)
	{
		// set hud
		CUI_Play_Icon_Hud* pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"));
		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));

		//m_SelfHealTime
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::HEALSEL, false, false);
			return CFM_Camper::EState::HIT_IDLE;
		}
		//hwang-perk
		CInven* pInven = CInven::Get_Instance();
		if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::SELF_HEAL))
			&& !m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT]) {
			m_pCamper_GamePlay->m_bIsMedikitPerk = true;
			m_dSelfHeal = 0.5f;
		}
		else
			m_dSelfHeal = 1.0;

		m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc();
		m_pCamper_GamePlay->m_pModel->Get_Anim_Duration();
		m_dHealTime += dTimeDelta;
		if (m_dHealTime > 1.) {//1.65
			m_dHealTime = 0.;
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_survivor_healing"), 6, -1, 0.5f, FMOD_2D);
			m_pCamper_GamePlay->m_SelfHealGauge += 3.29f * m_dSelfHeal;
		}
		// set gauge
		pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::HEALSEL, static_cast<_float>(m_pCamper_GamePlay->m_SelfHealGauge));

		if (m_pCamper_GamePlay->m_SelfHealGauge >= 100.f) {
			m_pCamper_GamePlay->m_SelfHealGauge = 0.f;

			dynamic_cast<CUI_Play_Icon_Passive*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("UI_Play_Icon_Passive")))->Pop_Slot();
			pHud->Set_HudOff(CUI_Play_Icon_Hud::EDiv::CENTER, true);
			//hwang - score
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::SURVIVAL_HEAL));
				pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::SURVIVAL), 300);
			}
			return CFM_Camper::EState::IDLE;
		}

	}
	else {
		m_dHealTime += dTimeDelta;
		if (m_dHealTime > 1.) {//1.65
			m_dHealTime = 0.;
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_survivor_healing"), 6, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 3.f });
		}
	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
