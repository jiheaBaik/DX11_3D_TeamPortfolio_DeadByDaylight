#include "pch.h"
#include "FS_Slasher_Attack_Slash_Miss.h"
#include "Slasher_Huntress.h"
#include "Weapon_Spirit.h"

CFS_Slasher_Attack_Slash_Miss::CFS_Slasher_Attack_Slash_Miss(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Attack_Slash_Miss::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Attack_Slash_Miss::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Attack_Slash_Miss::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	return S_OK;
}

const _ubyte  CFS_Slasher_Attack_Slash_Miss::Update(const _double& dTimeDelta) {
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
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S) & 0x80) {
			vDir -= vLook;
			m_pSlasher_Huntress->m_bMove[1] = true;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S)) {
			m_pSlasher_Huntress->m_bMove[1] = false;
		}
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
			m_pSlasher_Huntress->m_pTransform->Translate(vDir, 1.4f, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vDir));
			if (fDotTarget < 0)
				m_pSlasher_Huntress->m_pTransform->Translate(vDir, 1.4f, dTimeDelta);
		}
		XMStoreFloat3(&m_vDir, vDir);
		if (m_vDir.x != m_vPreDir.x || m_vDir.y != m_vPreDir.y || m_vDir.z != m_vPreDir.z) {
			m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Set_MoveDir");
		}
		XMStoreFloat3(&m_vPreDir, vDir);

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(20)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_2D);
			}
		}

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta * 1.2f);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta * 1.2f))
			return CFM_Slasher::EState::IDLE_SLASH;

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			m_pSlasher_Huntress->m_pModel3->Update_Anim(dTimeDelta, true);
			m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel3, m_pSlasher_Huntress->m_vecChange_Bone2);
		}
	}
	else {
		if (false == m_pSlasher_Huntress->m_bCollider_Move)
			m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 1.4f, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), XMLoadFloat3(&m_vDir)));
			if (fDotTarget < 0)
				m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 1.4f, dTimeDelta);
		}

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(20)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
		}

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta * 1.2f);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta * 1.2f);
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Attack_Slash_Miss::Get_Dir(void) {
	return m_vDir;
}

void CFS_Slasher_Attack_Slash_Miss::Set_Dir(const _float3& vDir) {
	m_vDir = vDir;
}