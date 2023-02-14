#include "pch.h"
#include "FS_Slasher_Stun_Flash_Out.h"
#include "Slasher_Huntress.h"
#include "Inter_PlankAnim.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Stun_Flash_Out::CFS_Slasher_Stun_Flash_Out(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Stun_Flash_Out::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Stun_Flash_Out::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Stun_Flash_Out::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	return S_OK;
}

const _ubyte  CFS_Slasher_Stun_Flash_Out::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {		
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");

			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			if (0 < m_pSlasher_Huntress->m_iWeaponCount)
				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::CENTER);

			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
