#include "pch.h"
#include "FS_Camper_Parkour_Grab_In.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Slasher_Huntress.h"


CFS_Camper_Parkour_Grab_In::CFS_Camper_Parkour_Grab_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Parkour_Grab_In::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Parkour_Grab_In::Init(_char* ModelName) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName, 0);
	m_pCamper_GamePlay->m_pModel->Update_Anim(0, true);

	if (m_pCamper_GamePlay->m_bControl) {
		m_pSlasher = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), m_pCamper_GamePlay->m_iTarObjID);;
	}
	else {

		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), m_pCamper_GamePlay->m_iTarObjID);
		
		if (m_pCamper_GamePlay->m_pTarObject != nullptr)
			m_pSlasher = m_pCamper_GamePlay->m_pTarObject;
		/*else {
			m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_¿À´Ï"), m_pCamper_GamePlay->m_iTarObjID);
			m_pSlasher = m_pCamper_GamePlay->m_pTarObject;
		}*/
	}


	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);

	XMStoreFloat3(&m_fRootPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));

	return S_OK;
}

const _ubyte  CFS_Camper_Parkour_Grab_In::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	
	if (nullptr != m_pSlasher) {
		CModel* pHuntressModel = dynamic_cast<CModel*>(m_pSlasher->Get_Component(TEXT("Model_FPV")));
		CTransform* pHuntressTransform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
		_matrix SocketMatrix;
		switch (dynamic_cast<CSlasher_Huntress*>(m_pSlasher)->Get_Slasher()) {
		case false:
			SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_CamperAttach_01") * pHuntressTransform->Get_Matrix();
			break;
		case true:
			SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_CamperAttach_01") * pHuntressTransform->Get_Matrix();
			break;
		}
		//_matrix SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_CamperAttach_01") * pHuntressTransform->Get_Matrix();
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);

		//m_pCamper_GamePlay->m_pTransform->Set_Matrix(SocketMatrix);

	}

	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
			return CFM_Camper::EState::PARKOUR_GRAB_F;
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_TT_Grab_Obstacles_FT");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_TT_Grab_Obstacles_FT");
				break;
			}
		}
	}

	if (m_pCamper_GamePlay->m_bControl) {
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(4)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.5f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.5f, FMOD_2D);
				break;
			}
		}

	}
	else {
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(4)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 10.f });
				break;
			}
		}
	}

	return CFM_Camper::EState::END;
}
