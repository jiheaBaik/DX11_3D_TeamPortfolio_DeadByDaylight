#include "pch.h"
#include "FS_Camper_Hook_Death.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"

CFS_Camper_Hook_Death::CFS_Camper_Hook_Death(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Hook_Death::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Hook_Death::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pAudio = dynamic_cast<CAudio*>(m_pCamper_GamePlay->Get_Component(TEXT("Audio")));
	m_pAudio->Play_Sound_Rand(TEXT("sfx_Loudnoise_Mix_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
	m_pAudio->Play_Sound_Rand(TEXT("sfx_theentity_background_whale"), 14, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
	m_pAudio->Play_Sound_Rand(TEXT("sfx_theentity_background"), 2, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
	return S_OK;
}

const _ubyte  CFS_Camper_Hook_Death::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {

		//if (pGame_Instance->Get_Input_KeyboardKey(DIK_1))
		//{
		//	//pHookObject->Set_State(CInter_Hook::ESTATE::TRY_ESCAPE);
		//	return CFM_Camper::EState::Hit_Crawl;
		//}
	
	}
	
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);

	return CFM_Camper::EState::END;
}
