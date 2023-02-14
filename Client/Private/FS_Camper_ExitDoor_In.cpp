#include "pch.h"
#include "FS_Camper_ExitDoor_In.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_ExitDoor.h"

CFS_Camper_ExitDoor_In::CFS_Camper_ExitDoor_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_ExitDoor_In::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_ExitDoor_In::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	
	if (m_pCamper_GamePlay->m_bControl) {
		m_pExitDoor = static_cast<CInter_ExitDoor*>(m_pCamper_GamePlay->m_pTarObject);		
		
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
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_ExitDoor"), m_pCamper_GamePlay->m_iTarObjID);
		m_pExitDoor = static_cast<CInter_ExitDoor*>(m_pCamper_GamePlay->m_pTarObject);
		
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

	m_pExitDoorTransform = dynamic_cast<CTransform*>(m_pExitDoor->Get_Component(TEXT("Transform")));
	m_pExitDoorModel = dynamic_cast<CModel*>(m_pExitDoor->Get_Component(TEXT("Model")));
	m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_exitdoor_switch_lever_pull"), 3, -1, 1.f, FMOD_3D, dynamic_cast<CTransform*>(m_pExitDoor->Get_Component(TEXT("Transform"))), _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
	return S_OK;
}

const _ubyte  CFS_Camper_ExitDoor_In::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {
		_matrix ExitDoorPosMatrix = m_pExitDoorModel->Get_BoneMatrix("joint_AttachCamper") * m_pExitDoorTransform->Get_Matrix();

		_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
		_vector vLerpPos = ExitDoorPosMatrix.r[3] * 0.2f + vPos * 0.8f;
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

		_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(ExitDoorPosMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

		m_pCamper_GamePlay->m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			m_pExitDoor->Set_State(CInter_ExitDoor::ESTATE::CLOSED, true);
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			return CFM_Camper::EState::EXITDOOR_LOOP;
		}
	}
	else {
		_matrix ExitDoorPosMatrix = m_pExitDoorModel->Get_BoneMatrix("joint_AttachCamper") * m_pExitDoorTransform->Get_Matrix();

		_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
		_vector vLerpPos = ExitDoorPosMatrix.r[3] * 0.2f + vPos * 0.8f;
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

		_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(ExitDoorPosMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

		m_pCamper_GamePlay->m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_UnlockExitIdle");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_UnlockExitIdle");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_UnlockExitIdle");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_UnlockExitIdle");
				break;
			}
		}
	}

	return CFM_Camper::EState::END;
}
