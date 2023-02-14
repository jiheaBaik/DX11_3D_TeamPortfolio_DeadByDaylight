#include "pch.h"
#include "FS_Camper_Spider_Struggle.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Hook.h"
#include "UI_Play_Icon_SkillCheck.h"

CFS_Camper_Spider_Struggle::CFS_Camper_Spider_Struggle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Spider_Struggle::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Spider_Struggle::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);

	if (m_pCamper_GamePlay->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		_float fTime = m_pCamper_GamePlay->m_fSpiderTime;
		dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_HitCount(3, fTime, CUI_Play_Icon_SkillCheck::ECheck::START, 200);
	}

	return S_OK;
}

const _ubyte  CFS_Camper_Spider_Struggle::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {

		m_dSoundTime += dTimeDelta;
		if (m_dSoundTime >= 1.5) {
			m_dSoundTime = 0.;
			
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 0.7f, FMOD_2D);
				break;
			}
		}

		
		m_pCamper_GamePlay->m_fSpiderTime -= (_float)dTimeDelta;
		m_pCamper_GamePlay->Update_Network("Set_SpiderTime");

		CUI_Play_Icon_SkillCheck* pSkillCheck = dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"));
		if (!m_bFailed && CUI_Play_Icon_SkillCheck::EState::FAIL == pSkillCheck->Get_State())//ui저항에 실패
		{
			m_pCamper_GamePlay->m_fSpiderTime -= (_float)dTimeDelta;//한번 더깎음
			m_pCamper_GamePlay->Update_Network("Set_SpiderTime");
			pSkillCheck->Set_HitCount(rand() % 3 + 1, m_pCamper_GamePlay->m_fSpiderTime, CUI_Play_Icon_SkillCheck::ECheck::FAILED, 150);
			m_bFailed = true;
		}

		if (0 > m_pCamper_GamePlay->m_fSpiderTime) {
			m_pHook->Set_State(CInter_Hook::ESTATE::SPREAD);
			return CFM_Camper::EState::SPIDER_IN;
		}
		/*if (0 > m_pCamper_GamePlay->m_fSpiderTime) {
			m_pHook->Set_State(CInter_Hook::ESTATE::KILL);
			return CFM_Camper::EState::Spider_KillIN;
		}*/
	}
	else
	{
		m_dSoundTime += dTimeDelta;
		if (m_dSoundTime >= 2.) {
			m_dSoundTime = 0.;
			
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 3.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 3.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 3.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 3.f });
				break;
			}
		}

	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
