#include "pch.h"
#include "FS_Camper_Carry_Idle.h"

#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_Carry_Wiggle::CFS_Camper_Carry_Wiggle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Carry_Wiggle::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Carry_Wiggle::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pHuntress = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	m_dTime = 0.;
	if (m_pCamper_GamePlay->m_bControl) {		
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_effort_hard"), 21, -1, 0.5f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_effort_hard"), 11, -1, 0.5f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_effort_hard"), 25, -1, 0.5f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_effort_hard"), 13, -1, 0.5f, FMOD_2D);
			break;
		}
		// set hud
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, -1);
	}
	else {
		
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_effort_hard"), 21, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_effort_hard"), 11, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_effort_hard"), 25, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_effort_hard"), 13, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
			break;
		}
	}
		

	return S_OK;
}

const _ubyte  CFS_Camper_Carry_Wiggle::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (nullptr != m_pHuntress) {
		CModel* pHuntressModel = dynamic_cast<CModel*>(m_pHuntress->Get_Component(TEXT("Model_FPV")));
		CTransform* pHuntressTransform = dynamic_cast<CTransform*>(m_pHuntress->Get_Component(TEXT("Transform")));
		_matrix SocketMatrix;
		switch (m_pHuntress->Get_Slasher()) {
		case false:
			SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_Char") * pHuntressTransform->Get_Matrix();
			break;
		case true:
			SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_CamperAttach_01") * pHuntressTransform->Get_Matrix();
			break;
		}

		m_pCamper_GamePlay->m_pTransform->Set_Matrix(SocketMatrix);
	}

	if (m_pCamper_GamePlay->m_bControl) {
		//gaugeUI
		m_dTime += dTimeDelta;
		if (m_dTime >= 1.) {
				m_pCamper_GamePlay->m_dWiggleGauge += 4.f;
				m_dTime = 0.;
		}
		//hwang - perk
		CInven* pInven = CInven::Get_Instance();
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_R)) {
			if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::ESCAPE_CARRY))) {
				m_pHuntress->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_DROP_IN);
				m_pHuntress->Get_FM_Slasher()->Update_Network("Set_State");
				m_pCamper_GamePlay->m_bEscapeCarry = true;
				//hwang - score
				CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
				if (pScore != nullptr) {
					pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::SURVIVAL_KILLER_ESCAPE));
					pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::SURVIVAL), 500);
				}
				return CFM_Camper::EState::STUNDROP;
			}
		}
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F9))
			return CFM_Camper::EState::IDLE;

		// set hud
		CUI_Play_Icon_SkillCheck* pSkillCheck = dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"));
		if (CUI_Play_Icon_SkillCheck::EState::FAIL == pSkillCheck->Get_State()) {
			//실패 시에만 
			m_pCamper_GamePlay->m_dWiggleGauge -= 4.f;
			_float fTime = (1.f / 4.f) * static_cast<_float>(100.f - m_pCamper_GamePlay->m_dWiggleGauge);
			pSkillCheck->Set_HitCount(rand() % 2, fTime, CUI_Play_Icon_SkillCheck::ECheck::FAILED, 150);
		}

		if (m_pCamper_GamePlay->m_dWiggleGauge >= 100.f)//14.
		{
			m_pHuntress->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_DROP_IN);
			m_pHuntress->Get_FM_Slasher()->Update_Network("Set_State");
			//hwang - score
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::SURVIVAL_KILLER_ESCAPE));
				pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::SURVIVAL), 500);
			}
			return CFM_Camper::EState::STUNDROP;
		}
	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);

	return CFM_Camper::EState::END;
}
