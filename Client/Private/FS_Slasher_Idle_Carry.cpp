#include "pch.h"
#include "FS_Slasher_Idle_Carry.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"

CFS_Slasher_Idle_Carry::CFS_Slasher_Idle_Carry(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Idle_Carry::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Idle_Carry::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Idle_Carry::Init(_uint iAnimIndex) {
	//if (19 == iAnimIndex)
	//	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(31);
	//else if (9 == iAnimIndex)
	//	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(31);
	//else
	//	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_dFootTime = 0.;

	m_pSlasher_Huntress->m_pModel2->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);

	return S_OK;
}

const _ubyte  CFS_Slasher_Idle_Carry::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		//Input
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
				return CFM_Slasher::EState::RUN_CARRY_FT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_S))
				return CFM_Slasher::EState::RUN_CARRY_BK;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_D))
				return CFM_Slasher::EState::RUN_CARRY_RT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_A))
				return CFM_Slasher::EState::RUN_CARRY_LT;
		}

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L))
			return CFM_Slasher::EState::ATTACK_CARRY_IN;
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_R)) {
			CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
			pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::DROP);
			pCamper->Get_FM_Camper()->Update_Network("Set_State");
			return CFM_Slasher::EState::DROP;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_SPACE)) {
			if (CSlasher_Huntress::ECollisionState::HOOK == m_pSlasher_Huntress->m_eCollisionState) {
				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
				pCamper->Set_TarObject(m_pSlasher_Huntress->m_pColObject);
				pCamper->Set_TarObjID(m_pSlasher_Huntress->m_pColObject->Get_Id());
				pCamper->Update_Network("Set_TarObjID");
				pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::HOOKIN);
				pCamper->Get_FM_Camper()->Update_Network("Set_State");

				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				CInter_Hook* pHook = dynamic_cast<CInter_Hook*>(m_pSlasher_Huntress->m_pTarObject);
				pHook->Set_State(CInter_Hook::ESTATE::HANGING);
				return CFM_Slasher::EState::HOOK;
			}
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
