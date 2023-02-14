#include "pch.h"
#include "FS_Slasher_Stun.h"
#include "Slasher_Huntress.h"
#include "Inter_PlankAnim.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Stun::CFS_Slasher_Stun(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Stun::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Stun::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Stun::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(44);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(44);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
		m_pSlasher_Huntress->Update_Network("Set_TarObjID");
		m_pPlank = dynamic_cast<CInter_PlankAnim*>(m_pSlasher_Huntress->m_pTarObject);
		m_pSlasher_Huntress->m_bStun = true;
		m_iPlankDir = m_pSlasher_Huntress->m_iColObjDir;
		// set ui
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, -1);

		m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pallet_killer_hit"), 3, -1, 0.3f, FMOD_2D);
		if (true == m_pSlasher_Huntress->m_bSlasher)
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_hurt"), 11, -1, 0.3f, FMOD_2D);
		else
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_grunt_hurt"), 4, -1, 0.3f, FMOD_2D);
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_PlankAnim*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_PlankAnim"), m_pSlasher_Huntress->m_iTarObjID));
		m_pPlank = dynamic_cast<CInter_PlankAnim*>(m_pSlasher_Huntress->m_pTarObject);
		m_pSlasher_Huntress->m_bStun = true;
		m_iPlankDir = m_pSlasher_Huntress->m_iColObjDir;

		m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pallet_killer_hit"), 3, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		if (true == m_pSlasher_Huntress->m_bSlasher)
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_hurt"), 11, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		else
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_grunt_hurt"), 4, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
	}

	m_pPlankTransform = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform")));
	m_pPlankTransform_Slasher = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform_Slasher")));

	enum Dir { FRONT, BACK, END };
	switch (m_iPlankDir) {
	case FRONT:
		m_pPlankTransform2 = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform_F")));
		break;
	case BACK:
		m_pPlankTransform2 = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform_B")));
		break;
	}
	return S_OK;
}

const _ubyte  CFS_Slasher_Stun::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		if (true == m_pSlasher_Huntress->m_bStun)
			m_pSlasher_Huntress->m_pTransform->Translate(XMVector3TransformCoord(m_pPlankTransform2->Get_Row(CTransform::ERow::POS), m_pPlankTransform->Get_Matrix()) - XMVector3TransformCoord(m_pPlankTransform_Slasher->Get_Row(CTransform::ERow::POS), m_pPlankTransform->Get_Matrix()), 3.f, dTimeDelta);
		//m_pSlasher_Huntress->m_pTransform->Translate(m_pPlankTransform2->Get_Row(CTransform::ERow::POS) - m_pPlankTransform_Slasher->Get_Row(CTransform::ERow::POS), 3.f, dTimeDelta);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Set_Type");
			// set ui
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			if (0 < m_pSlasher_Huntress->m_iWeaponCount)
				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::CENTER);
			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		if (true == m_pSlasher_Huntress->m_bStun)
			m_pSlasher_Huntress->m_pTransform->Translate(XMVector3TransformCoord(m_pPlankTransform2->Get_Row(CTransform::ERow::POS), m_pPlankTransform->Get_Matrix()) - XMVector3TransformCoord(m_pPlankTransform_Slasher->Get_Row(CTransform::ERow::POS), m_pPlankTransform->Get_Matrix()), 3.f, dTimeDelta);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
