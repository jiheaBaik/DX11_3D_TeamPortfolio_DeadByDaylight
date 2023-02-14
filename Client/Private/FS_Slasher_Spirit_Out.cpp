#include "pch.h"
#include "FS_Slasher_Spirit_Out.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Icon_Passive.h"

CFS_Slasher_Spirit_Out::CFS_Slasher_Spirit_Out(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Spirit_Out::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Spirit_Out::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Spirit_Out::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(25);
	
	//if (true == m_pSlasher_Huntress->m_bControl) {
	//	m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_throw"), 8, -1, 1.f, FMOD_2D);
	//	m_pSlasher_Huntress->m_pAudio->Play_Sound(TEXT("sfx_hatchet_throw"), -1, 1.f, FMOD_2D);

	//	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//	//dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::CANCLE), true);
	//	dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::CANCLE));
	//}
	//else {
	//	m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_throw"), 8, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
	//	m_pSlasher_Huntress->m_pAudio->Play_Sound(TEXT("sfx_hatchet_throw"), -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
	//}

	return S_OK;
}

const _ubyte  CFS_Slasher_Spirit_Out::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CCompute* pCompute = CCompute::Get_Instance();
		if (pCompute->Get_Saturation() <= 1.2f) {
			pCompute->Add_Saturation(dTimeDelta * 1.2f);
		}
		else
			pCompute->Set_Saturation(1.2f);

		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(10))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(15))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(22))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(25))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);

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

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");

			vDir = XMVector3Normalize(vDir);
			XMStoreFloat3(&m_vDir, vDir);
			m_pSlasher_Huntress->m_pFM->Update_Network("Set_MoveDir");
			return CFM_Slasher::EState::ATTACK_SLASH_IN;
		}

		vDir = XMVector3Normalize(vDir);

		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (XMVectorGetX(vDir) != 0.f || XMVectorGetY(vDir) != 0.f || XMVectorGetZ(vDir) != 0.f || XMVectorGetW(vDir) != 0.f) {
			if (false == m_pSlasher_Huntress->m_bCollider_Move)
				m_pSlasher_Huntress->m_pTransform->Translate(vDir, 4.4f, dTimeDelta);
			else {
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vDir));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(vDir, 4.4f, dTimeDelta);
			}
			m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
			m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
		}

		XMStoreFloat3(&m_vDir, vDir);
		if (m_vDir.x != m_vPreDir.x || m_vDir.y != m_vPreDir.y || m_vDir.z != m_vPreDir.z) {
			m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Set_S_MoveDir");
		}
		XMStoreFloat3(&m_vPreDir, vDir);

		// Anim
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
		//if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(30)) {
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");
			// set ui
			
			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(10))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(15))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(22))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(25))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });

		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (m_vDir.x != 0.f || m_vDir.y != 0.f || m_vDir.z != 0.f) {
			if (false == m_pSlasher_Huntress->m_bCollider_Move)
				m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 4.4f, dTimeDelta);
			else {
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), XMLoadFloat3(&m_vDir)));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 4.4f, dTimeDelta);
			}
			m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
			m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
		}
		// Anim
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
		/*m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);*/
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Spirit_Out::Get_Dir(void) {
	return m_vDir;
}

void CFS_Slasher_Spirit_Out::Set_Dir(const _float3& vDir) {
	m_vDir = vDir;
}
