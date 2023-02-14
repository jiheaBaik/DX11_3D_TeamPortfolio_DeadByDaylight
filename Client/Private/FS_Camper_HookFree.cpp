#include "pch.h"
#include "FS_Camper_HookFree.h"

#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Hook.h"

CFS_Camper_HookFree::CFS_Camper_HookFree(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_HookFree::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookFree::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);

	//hwang
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
	if (nullptr != listCamper) {
		for (auto& iter : *listCamper) {
			CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
			if (pCamper != m_pCamper_GamePlay)
				pCamper->Set_FindPerk(0.0, false);
		}
	}

	//Å»Ãâ½Ãµµui Rnderx
	return S_OK;
}

const _ubyte  CFS_Camper_HookFree::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {


		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_pCamper_GamePlay->Set_Hp(1);
			
			m_pHook->Set_State(CInter_Hook::ESTATE::IDLE);

			return CFM_Camper::EState::HIT_IDLE;
		}
		
	}
	else
	{
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)){
			m_pCamper_GamePlay->Set_Hp(1);
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_inj_StandIdle");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_inj_StandIdle");
				break;
			}
		}
	}

	return CFM_Camper::EState::END;
}
