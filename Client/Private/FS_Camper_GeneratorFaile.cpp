#include "pch.h"
#include "FS_Camper_GeneratorFaile.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Generator.h"

CFS_Camper_GeneratorFaile::CFS_Camper_GeneratorFaile(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_GeneratorFaile::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;

}

const HRESULT CFS_Camper_GeneratorFaile::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	if (m_pCamper_GamePlay->m_bControl) {
		m_pGenerator = static_cast<CInter_Generator*>(m_pCamper_GamePlay->m_pTarObject);
		m_iDir = m_pCamper_GamePlay->m_iTarObjDir;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_effort_hard"), 5, -1, 0.5f, FMOD_2D);
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

	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Generator"), m_pCamper_GamePlay->m_iTarObjID);
		m_pGenerator = static_cast<CInter_Generator*>(m_pCamper_GamePlay->m_pTarObject);
		m_iDir = m_pCamper_GamePlay->m_iTarObjDir;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_effort_hard"), 5, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_effort_hard"), 11, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_effort_hard"), 25, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_effort_hard"), 13, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		}
	}
	return S_OK;
}

const _ubyte  CFS_Camper_GeneratorFaile::Update(const _double& dTimeDelta) {
	
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
		m_pGenerator->Set_Exist(m_iDir, false);
		m_pGenerator->Update_Network("Set_PlaceID");
		if (2 == m_pCamper_GamePlay->Get_Hp())
			return CFM_Camper::EState::IDLE;
		else if (1 == m_pCamper_GamePlay->Get_Hp())
			return CFM_Camper::EState::HIT_IDLE;
	}

	enum Dir { FRONT, BACK, LEFT, RIGHT, END };
	if (m_pCamper_GamePlay->m_bControl) {

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
			switch (m_pCamper_GamePlay->m_iTarObjDir) {
			case FRONT:
				return CFM_Camper::EState::GENERATOR_IDLEF;
			case BACK:
				return CFM_Camper::EState::GENERATOR_IDLEB;
			case LEFT:
				return CFM_Camper::EState::GENERATOR_IDLEL;
			case RIGHT:
				return CFM_Camper::EState::GENERATOR_IDLER;
			}
	}
	else{
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				switch (m_pCamper_GamePlay->m_iTarObjDir) {
				case FRONT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Generator_Idle_FT_RTFoot");
				case BACK:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Generator_Idle_BK_RTFoot");
				case LEFT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Generator_Idle_LT_RTFoot");
				case RIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Generator_Idle_RT_RTFoot");
				}
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				switch (m_pCamper_GamePlay->m_iTarObjDir) {
				case FRONT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Generator_Idle_FT_RTFoot");
				case BACK:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Generator_Idle_BK_RTFoot");
				case LEFT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Generator_Idle_LT_RTFoot");
				case RIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Generator_Idle_RT_RTFoot");
				}
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				switch (m_pCamper_GamePlay->m_iTarObjDir) {
				case FRONT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Generator_Idle_FT_RTFoot");
				case BACK:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Generator_Idle_BK_RTFoot");
				case LEFT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Generator_Idle_LT_RTFoot");
				case RIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Generator_Idle_RT_RTFoot");
				}
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				switch (m_pCamper_GamePlay->m_iTarObjDir) {
				case FRONT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Generator_Idle_FT_RTFoot");
				case BACK:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Generator_Idle_BK_RTFoot");
				case LEFT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Generator_Idle_LT_RTFoot");
				case RIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Generator_Idle_RT_RTFoot");
				}
				break;
			}

			
		}
	}
	return CFM_Camper::EState::END;
}
