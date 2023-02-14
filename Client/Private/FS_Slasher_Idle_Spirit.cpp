#include "pch.h"
#include "FS_Slasher_Idle_Spirit.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"

CFS_Slasher_Idle_Spirit::CFS_Slasher_Idle_Spirit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Idle_Spirit::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Idle_Spirit::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Idle_Spirit::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	return S_OK;
}

const _ubyte  CFS_Slasher_Idle_Spirit::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
		pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, m_pSlasher_Huntress->m_fSpiritGauge, false);

		if (m_pSlasher_Huntress->m_fSpiritGauge <= 0.f) {
			m_pSlasher_Huntress->m_bSpirit = false;
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_activephasing_flash"), 2, -1, 0.3f, FMOD_2D);
			pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, false, false);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Stop_Sound(6);
			return CFM_Slasher::EState::SPIRIT_OUT;
		}
		else
			m_pSlasher_Huntress->m_fSpiritGauge -= static_cast<_float>(dTimeDelta) * 35.f;

		// Input
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W))
			m_pSlasher_Huntress->m_bMove[0] = false;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S))
			m_pSlasher_Huntress->m_bMove[1] = false;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D))
			m_pSlasher_Huntress->m_bMove[2] = false;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A))
			m_pSlasher_Huntress->m_bMove[3] = false;

		if ((false == m_pSlasher_Huntress->m_bMove[0] || false == m_pSlasher_Huntress->m_bMove[1])
			&& (false == m_pSlasher_Huntress->m_bMove[2] || false == m_pSlasher_Huntress->m_bMove[3])) {
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_W) & 0x80)
				return CFM_Slasher::EState::RUN_SPIRIT_FT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_S) & 0x80)
				return CFM_Slasher::EState::RUN_SPIRIT_BK;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_D) & 0x80)
				return CFM_Slasher::EState::RUN_SPIRIT_RT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_A) & 0x80)
				return CFM_Slasher::EState::RUN_SPIRIT_LT;
		}

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			m_pSlasher_Huntress->m_bSpirit = false;
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");
			m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Set_MoveDir");
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_activephasing_flash"), 2, -1, 0.3f, FMOD_2D);
			pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, false, false);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Stop_Sound(6);
			return CFM_Slasher::EState::ATTACK_SLASH_IN;
		}
		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R)) {
			m_pSlasher_Huntress->m_bSpirit = false;
			m_pSlasher_Huntress->m_pFM->Update_Network("Set_S_MoveDir");
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_activephasing_flash"), 2, -1, 0.3f, FMOD_2D);
			pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, false, false);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Stop_Sound(6);
			return CFM_Slasher::EState::SPIRIT_OUT;
		}
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
	}
	else {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Idle_Spirit::Get_Dir(void) {
	return m_vDir;
}
