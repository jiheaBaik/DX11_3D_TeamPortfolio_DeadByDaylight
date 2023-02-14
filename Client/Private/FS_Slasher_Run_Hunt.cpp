#include "pch.h"
#include "FS_Slasher_Run_Hunt.h"
#include "Slasher_Huntress.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Run_Hunt::CFS_Slasher_Run_Hunt(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Run_Hunt::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Run_Hunt::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Run_Hunt::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	return S_OK;
}

const _ubyte  CFS_Slasher_Run_Hunt::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(6))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(19))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);

		_vector vLook = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 0.f));
		_vector vRight = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 0.f));

		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		// Input
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
			vDir += vLook;
			m_pSlasher_Huntress->m_bMove[0] = true;
			m_eCurRun = ERun::RUN_FT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W)) {
			m_pSlasher_Huntress->m_bMove[0] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S)) {
			vDir -= vLook;
			m_pSlasher_Huntress->m_bMove[1] = true;
			m_eCurRun = ERun::RUN_BK;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S)) {
			m_pSlasher_Huntress->m_bMove[1] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D)) {
			vDir += vRight;
			m_pSlasher_Huntress->m_bMove[2] = true;
			m_eCurRun = ERun::RUN_RT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D)) {
			m_pSlasher_Huntress->m_bMove[2] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A)) {
			vDir -= vRight;
			m_pSlasher_Huntress->m_bMove[3] = true;
			m_eCurRun = ERun::RUN_LT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A)) {
			m_pSlasher_Huntress->m_bMove[3] = false;
		}

		if ((true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[1]) || (true == m_pSlasher_Huntress->m_bMove[2] && true == m_pSlasher_Huntress->m_bMove[3]))
			return CFM_Slasher::EState::IDLE_HUNT;

		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
			vDir += vLook;
			vDir += vRight;
			m_eCurRun = ERun::RUN_FTRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
			vDir += vLook;
			vDir -= vRight;
			m_eCurRun = ERun::RUN_FTLT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
			vDir -= vLook;
			vDir += vRight;
			m_eCurRun = ERun::RUN_BKRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
			vDir -= vLook;
			vDir -= vRight;
			m_eCurRun = ERun::RUN_BKLT;
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

			switch (m_eCurRun) {
			case ERun::RUN_FT:
				return CFM_Slasher::EState::RUN_SLASH_FT;
			case ERun::RUN_BK:
				return CFM_Slasher::EState::RUN_SLASH_BK;
			case ERun::RUN_RT:
				return CFM_Slasher::EState::RUN_SLASH_RT;
			case ERun::RUN_LT:
				return CFM_Slasher::EState::RUN_SLASH_LT;
			case ERun::RUN_FTRT:
				return CFM_Slasher::EState::RUN_SLASH_FTRT;
			case ERun::RUN_FTLT:
				return CFM_Slasher::EState::RUN_SLASH_FTLT;
			case ERun::RUN_BKRT:
				return CFM_Slasher::EState::RUN_SLASH_BKRT;
			case ERun::RUN_BKLT:
				return CFM_Slasher::EState::RUN_SLASH_BKLT;
			}
		}
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::R)) {
			vDir = XMVector3Normalize(vDir);
			XMStoreFloat3(&m_vDir, vDir);
			m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Set_T_MoveDir");

			return CFM_Slasher::EState::THROW_SWING;
		}
		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (false == m_pSlasher_Huntress->m_bCollider_Move) {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
		}
		else {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
			}
		}
		if (false == m_pSlasher_Huntress->m_bMove[0] && false == m_pSlasher_Huntress->m_bMove[1] && false == m_pSlasher_Huntress->m_bMove[2] && false == m_pSlasher_Huntress->m_bMove[3])
			return CFM_Slasher::EState::IDLE_HUNT;
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);

		if (m_ePreRun == m_eCurRun)
			return CFM_Slasher::EState::END;
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_FTRT;
			return CFM_Slasher::EState::RUN_HUNT_FTRT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_FTLT;
			return CFM_Slasher::EState::RUN_HUNT_FTLT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_BKRT;
			return CFM_Slasher::EState::RUN_HUNT_BKRT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_BKLT;
			return CFM_Slasher::EState::RUN_HUNT_BKLT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true) {
			m_ePreRun = ERun::RUN_FT;
			return CFM_Slasher::EState::RUN_HUNT_FT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true) {
			m_ePreRun = ERun::RUN_BK;
			return CFM_Slasher::EState::RUN_HUNT_BK;
		}
		if (m_pSlasher_Huntress->m_bMove[2] == true) {
			m_ePreRun = ERun::RUN_RT;
			return CFM_Slasher::EState::RUN_HUNT_RT;
		}
		if (m_pSlasher_Huntress->m_bMove[3] == true) {
			m_ePreRun = ERun::RUN_LT;
			return CFM_Slasher::EState::RUN_HUNT_LT;
		}
	}
	else {
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(6))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(19))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });

		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (CFM_Slasher::EState::RUN_HUNT_FTRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_HUNT_FTLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_HUNT_BKRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_HUNT_BKLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_HUNT_FT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_HUNT_BK == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_HUNT_RT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_HUNT_LT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Run_Hunt::Get_Dir(void) {
	return m_vDir;
}
