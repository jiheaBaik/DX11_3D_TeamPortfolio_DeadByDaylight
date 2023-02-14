#include "pch.h"
#include "FS_Camper_HookIdle2.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"

CFS_Camper_HookIdle2::CFS_Camper_HookIdle2(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HookIdle2::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookIdle2::Init(_char* pModelName) {
	/*CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pHookObject = dynamic_cast<CInter_Hook*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Hook"), m_pCamper_GamePlay->m_iCollisionId));
	m_pHookObject->Set_State(CInter_Hook::ESTATE::IDLE2);

	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);*/
	return S_OK;
}

const _ubyte  CFS_Camper_HookIdle2::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
