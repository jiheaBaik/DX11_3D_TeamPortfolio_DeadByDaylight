#include "pch.h"
#include "FS_Slasher_Idle_Hunt.h"
#include "Slasher_Huntress.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Idle_Hunt::CFS_Slasher_Idle_Hunt(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Idle_Hunt::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Idle_Hunt::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Idle_Hunt::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_dFootTime = 0.;

	if (19 == iAnimIndex) {
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(31);
	}
	/*else if (9 == iAnimIndex) {
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(31);
	}*/
	else {
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);

	/*if (true == m_pSlasher_Huntress->m_bControl)
		m_pSlasher_Huntress->Update_Network("Set_State");*/
	return S_OK;
}

const _ubyte  CFS_Slasher_Idle_Hunt::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
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
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_W))
				return CFM_Slasher::EState::RUN_HUNT_FT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_S))
				return CFM_Slasher::EState::RUN_HUNT_BK;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_D))
				return CFM_Slasher::EState::RUN_HUNT_RT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_A))
				return CFM_Slasher::EState::RUN_HUNT_LT;
		}

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			// throw cancle
			m_pSlasher_Huntress->m_bCancle = true;
			m_pSlasher_Huntress->Update_Network("Set_Cancle");
			
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_swap_blade"), 4, -1, 0.3f, FMOD_2D);
			// set hud : cancle
			(0 < m_pSlasher_Huntress->m_iWeaponCount) ?
				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::CENTER, true) :
				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::CANCLE), true, true);

			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");
			return CFM_Slasher::EState::IDLE_SLASH;
		}
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::R) && m_pSlasher_Huntress->m_iWeaponCount > 0) {
			m_pSlasher_Huntress->m_pFM->Update_Network("Set_T_MoveDir");
			return CFM_Slasher::EState::THROW_SWING;
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

const _float3& CFS_Slasher_Idle_Hunt::Get_Dir(void) {
	return m_vDir;
}
