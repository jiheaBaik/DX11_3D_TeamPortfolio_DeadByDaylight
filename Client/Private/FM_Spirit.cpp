#include "pch.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "FM_Spirit.h"
#include "FS_Slasher_Idle_Intro.h"
#include "FS_Slasher_Idle_Slash.h"
#include "FS_Slasher_Idle_Carry.h"
#include "FS_Slasher_Idle_Hunt.h"
#include "FS_Slasher_Run_Slash.h"
#include "FS_Slasher_Run_CARRY.h"
#include "FS_Slasher_Run_HUNT.h"
#include "FS_Slasher_Attack_Carry_In.h"
#include "FS_Slasher_Attack_Carry_Swing.h"
#include "FS_Slasher_Attack_Carry_Out.h"
#include "FS_Slasher_Attack_Slash_In.h"
#include "FS_Slasher_Attack_Slash_Swing.h"
#include "FS_Slasher_Attack_Slash_Miss.h"
#include "FS_Slasher_Attack_Slash_Wipe.h"
#include "FS_Slasher_PickUp_In.h"
#include "FS_Slasher_PickUp.h"
#include "FS_Slasher_Stun_Drop_In.h"
#include "FS_Slasher_Stun_Drop_Out.h"
#include "FS_Slasher_Stun_Flash.h"
#include "FS_Slasher_Stun_Flash_In.h"
#include "FS_Slasher_Stun_Flash_Out.h"
#include "FS_Slasher_Stun.h"
#include "FS_Slasher_Stun_Locker.h"
#include "FS_Slasher_Throw_CoolDown.h"
#include "FS_Slasher_Throw_In.h"
#include "FS_Slasher_Throw_Swing.h"
#include "FS_Slasher_Destroy_Gene.h"
#include "FS_Slasher_Destroy_Hatch.h"
#include "FS_Slasher_Destroy_Plank.h"
#include "FS_Slasher_Destroy_Totem.h"
#include "FS_Slasher_Locker_Grab.h"
#include "FS_Slasher_Locker_Reload.h"
#include "FS_Slasher_Locker_Search.h"
#include "FS_Slasher_Mori.h"
#include "FS_Slasher_Drop.h"
#include "FS_Slasher_Hook.h"
#include "FS_Slasher_Parkour.h"
#include "FS_Slasher_Parkour_Vault.h"
#include "FS_Slasher_Grab_Gene.h" 
#include "FS_Slasher_Grab_Gene_In.h"
#include "FS_Slasher_Grab_Parkour_BK.h"
#include "FS_Slasher_Grab_Parkour_BK_In.h"
#include "FS_Slasher_Grab_Parkour_FT.h"
#include "FS_Slasher_Grab_Parkour_FT_In.h"

CFM_Spirit::CFM_Spirit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pNetwork(pNetwork) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pNetwork);
	m_pSlasher_Huntress = pSlasher_Huntress;
	Init();
}

