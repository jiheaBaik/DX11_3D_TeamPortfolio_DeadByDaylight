#include "pch.h"
#include "FS_Camper_BeingHealed.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"

CFS_Camper_BeingHealed::CFS_Camper_BeingHealed(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_BeingHealed::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_BeingHealed::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	
	return S_OK;
}

const _ubyte  CFS_Camper_BeingHealed::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	
	
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
