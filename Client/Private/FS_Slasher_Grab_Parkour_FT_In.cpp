#include "pch.h"
#include "FS_Slasher_Grab_Parkour_FT_In.h"
#include "Slasher_Huntress.h"

CFS_Slasher_Grab_Parkour_FT_In::CFS_Slasher_Grab_Parkour_FT_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Grab_Parkour_FT_In::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Grab_Parkour_FT_In::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Grab_Parkour_FT_In::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(23);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(23);
	}
	return S_OK;
}

const _ubyte  CFS_Slasher_Grab_Parkour_FT_In::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta))
			return CFM_Slasher::EState::GRAB_PARKOUR_FT;
	}
	else {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}
	
	return CFM_Slasher::EState::END;
}