void CFM_Spirit::Delete(void) {
	for (_uint i = 0; i < EState::END; ++i)
		Safe_Release(m_pFS[i]);
	Safe_Release(m_pNetwork);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CFM_Spirit::Init(void) {
	m_pFS[EState::IDLE_INTRO] = new CFS_Slasher_Idle_Intro(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::IDLE_SLASH] = new CFS_Slasher_Idle_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::IDLE_CARRY] = new CFS_Slasher_Idle_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	// class
	m_pFS[EState::IDLE_SPIRIT] = new CFS_Slasher_Idle_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	m_pFS[EState::RUN_SLASH_FT] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SLASH_BK] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SLASH_RT] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SLASH_LT] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SLASH_FTRT] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SLASH_FTLT] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SLASH_BKRT] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SLASH_BKLT] = new CFS_Slasher_Run_Slash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	m_pFS[EState::RUN_CARRY_FT] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_CARRY_BK] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_CARRY_RT] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_CARRY_LT] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_CARRY_FTRT] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_CARRY_FTLT] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_CARRY_BKRT] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_CARRY_BKLT] = new CFS_Slasher_Run_Carry(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	// class
	m_pFS[EState::RUN_SPIRIT_FT] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SPIRIT_BK] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SPIRIT_RT] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SPIRIT_LT] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SPIRIT_FTRT] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SPIRIT_FTLT] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SPIRIT_BKRT] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::RUN_SPIRIT_BKLT] = new CFS_Slasher_Run_Hunt(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	m_pFS[EState::ATTACK_CARRY_IN] = new CFS_Slasher_Attack_Carry_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::ATTACK_CARRY_SWING] = new CFS_Slasher_Attack_Carry_Swing(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::ATTACK_CARRY_OUT] = new CFS_Slasher_Attack_Carry_Out(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::ATTACK_SLASH_IN] = new CFS_Slasher_Attack_Slash_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::ATTACK_SLASH_SWING] = new CFS_Slasher_Attack_Slash_Swing(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::ATTACK_SLASH_MISS] = new CFS_Slasher_Attack_Slash_Miss(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::ATTACK_SLASH_WIPE] = new CFS_Slasher_Attack_Slash_Wipe(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::PICKUP_IN] = new CFS_Slasher_PickUp_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::PICKUP] = new CFS_Slasher_PickUp(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::HOOK] = new CFS_Slasher_Hook(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::DROP] = new CFS_Slasher_Drop(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::STUN_DROP_IN] = new CFS_Slasher_Stun_Drop_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::STUN_DROP_OUT] = new CFS_Slasher_Stun_Drop_Out(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::STUN_FLASH] = new CFS_Slasher_Stun_Flash(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::STUN_FLASH_IN] = new CFS_Slasher_Stun_Flash_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::STUN_FLASH_OUT] = new CFS_Slasher_Stun_Flash_Out(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::STUN] = new CFS_Slasher_Stun(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::STUN_LOCKER] = new CFS_Slasher_Stun_Locker(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	// class
	m_pFS[EState::SPIRIT_OUT] = new CFS_Slasher_Throw_CoolDown(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::SPIRIT_CHARGE] = new CFS_Slasher_Throw_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::SPIRIT_OUT_BK] = new CFS_Slasher_Throw_Swing(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::SPIRIT_OUT_LT] = new CFS_Slasher_Throw_Swing(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::SPIRIT_OUT_RT] = new CFS_Slasher_Throw_Swing(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	m_pFS[EState::MORI] = new CFS_Slasher_Mori(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::PARKOUR] = new CFS_Slasher_Pakour(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::PARKOUR_VAULT] = new CFS_Slasher_Pakour_Vault(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::DESTROY_GENE] = new CFS_Slasher_Destroy_Gene(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::DESTROY_HATCH] = new CFS_Slasher_Destroy_Hatch(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::DESTROY_PLANK] = new CFS_Slasher_Destroy_Plank(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::DESTROY_TOTEM] = new CFS_Slasher_Destroy_Totem(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::LOCKER_GRAB] = new CFS_Slasher_Locker_Grab(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::LOCKER_SEARCH] = new CFS_Slasher_Locker_Search(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);

	m_pFS[EState::GRAB_GENE] = new CFS_Slasher_Grab_Gene(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::GRAB_GENE_IN] = new CFS_Slasher_Grab_Gene_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::GRAB_PARKOUR_BK] = new CFS_Slasher_Grab_Parkour_BK(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::GRAB_PARKOUR_BK_IN] = new CFS_Slasher_Grab_Parkour_BK_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::GRAB_PARKOUR_FT] = new CFS_Slasher_Grab_Parkour_FT(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	m_pFS[EState::GRAB_PARKOUR_FT_IN] = new CFS_Slasher_Grab_Parkour_FT_In(m_pDevice, m_pContext, m_pNetwork, m_pSlasher_Huntress);
	return S_OK;
}

