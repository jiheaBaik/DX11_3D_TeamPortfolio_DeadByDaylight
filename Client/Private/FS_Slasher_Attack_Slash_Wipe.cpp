#include "pch.h"
#include "FS_Slasher_Attack_Slash_Wipe.h"
#include "Slasher_Huntress.h"
#include "Weapon_Spirit.h"
#include "UI_Play_Score.h"

CFS_Slasher_Attack_Slash_Wipe::CFS_Slasher_Attack_Slash_Wipe(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Attack_Slash_Wipe::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Attack_Slash_Wipe::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Attack_Slash_Wipe::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	if (true == m_pSlasher_Huntress->m_bSlasher) {
		if (true == m_pSlasher_Huntress->m_bControl) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_axe_clean_blood"), 2, -1, 0.3f, FMOD_2D);
			//hwang - score
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::BRUTALLITY_HIT));
				pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::BRUTALITY), 300);
			}
		}
		else {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_axe_clean_blood"), 2, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		}
	}
	else {
		if (true == m_pSlasher_Huntress->m_bControl) {
			//m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_axe_clean_blood"), 2, -1, 1.f, FMOD_2D);
			//hwang - score
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::BRUTALLITY_HIT));
				pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::BRUTALITY), 300);
			}
		}
		else {
			//m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_axe_clean_blood"), 2, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		}
	}

	return S_OK;
}

const _ubyte  CFS_Slasher_Attack_Slash_Wipe::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (true == m_pSlasher_Huntress->m_bControl) {
		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(56))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);

			if (true == m_pSlasher_Huntress->m_bSaturation) {
				CCompute* pCompute = CCompute::Get_Instance();
				if (pCompute->Get_Saturation() <= 1.2f) {
					pCompute->Add_Saturation(dTimeDelta * 1.2f);
				}
				else {
					m_pSlasher_Huntress->m_bSaturation = false;
					pCompute->Set_Saturation(1.2f);
				}
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

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta))
			return CFM_Slasher::EState::IDLE_SLASH;

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
			CTransform* pWeapon_Transform = dynamic_cast<CTransform*>(pWeapon_Spirit->Get_Component(TEXT("Transform")));
			CTransform* pWeapon_Transform_FPV = dynamic_cast<CTransform*>(pWeapon_Spirit->Get_Component(TEXT("Transform_FPV")));

			CModel* pModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model")));
			CModel* pModel_FPV = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model_FPV")));
			_matrix BoneMatrix = pModel->Get_BoneMatrix("joint_WeaponRT_01");
			_matrix BoneMatrix_FPV = pModel_FPV->Get_BoneMatrix("joint_WeaponRT_01");

			_matrix SocketMatrix;
			_matrix SocketMatrix_FPV;

			SocketMatrix = BoneMatrix * m_pSlasher_Huntress->m_pTransform->Get_Matrix();
			SocketMatrix_FPV = BoneMatrix_FPV * m_pSlasher_Huntress->m_pTransform->Get_Matrix();

			pWeapon_Transform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
			pWeapon_Transform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
			pWeapon_Transform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
			pWeapon_Transform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);

			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix_FPV.r[0]);
			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::UP, SocketMatrix_FPV.r[1]);
			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::LOOK, -SocketMatrix_FPV.r[2]);
			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::POS, SocketMatrix_FPV.r[3]);

			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(50)) {
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_2D);
			}

			m_pSlasher_Huntress->m_pModel3->Update_Anim(dTimeDelta, true);
			m_pSlasher_Huntress->m_pModel_FPV->Change_Bone(m_pSlasher_Huntress->m_pModel3, m_pSlasher_Huntress->m_vecChange_Bone2);
		}
	}
	else {
		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(56))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}

		if (false == m_pSlasher_Huntress->m_bCollider_Move)
			m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 1.4f, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), XMLoadFloat3(&m_vDir)));
			if (fDotTarget < 0)
				m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 1.4f, dTimeDelta);
		}
				
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
			CTransform* pWeapon_Transform = dynamic_cast<CTransform*>(pWeapon_Spirit->Get_Component(TEXT("Transform")));
			CTransform* pWeapon_Transform_FPV = dynamic_cast<CTransform*>(pWeapon_Spirit->Get_Component(TEXT("Transform_FPV")));

			CModel* pModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model")));
			CModel* pModel_FPV = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model_FPV")));
			_matrix BoneMatrix = pModel->Get_BoneMatrix("joint_WeaponRT_01");
			_matrix BoneMatrix_FPV = pModel_FPV->Get_BoneMatrix("joint_WeaponRT_01");

			_matrix SocketMatrix;
			_matrix SocketMatrix_FPV;

			SocketMatrix = BoneMatrix * m_pSlasher_Huntress->m_pTransform->Get_Matrix();
			SocketMatrix_FPV = BoneMatrix_FPV * m_pSlasher_Huntress->m_pTransform->Get_Matrix();

			pWeapon_Transform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
			pWeapon_Transform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
			pWeapon_Transform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
			pWeapon_Transform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);

			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix_FPV.r[0]);
			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::UP, SocketMatrix_FPV.r[1]);
			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::LOOK, -SocketMatrix_FPV.r[2]);
			pWeapon_Transform_FPV->Set_Row(CTransform::ERow::POS, SocketMatrix_FPV.r[3]);

			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(50)) {
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}

			m_pSlasher_Huntress->m_pModel3->Update_Anim(dTimeDelta, true);
			m_pSlasher_Huntress->m_pModel_FPV->Change_Bone(m_pSlasher_Huntress->m_pModel3, m_pSlasher_Huntress->m_vecChange_Bone2);
		}
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Attack_Slash_Wipe::Get_Dir(void) {
	return m_vDir;
}

void CFS_Slasher_Attack_Slash_Wipe::Set_Dir(const _float3& vDir) {
	m_vDir = vDir;
}
