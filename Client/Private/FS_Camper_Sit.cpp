#include "pch.h"
#include "FS_Camper_Sit.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"

CFS_Camper_Sit::CFS_Camper_Sit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Sit::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Sit::Init(_char* pModelName) {

	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);

	m_pCamper_GamePlay->m_pCollider->Set_Center(_float3(0.f, 0.5f, 0.f));
	m_pCamper_GamePlay->m_pCollider->Set_Extents(_float3(0.25f, 0.5f, 0.25f));

	
	return S_OK;

}

const HRESULT CFS_Camper_Sit::Init(_char* pModelName, _double dLerpTime)
{
	m_pCamper_GamePlay->m_pCollider->Set_Center(_float3(0.f, 0.5f, 0.f));
	m_pCamper_GamePlay->m_pCollider->Set_Extents(_float3(0.25f, 0.5f, 0.25f));
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName, dLerpTime);
	return S_OK;
}

const _ubyte  CFS_Camper_Sit::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {

		if (m_pCamper_GamePlay->m_pFM->m_eState != CFM_Camper::EState::SIT) {
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(1))
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_walk"), 10, -1, 0.2f, FMOD_2D);
		}

		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_LCONTROL)) {
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}

		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::SIT_WALK_F;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_SITWALK;

		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S)) {
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::SIT_WALK_B;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_SITWALK;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A)) {
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::SIT_WALK_F;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_SITWALK;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D)) {
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::SIT_WALK_F;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_SITWALK;
		}

		if (!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT"))
		{
			
			if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 2.f, false)) {
				if (m_pCamper_GamePlay->m_pFM->m_eState != CFM_Camper::EState::SIT) {
					return CFM_Camper::EState::SIT;
				}
				
			}
		}
		else
			m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	}
	else {
		if (!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT"))
		{
			if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 2.f, false)) {
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_CrouchIdle_FootLT");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_CrouchIdle_FootLT");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_CrouchIdle_FootLT");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_CrouchIdle_FootLT");
					break;
				}
			}
		}
		else
			m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	}


	return CFM_Camper::EState::END;
}
