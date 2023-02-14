#include "pch.h"
#include "FS_Camper_Fall.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"

CFS_Camper_Fall::CFS_Camper_Fall(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Fall::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Fall::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName, 0.7f);
	m_pCamper_GamePlay->m_bFlag = false;
		
	return S_OK;
}

const _ubyte  CFS_Camper_Fall::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pCamper_GamePlay->m_bControl) {
		if (m_pCamper_GamePlay->m_bIsCollider_Foot) {
			//hwang
			CInven* pInven = CInven::Get_Instance();
			if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FAST))) {
				m_pCamper_GamePlay->m_bFast = true;
				m_pCamper_GamePlay->m_bStopFast = true;
			}
			if (m_pCamper_GamePlay->m_iHP == 1)
				return CFM_Camper::EState::SITTO_HIT_IDLE;
			else
				return CFM_Camper::EState::SITTO_IDLE;
		}
	}
	else {
		if (m_pCamper_GamePlay->m_bIsCollider_Foot) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT");
				break;
			}
		}
	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
