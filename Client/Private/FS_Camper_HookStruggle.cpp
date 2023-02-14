#include "pch.h"
#include "FS_Camper_HookStruggle.h"
#include "Inter_Hook.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Score.h"

CFS_Camper_HookStruggle::CFS_Camper_HookStruggle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HookStruggle::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookStruggle::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);

	if (m_pCamper_GamePlay->m_bControl) {
		
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_effort_hard"), 21, -1, 0.3f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_effort_hard"), 11, -1, 0.3f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_effort_hard"), 25, -1, 0.3f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_effort_hard"), 13, -1, 0.3f, FMOD_2D);
			break;
		}
	}
		
	else {
		
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_effort_hard"), 21, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_effort_hard"), 11, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_effort_hard"), 25, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_effort_hard"), 13, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 0.3f });
			break;
		}
	}
		

	/*if (m_pCamper_GamePlay->m_bControl) {
		pHookObject = static_cast<CInter_Hook*>(m_pCamper_GamePlay->Get_ColObject());
		m_pCamper_GamePlay->Get_FM_Camper()->Update_Network("Set_CollisionId");
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->Set_ColObject(dynamic_cast<CInter_Hook*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Hook"), m_pCamper_GamePlay->m_iCollisionId)));
		pHookObject = static_cast<CInter_Hook*>(m_pCamper_GamePlay->Get_ColObject());
	}*/
	return S_OK;
}

const _ubyte  CFS_Camper_HookStruggle::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {

		_float fStroggleGuage = 0.f;
		fStroggleGuage = (_float)m_pCamper_GamePlay->m_pModel->Get_Anim_TimeAcc() / (_float)m_pCamper_GamePlay->m_pModel->Get_Anim_Duration() * 100.f;
		
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			//Å»Ãâ½Ãµµui RenderX
			m_pHook->Set_State(CInter_Hook::ESTATE::IDLE);
			return CFM_Camper::EState::HOOKIDLE;
		}

		if (m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			//ui RenderX
			_uint iRand = rand() % 40;//4%
			iRand = 0;
			if (iRand == 0)	{
				m_pHook->Set_State(CInter_Hook::ESTATE::ESCAPE);
				m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::HOOKFREE);
				m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
				//hwang - score
				CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
				if (pScore != nullptr) {
					pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::SURVIVAL_HOOK_ESCAPE));
					pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::SURVIVAL), 1500);
				}
			}
			else{

				m_pCamper_GamePlay->m_fSpiderTime = -1.f;
				m_pCamper_GamePlay->Update_Network("Set_SpiderTime");
				m_pHook->Set_State(CInter_Hook::ESTATE::SPREAD);
				return CFM_Camper::EState::SPIDER_IN;
				m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
			}
		}
	}
	else{
		if (m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_HookedFree");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_HookedFree");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_HookedFree");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_HookedFree");
				break;
			}
		}
	}
	
	return CFM_Camper::EState::END;
}
