#include "pch.h"
#include "FS_Camper_Parkour_In.h"
#include "Camper_Cheryl.h"
#include "FM_Camper.h"
#include "Inter_Parkour.h"

CFS_Camper_Parkour_In::CFS_Camper_Parkour_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_Cheryl* const& pCamper_Cheryl)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_Cheryl = pCamper_Cheryl;
}

void CFS_Camper_Parkour_In::Delete(void) {
	__super::Delete();
	m_pCamper_Cheryl = nullptr;
}

const HRESULT CFS_Camper_Parkour_In::Init(_char* ModelName) {
	m_pCamper_Cheryl->m_pModel->Set_Anim(ModelName);

	if (m_pCamper_Cheryl->m_bControl) {
		m_pParkour = static_cast<CInter_Parkour*>(m_pCamper_Cheryl->m_pTarObject);
		m_iDir = m_pCamper_Cheryl->m_iTarObjDir;

	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_Cheryl->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Parkour"), m_pCamper_Cheryl->m_iTarObjID);
		m_pParkour = static_cast<CInter_Parkour*>(m_pCamper_Cheryl->m_pTarObject);
		m_iDir = m_pCamper_Cheryl->m_iTarObjDir;
	}

	if (0 == m_pCamper_Cheryl->m_iTarObjDir)
		pParkourTransform2 = dynamic_cast<CTransform*>(m_pParkour->Get_Component(TEXT("Transform_F")));
	else
		pParkourTransform2 = dynamic_cast<CTransform*>(m_pParkour->Get_Component(TEXT("Transform_B")));

	m_pCamper_Cheryl->m_bFlag = true;
	return S_OK;
}

const _ubyte  CFS_Camper_Parkour_In::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	CTransform* pParkourTransform = dynamic_cast<CTransform*>(m_pParkour->Get_Component(TEXT("Transform")));

	XMMATRIX ParkourMatrix = pParkourTransform2->Get_Matrix() * pParkourTransform->Get_Matrix();
	_vector vPos = m_pCamper_Cheryl->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLerpPos = ParkourMatrix.r[3] * 0.4f + vPos * 0.6f;
	m_pCamper_Cheryl->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

	_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_Cheryl->m_pTransform->Get_Matrix());
	_vector vChestQaut = XMQuaternionRotationMatrix(ParkourMatrix);
	_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.4f);
	_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

	m_pCamper_Cheryl->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
	m_pCamper_Cheryl->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
	m_pCamper_Cheryl->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);


	if (m_pCamper_Cheryl->m_bControl) {
		if (ANIM_END == m_pCamper_Cheryl->m_pModel->Update_Anim(dTimeDelta, false))
			return CFM_Camper::EState::Parkour_Jump;
	}
	else {
		if (ANIM_END == m_pCamper_Cheryl->m_pModel->Update_Anim(dTimeDelta, false))
			m_pCamper_Cheryl->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_WindowVault_JUMP");
	}
	return CFM_Camper::EState::End;
}
