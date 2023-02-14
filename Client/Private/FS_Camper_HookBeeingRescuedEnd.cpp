#include "pch.h"
#include "FS_Camper_HookBeeingRescuedEnd.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"


CFS_Camper_HookBeeingRescuedEnd::CFS_Camper_HookBeeingRescuedEnd(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_HookBeeingRescuedEnd::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookBeeingRescuedEnd::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	
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
	if (true == m_pCamper_GamePlay->m_bControl) {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Hook"), m_pCamper_GamePlay->m_iTarObjID);
		m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Hook"), m_pCamper_GamePlay->m_iTarObjID);
		m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	}


	return S_OK;
}

const _ubyte  CFS_Camper_HookBeeingRescuedEnd::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	
	if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(15)) 
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_blood_hook_out"), 3, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });

	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
			return CFM_Camper::EState::HIT_IDLE;
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_pHook->Set_State(CInter_Hook::ESTATE::IDLE);
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
