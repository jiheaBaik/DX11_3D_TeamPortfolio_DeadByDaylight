#include "pch.h"
#include "FS_Camper_Spider_IN.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Hook.h"

CFS_Camper_Spider_IN::CFS_Camper_Spider_IN(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Spider_IN::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Spider_IN::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_theentity_background_whale"), 14, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 3.f });

	//Å»Ãâ½Ãµµui Rnderx
	return S_OK;
}

const _ubyte  CFS_Camper_Spider_IN::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {

		if (10. <= m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc() && 11. > m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc())
		{
			
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 1.f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 1.f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 1.f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 1.f, FMOD_2D);
				break;
			}
		}
			

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			m_pHook->Set_State(CInter_Hook::ESTATE::SHAKE);
			return CFM_Camper::EState::SPIDER_LOOP;
		}
	}
	else
	{
		if (10. <= m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc() && 11. > m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc())
		{
			
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			}
		}
			

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Spider_Reaction_Loop");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Spider_Reaction_Loop");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Spider_Reaction_Loop");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Spider_Reaction_Loop");
				break;
			}
		}
	}

	return CFM_Camper::EState::END;
}
