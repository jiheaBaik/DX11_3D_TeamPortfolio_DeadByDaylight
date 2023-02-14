#include "pch.h"
#include "FS_Camper_Hit.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_Hit::CFS_Camper_Hit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Hit::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Hit::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pCamper_GamePlay->m_bControl) {
		CObject* pCamBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_CamBlood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_CamBlood"));
		CObject* pBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood"));
		CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
		pTransform->Set_Row(CTransform::ERow::POS, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_hurt_hard"), 12, -1, 0.5f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_hurt_hard"), 15, -1, 0.5f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_hurt_hard"), 14, -1, 0.5f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_hurt_hard"), 9, -1, 0.5f, FMOD_2D);
			break;
		}
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Icon_Hud")))->Set_AllHuds(false, -1);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Gauge_Inter")))->Set_RenderAll(false, false);
	}
	else {
		CObject* pBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood"));
		CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
		pTransform->Set_Row(CTransform::ERow::POS, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 1.f, 0.f, 0.f));
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_hurt_hard"), 12, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_hurt_hard"), 15, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_hurt_hard"), 14, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_hurt_hard"), 9, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			break;
		}
	}
	
	return S_OK;
}

const _ubyte  CFS_Camper_Hit::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
					m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::HIT_RUN);
					m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
				}
			}

			if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
				m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::HIT_WALK);
				m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
			}
		
			return CFM_Camper::EState::HIT_IDLE;
		}

	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
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