void CFM_Spirit::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		EState eState = static_cast<EState>(m_pFS[m_eState]->Update(dTimeDelta));

		if (eState != EState::END) {
			Set_State(eState);
		}
	}
	else {
		m_pFS[m_eState]->Update(dTimeDelta);
	}	
}

void CFM_Spirit::Set_State(const EState& eState) {
	switch (m_pSlasher_Huntress->m_eType) {
	case CSlasher_Huntress::EType::SLASH: {
		switch (eState) {
		case EState::IDLE_INTRO: {
			m_eState = eState;
			m_pFS[eState]->Init(25);
			break;
		}
		case EState::IDLE_SLASH: {
			m_eState = eState;
			m_pFS[eState]->Init(25);
			break;
		}
		case EState::RUN_SLASH_FT: {
			m_eState = eState;
			m_pFS[eState]->Init(38);
			break;
		}
		case EState::RUN_SLASH_BK: {
			m_eState = eState;
			m_pFS[eState]->Init(36);
			break;
		}
		case EState::RUN_SLASH_RT: {
			m_eState = eState;
			m_pFS[eState]->Init(40);
			break;
		}
		case EState::RUN_SLASH_LT: {
			m_eState = eState;
			m_pFS[eState]->Init(39);
			break;
		}
		case EState::RUN_SLASH_FTRT: {
			m_eState = eState;
			m_pFS[eState]->Init(38);
			break;
		}
		case EState::RUN_SLASH_FTLT: {
			m_eState = eState;
			m_pFS[eState]->Init(38);
			break;
		}
		case EState::RUN_SLASH_BKRT: {
			m_eState = eState;
			m_pFS[eState]->Init(36);
			break;
		}
		case EState::RUN_SLASH_BKLT: {
			m_eState = eState;
			m_pFS[eState]->Init(36);
			break;
		}
		case EState::ATTACK_SLASH_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(1);
			break;
		}
		case EState::ATTACK_SLASH_SWING: {
			m_eState = eState;
			m_pFS[eState]->Init(3);
			break;
		}
		case EState::ATTACK_SLASH_MISS: {
			m_eState = eState;
			m_pFS[eState]->Init(2);
			break;
		}
		case EState::ATTACK_SLASH_WIPE: {
			m_eState = eState;
			m_pFS[eState]->Init(4);
			break;
		}
		case EState::PICKUP_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(28);
			break;
		}
		case EState::PICKUP: {
			m_eState = eState;
			m_pFS[eState]->Init(27);
			break;
		}
		case EState::STUN: {
			m_eState = eState;
			m_pFS[eState]->Init(44);
			break;
		}
		case EState::STUN_FLASH: {
			m_eState = eState;
			m_pFS[eState]->Init(44);
			break;
		}
		case EState::STUN_LOCKER: {
			m_eState = eState;
			m_pFS[eState]->Init(44);
			break;
		}		
		case EState::DESTROY_GENE: {
			m_eState = eState;
			m_pFS[eState]->Init(11);
			break;
		}
		case EState::DESTROY_HATCH: {
			m_eState = eState;
			m_pFS[eState]->Init(10);
			break;
		}
		case EState::DESTROY_PLANK: {
			m_eState = eState;
			m_pFS[eState]->Init(14);
			break;
		}
		case EState::DESTROY_TOTEM: {
			m_eState = eState;
			m_pFS[eState]->Init(10);
			break;
		}
		case EState::LOCKER_GRAB: {
			m_eState = eState;
			m_pFS[eState]->Init(19);
			break;
		}
		case EState::LOCKER_SEARCH: {
			m_eState = eState;
			m_pFS[eState]->Init(41);
			break;
		}
		case EState::MORI: {
			m_eState = eState;
			m_pFS[eState]->Init(26);
			break;
		}
		case EState::PARKOUR: {
			m_eState = eState;
			m_pFS[eState]->Init(46);
			break;
		}
		case EState::PARKOUR_VAULT: {
			m_eState = eState;
			m_pFS[eState]->Init(46);
			break;
		}
		case EState::GRAB_GENE: {
			m_eState = eState;
			m_pFS[eState]->Init(17);
			break;
		}
		case EState::GRAB_GENE_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(18);
			break;
		}
		case EState::GRAB_PARKOUR_BK: {
			m_eState = eState;
			m_pFS[eState]->Init(20);
			break;
		}
		case EState::GRAB_PARKOUR_BK_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(21);
			break;
		}
		case EState::GRAB_PARKOUR_FT: {
			m_eState = eState;
			m_pFS[eState]->Init(22);
			break;
		}
		case EState::GRAB_PARKOUR_FT_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(23);
			break;
		}
		}
		break;
	}
	case CSlasher_Huntress::EType::CARRY: {
		switch (eState) {
		case EState::IDLE_CARRY: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(9);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_FT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunFT_Slash_FPV"); 61
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_BK: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunBK_Slash_FPV"); 60
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_RT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunRT_Slash_FPV"); 63
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_LT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunLT_Slash_FPV"); 62
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_FTRT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunFT_Slash_FPV"); 61
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_FTLT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunBK_Slash_FPV"); 60
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_BKRT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunRT_Slash_FPV"); 63
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::RUN_CARRY_BKLT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunLT_Slash_FPV"); 62
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Carry_Idle"); 9
			m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
			m_pFS[eState]->Init(9);
			break;
		}
		case EState::ATTACK_CARRY_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(6);
			break;
		}
		case EState::ATTACK_CARRY_SWING: {
			m_eState = eState;
			m_pFS[eState]->Init(8);
			break;
		}
		case EState::ATTACK_CARRY_OUT: {
			m_eState = eState;
			m_pFS[eState]->Init(7);
			break;
		}
		case EState::HOOK: {
			m_eState = eState;
			m_pFS[eState]->Init(16);
			break;
		}
		case EState::DROP: {
			m_eState = eState;
			m_pFS[eState]->Init(14);
			break;
		}
		case EState::STUN_DROP_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(46);
			break;
		}
		case EState::STUN_DROP_OUT: {
			m_eState = eState;
			m_pFS[eState]->Init(47);
			break;
		}
		case EState::STUN_FLASH_IN: {
			m_eState = eState;
			m_pFS[eState]->Init(46);
			break;
		}
		case EState::STUN_FLASH_OUT: {
			m_eState = eState;
			m_pFS[eState]->Init(47);
			break;
		}
		}
		break;
	}
	case CSlasher_Huntress::EType::SPIRIT: {
		switch (eState) {
		case EState::IDLE_SPIRIT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV"); 50
			m_pFS[eState]->Init(50);
			break;
		}
		case EState::RUN_SPIRIT_FT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunFT_Hunt");		30
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(30);
			break;
		}
		case EState::RUN_SPIRIT_BK: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunBK_Hunt");		25
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(25);
			break;
		}
		case EState::RUN_SPIRIT_RT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunRT_Hunt");		38
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(38);
			break;
		}
		case EState::RUN_SPIRIT_LT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunLT_Hunt");		34
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(34);
			break;
		}
		case EState::RUN_SPIRIT_FTRT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunFT_Hunt");		30
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(30);
			break;
		}
		case EState::RUN_SPIRIT_FTLT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunBK_Hunt");		25
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(30);
			break;
		}
		case EState::RUN_SPIRIT_BKRT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunRT_Hunt");		38
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(25);
			break;
		}
		case EState::RUN_SPIRIT_BKLT: {
			m_eState = eState;
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_RunLT_Hunt");		34
			//m_pFS[eState]->Init("BE_DSkeleton_REF.ao|BE_Throw01_Idle_FPV");
			m_pFS[eState]->Init(25);
			break;
		}
		case EState::STUN: {
			m_eState = eState;
			m_pFS[eState]->Init(48);
			break;
		}
		case EState::STUN_FLASH: {
			m_eState = eState;
			m_pFS[eState]->Init(48);
			break;
		}
		case EState::STUN_LOCKER: {
			m_eState = eState;
			m_pFS[eState]->Init(48);
			break;
		}		
		}
		break;
	}
	}
	if (true == m_pSlasher_Huntress->m_bControl) {
		Update_Network("Set_State");
	}
}

