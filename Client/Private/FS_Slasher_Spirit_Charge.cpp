#include "pch.h"
#include "FS_Slasher_Spirit_Charge.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Panel_Second.h"

CFS_Slasher_Spirit_Charge::CFS_Slasher_Spirit_Charge(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Spirit_Charge::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Spirit_Charge::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Spirit_Charge::Init(_uint iAnimIndex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	if (true == m_pSlasher_Huntress->m_bControl) {
		//hwang - score
		CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
		if (pScore != nullptr) {
			pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::DEVIOUSNESS_HAUNTING));
			pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::DEVIOUSNESS), 200);
		}

		m_pSlasher_Huntress->m_pAudio->Play_Sound(TEXT("sfx_activephasing_ready_01"), -1, 0.3f, FMOD_2D);

		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
			Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1));
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, true, true);
	}
	else
		m_pSlasher_Huntress->m_pAudio->Play_Sound(TEXT("sfx_activephasing_ready_01"), -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

	m_pSlasher_Huntress->m_bSaturation = true;

	//if (true == m_pSlasher_Huntress->m_bControl) {
	//	CUI_Play_Icon_Hud* pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"));
	//	pHud->Set_AllHuds(false/*, -1*/);
	//	pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::CANCLE), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
	//	dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_RenderAll(false, false);
	//}

	return S_OK;
}

const _ubyte  CFS_Slasher_Spirit_Charge::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		CCompute* pCompute = CCompute::Get_Instance();
		if (pCompute->Get_Saturation() >= 0.f) {
			pCompute->Add_Saturation(-dTimeDelta * 1.2f);
		}
		else
			pCompute->Set_Saturation(0.f);

		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(11)) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, 5, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
		}
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(22)) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, 5, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
		}
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(33)) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, 5, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
		}

		_double dSpiritGauge = 0.;
		dSpiritGauge = m_pSlasher_Huntress->m_pModel->Get_Anim_TimeAcc() / m_pSlasher_Huntress->m_pModel->Get_Anim_Duration() * 100.;

		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
		pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, static_cast<_float>(dSpiritGauge), false);

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			dSpiritGauge = 100.;
			m_pSlasher_Huntress->m_fSpiritGauge = static_cast<_float>(dSpiritGauge);

			m_pSlasher_Huntress->m_pAudio->Play_Sound(TEXT("ActivePhasing_Radius_Mix_Bounce"), 6, 0.04f, FMOD_2D | FMOD_LOOP_NORMAL);
			return CFM_Slasher::EState::IDLE_SPIRIT;
		}
	}
	else {
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(11)) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, 5, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(22)) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, 5, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(33)) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, 5, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
