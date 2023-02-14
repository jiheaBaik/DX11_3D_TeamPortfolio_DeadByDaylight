#include "pch.h"
#include "FS_Slasher_Attack_Slash_Swing.h"
#include "Slasher_Huntress.h"
#include "Weapon_Slash.h"
#include "Weapon_Spirit.h"

CFS_Slasher_Attack_Slash_Swing::CFS_Slasher_Attack_Slash_Swing(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Attack_Slash_Swing::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Attack_Slash_Swing::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Attack_Slash_Swing::Init(_uint iAnimIndex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	if (true == m_pSlasher_Huntress->m_bSlasher) {
		if (true == m_pSlasher_Huntress->m_bControl) {
			CWeapon_Slash* pWeapon_Slash = dynamic_cast<CWeapon_Slash*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Slash")));
			pWeapon_Slash->Set_Attack(true);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_attack"), 11, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 5, -1, 0.3f, FMOD_2D);
		}
		else {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_attack"), 11, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
		}
	}
	else {
		if (true == m_pSlasher_Huntress->m_bControl) {
			CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
			pWeapon_Spirit->Set_Attack(true);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Katana_Attack_Bounce"), 6, -1, 0.3f, FMOD_2D);
		}
		else {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Katana_Attack_Bounce"), 6, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
		}
	}

	return S_OK;
}

const _ubyte  CFS_Slasher_Attack_Slash_Swing::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		if (false == m_pSlasher_Huntress->m_bSlasher && true == m_pSlasher_Huntress->m_bSaturation) {
			CCompute* pCompute = CCompute::Get_Instance();
			if (pCompute->Get_Saturation() <= 1.2f) {
				pCompute->Add_Saturation(dTimeDelta * 1.2f);
			}
			else {
				m_pSlasher_Huntress->m_bSaturation = false;
				pCompute->Set_Saturation(1.2f);
			}
		}

		_vector vLook = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 0.f));
		_vector vRight = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 0.f));

		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W) & 0x80) {
			vDir += vLook;
			m_pSlasher_Huntress->m_bMove[0] = true;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W)) {
			m_pSlasher_Huntress->m_bMove[0] = false;
		}
		/*if (pGame_Instance->Get_Input_KeyboardKey(DIK_S)) {
			vDir -= vLook;
			m_pSlasher_Huntress->m_bMove[1] = true;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S)) {
			m_pSlasher_Huntress->m_bMove[1] = false;
		}*/
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D) & 0x80) {
			vDir += vRight;
			m_pSlasher_Huntress->m_bMove[2] = true;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D)) {
			m_pSlasher_Huntress->m_bMove[2] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A) & 0x80) {
			vDir -= vRight;
			m_pSlasher_Huntress->m_bMove[3] = true;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A)) {
			m_pSlasher_Huntress->m_bMove[3] = false;
		}

		vDir = XMVector3Normalize(vDir);
		if (false == m_pSlasher_Huntress->m_bCollider_Move)
			m_pSlasher_Huntress->m_pTransform->Translate(vDir, 2.5f, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vDir));
			if (fDotTarget < 0)
				m_pSlasher_Huntress->m_pTransform->Translate(vDir, 2.5f, dTimeDelta);
		}

		XMStoreFloat3(&m_vDir, vDir);
		if (m_vDir.x != m_vPreDir.x || m_vDir.y != m_vPreDir.y || m_vDir.z != m_vPreDir.z) {
			m_pSlasher_Huntress->m_pFM->Update_Network("Set_MoveDir");
		}
		XMStoreFloat3(&m_vPreDir, vDir);

		if (true == m_pSlasher_Huntress->m_bSlasher) {
			CWeapon_Slash* pWeapon_Slash = dynamic_cast<CWeapon_Slash*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Slash")));

			// Anim
			m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta * 3.f);
			if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta * 3.f)) {
				if (false == pWeapon_Slash->Get_Attack()) {
					m_bAttack = pWeapon_Slash->Get_Attack();
					m_pSlasher_Huntress->m_pFM->Update_Network("Set_NextDir");
					return CFM_Slasher::EState::ATTACK_SLASH_WIPE;
				}
				else {
					m_bAttack = pWeapon_Slash->Get_Attack();
					m_pSlasher_Huntress->m_pFM->Update_Network("Set_NextDir");
					return CFM_Slasher::EState::ATTACK_SLASH_MISS;
				}
			}
		}
		else {
			CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));

			// Anim
			m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta * 1.5f);
			if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta * 1.5f)) {
				if (false == pWeapon_Spirit->Get_Attack()) {
					m_bAttack = pWeapon_Spirit->Get_Attack();
					m_pSlasher_Huntress->m_pFM->Update_Network("Set_NextDir");
					return CFM_Slasher::EState::ATTACK_SLASH_WIPE;
				}
				else {
					m_bAttack = pWeapon_Spirit->Get_Attack();
					m_pSlasher_Huntress->m_pFM->Update_Network("Set_NextDir");
					return CFM_Slasher::EState::ATTACK_SLASH_MISS;
				}
			}
		}
	}
	else {
		if (false == m_pSlasher_Huntress->m_bCollider_Move)
			m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 2.5f, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), XMLoadFloat3(&m_vDir)));
			if (fDotTarget < 0)
				m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 2.5f, dTimeDelta);
		}

		// Anim
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta * 3.f);
			m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta * 3.f);
		}
		else {
			m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta * 1.5f);
			m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta * 1.5f);
		}
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Attack_Slash_Swing::Get_Dir(void) {
	return m_vDir;
}

void CFS_Slasher_Attack_Slash_Swing::Set_Dir(const _float3& vDir) {
	m_vDir = vDir;
}

const _bool& CFS_Slasher_Attack_Slash_Swing::Get_Attack(void) {
	return m_bAttack;
}