void CFM_Spirit::Update_Network(const string& pKey) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	Json::Value root;
	if ("Set_State" == pKey) {
		root["State"] = static_cast<_int>(m_eState);
		m_pNetwork->Set_Send(m_pSlasher_Huntress->m_iId, pKey, root);
	}
	if ("Set_MoveDir" == pKey) {
		_float3 vDir = {};
		if (m_eState == EState::IDLE_SLASH) {
			CFS_Slasher_Idle_Slash* pFS = dynamic_cast<CFS_Slasher_Idle_Slash*>(m_pFS[EState::IDLE_SLASH]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_FT) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_FT]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_BK) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_BK]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_RT) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_RT]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_LT) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_LT]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_FTRT) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_FTRT]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_FTLT) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_FTLT]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_BKRT) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_BKRT]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::RUN_SLASH_BKLT) {
			CFS_Slasher_Run_Slash* pFS = dynamic_cast<CFS_Slasher_Run_Slash*>(m_pFS[EState::RUN_SLASH_BKLT]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::ATTACK_SLASH_IN) {
			CFS_Slasher_Attack_Slash_In* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_In*>(m_pFS[EState::ATTACK_SLASH_IN]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::ATTACK_SLASH_SWING) {
			CFS_Slasher_Attack_Slash_Swing* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Swing*>(m_pFS[EState::ATTACK_SLASH_SWING]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::ATTACK_SLASH_MISS) {
			CFS_Slasher_Attack_Slash_Miss* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Miss*>(m_pFS[EState::ATTACK_SLASH_MISS]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::ATTACK_SLASH_WIPE) {
			CFS_Slasher_Attack_Slash_Wipe* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Wipe*>(m_pFS[EState::ATTACK_SLASH_WIPE]);
			vDir = pFS->Get_Dir();
		}

		root["Dir"]["x"] = vDir.x;
		root["Dir"]["y"] = vDir.y;
		root["Dir"]["z"] = vDir.z;
		m_pNetwork->Set_Send(m_pSlasher_Huntress->m_iId, pKey, root);
	}
	if ("Set_NextDir" == pKey) {
		_float3 vDir = {};
		if (m_eState == EState::ATTACK_SLASH_IN) {
			CFS_Slasher_Attack_Slash_In* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_In*>(m_pFS[EState::ATTACK_SLASH_IN]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::ATTACK_SLASH_SWING) {
			CFS_Slasher_Attack_Slash_Swing* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Swing*>(m_pFS[EState::ATTACK_SLASH_SWING]);
			_bool bAttack = pFS->Get_Attack();
			vDir = pFS->Get_Dir();
			root["Attack"] = bAttack;
		}
		else if (m_eState == EState::ATTACK_SLASH_MISS) {
			CFS_Slasher_Attack_Slash_Miss* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Miss*>(m_pFS[EState::ATTACK_SLASH_MISS]);
			vDir = pFS->Get_Dir();
		}
		else if (m_eState == EState::ATTACK_SLASH_WIPE) {
			CFS_Slasher_Attack_Slash_Wipe* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Wipe*>(m_pFS[EState::ATTACK_SLASH_WIPE]);
			vDir = pFS->Get_Dir();
		}

		root["Dir"]["x"] = vDir.x;
		root["Dir"]["y"] = vDir.y;
		root["Dir"]["z"] = vDir.z;
		m_pNetwork->Set_Send(m_pSlasher_Huntress->m_iId, pKey, root);
	}
	if ("Plank_Broken" == pKey) {
		m_pNetwork->Set_Send(m_pSlasher_Huntress->m_iId, pKey, root);
	}
}

void CFM_Spirit::Update_Network(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<Json::Value>& recv = m_pNetwork->Get_Recv(m_pSlasher_Huntress->m_iId);
	list<list<Json::Value>::iterator> iter;

	for (auto iter1 = recv.begin(); iter1 != recv.end(); ++iter1) {
		Json::Value root = (*iter1);
		string key = root["Key"].asString();
		if ("Set_State" == key) {
			Set_State(static_cast<EState>(root["State"].asInt()));
			iter.emplace_back(iter1);
		}
		else if ("Set_MoveDir" == key) {
			_float3 vDir = {};
			if (m_eState == EState::IDLE_SLASH || m_eState == EState::RUN_SLASH_FT || m_eState == EState::RUN_SLASH_BK || m_eState == EState::RUN_SLASH_RT || m_eState == EState::RUN_SLASH_LT
				|| m_eState == EState::RUN_SLASH_FTRT || m_eState == EState::RUN_SLASH_FTLT || m_eState == EState::RUN_SLASH_BKRT || m_eState == EState::RUN_SLASH_BKLT) {
				CFS_Slasher_Attack_Slash_In* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_In*>(m_pFS[EState::ATTACK_SLASH_IN]);
				vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
				pFS->Set_Dir(vDir);
			}
			else if (m_eState == EState::ATTACK_SLASH_IN) {
				CFS_Slasher_Attack_Slash_In* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_In*>(m_pFS[EState::ATTACK_SLASH_IN]);
				vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
				pFS->Set_Dir(vDir);
			}
			else if (m_eState == EState::ATTACK_SLASH_SWING) {
				CFS_Slasher_Attack_Slash_Swing* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Swing*>(m_pFS[EState::ATTACK_SLASH_SWING]);
				vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
				pFS->Set_Dir(vDir);
			}
			else if (m_eState == EState::ATTACK_SLASH_MISS) {
				CFS_Slasher_Attack_Slash_Miss* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Miss*>(m_pFS[EState::ATTACK_SLASH_MISS]);
				vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
				pFS->Set_Dir(vDir);
			}
			else if (m_eState == EState::ATTACK_SLASH_WIPE) {
				CFS_Slasher_Attack_Slash_Wipe* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Wipe*>(m_pFS[EState::ATTACK_SLASH_WIPE]);
				vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
				pFS->Set_Dir(vDir);
			}
			iter.emplace_back(iter1);
		}
		else if ("Set_NextDir" == key) {
			_float3 vDir = {};
			if (m_eState == EState::ATTACK_SLASH_IN) {
				CFS_Slasher_Attack_Slash_Swing* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Swing*>(m_pFS[EState::ATTACK_SLASH_SWING]);
				vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
				pFS->Set_Dir(vDir);
			}
			else if (m_eState == EState::ATTACK_SLASH_SWING) {
				_bool bAttack = root["Attack"].asBool();
				if (false == bAttack) {
					CFS_Slasher_Attack_Slash_Miss* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Miss*>(m_pFS[EState::ATTACK_SLASH_MISS]);
					vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
					pFS->Set_Dir(vDir);
				}
				else {
					CFS_Slasher_Attack_Slash_Wipe* pFS = dynamic_cast<CFS_Slasher_Attack_Slash_Wipe*>(m_pFS[EState::ATTACK_SLASH_WIPE]);
					vDir = _float3{ root["Dir"]["x"].asFloat(), root["Dir"]["y"].asFloat(),root["Dir"]["z"].asFloat() };
					pFS->Set_Dir(vDir);
				}
			}
			iter.emplace_back(iter1);
		}		
		else if ("Plank_Broken" == key) {
			CFS_Slasher_Destroy_Plank* pFS = dynamic_cast<CFS_Slasher_Destroy_Plank*>(m_pFS[EState::DESTROY_PLANK]);
			pFS->Set_Broken(true);
			iter.emplace_back(iter1);
		}
	}
	for (auto& iter1 : iter)
		recv.erase(iter1);
}
