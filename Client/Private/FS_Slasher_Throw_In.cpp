#include "pch.h"
#include "FS_Slasher_Throw_In.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Panel_Second.h"

CFS_Slasher_Throw_In::CFS_Slasher_Throw_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Throw_In::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Throw_In::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Throw_In::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	if (true == m_pSlasher_Huntress->m_bControl) {
		// set ui : cancle
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CUI_Play_Icon_Hud* pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"));
		pHud->Set_AllHuds(false/*, -1*/);
		pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::CANCLE), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_RenderAll(false, false);

		m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_charge"), 8, -1, 0.3f, FMOD_2D);
	}
	else {
		m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_charge"), 8, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
	}

	m_bThrow = false;
	return S_OK;
}

const _ubyte  CFS_Slasher_Throw_In::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
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

		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::R) && m_pSlasher_Huntress->m_iWeaponCount > 0) {
			m_bThrow = true;
		}

		vDir = XMVector3Normalize(vDir);
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (XMVectorGetX(vDir) != 0.f || XMVectorGetY(vDir) != 0.f || XMVectorGetZ(vDir) != 0.f || XMVectorGetW(vDir) != 0.f) {
			if (false == m_pSlasher_Huntress->m_bCollider_Move)
				m_pSlasher_Huntress->m_pTransform->Translate(vDir, 3.f, dTimeDelta);
			else {
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vDir));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(vDir, 3.f, dTimeDelta);
			}
			m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
			m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
		}				
		XMStoreFloat3(&m_vDir, vDir);
		if (m_vDir.x != m_vPreDir.x || m_vDir.y != m_vPreDir.y || m_vDir.z != m_vPreDir.z) {
			m_pSlasher_Huntress->m_pFM->Update_Network("Set_T_MoveDir");
		}
		XMStoreFloat3(&m_vPreDir, vDir);

		// Anim
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			//m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::HUNT;
			if (true == m_bThrow) {
				m_pSlasher_Huntress->m_pFM->Update_Network("Set_T_NextDir");
				return CFM_Slasher::EState::THROW_SWING;
			}
			else 
				return CFM_Slasher::EState::IDLE_HUNT;
		}
	}
	else {
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (m_vDir.x != 0.f || m_vDir.y != 0.f || m_vDir.z != 0.f) {
			if (false == m_pSlasher_Huntress->m_bCollider_Move)
				m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 3.f, dTimeDelta);
			else {
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), XMLoadFloat3(&m_vDir)));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(XMLoadFloat3(&m_vDir), 3.f, dTimeDelta);
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

const _float3& CFS_Slasher_Throw_In::Get_Dir(void) {
	return m_vDir;
}

void CFS_Slasher_Throw_In::Set_Dir(const _float3& vDir) {
	m_vDir = vDir;
}