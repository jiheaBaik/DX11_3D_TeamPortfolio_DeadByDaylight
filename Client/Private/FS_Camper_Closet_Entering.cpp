#include "pch.h"
#include "FS_Camper_Closet_Entering.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"

CFS_Camper_Closet_Entering::CFS_Camper_Closet_Entering(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Closet_Entering::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Closet_Entering::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	if (m_pCamper_GamePlay->m_bControl) {
		m_pLocker = static_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Locker"), m_pCamper_GamePlay->m_iTarObjID);
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}

	m_pLockerTarnsform = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform")));
	m_pLockerTransform2 = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("ColTransform")));
	return S_OK;
}

const _ubyte  CFS_Camper_Closet_Entering::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	XMMATRIX LockerMatrix = m_pLockerTransform2->Get_Matrix() * m_pLockerTarnsform->Get_Matrix();
	_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLerpPos = LockerMatrix.r[3] * 0.2f + vPos * 0.8f;
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

	_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
	_vector vLockerQaut = XMQuaternionRotationMatrix(LockerMatrix);
	_vector vResultQuat = XMQuaternionSlerp(vQuat, vLockerQaut, 0.2f);
	_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

	if (m_pCamper_GamePlay->m_bControl) {

		/*if (!m_bInitalSettig) {
			m_pCamper_GamePlay->Set_Render(true);
			m_bInitalSettig = true;
		}*/
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta *1.2f, false))
		{
			/*CInter_Locker* pColObject = static_cast<CInter_Locker*>(m_pCamper_GamePlay->Get_ColObject());
			CTransform* pTransform = dynamic_cast<CTransform*>(pColObject->Get_Component(TEXT("Transform")));*/
			//m_pCamper_GamePlay->Set_Render(false);
			//pColObject->Set_State(CInter_Locker::ESTATE::IDLE);
			m_bInitalSettig = false;
			return CFM_Camper::EState::IDLE;
		}

	}
	else
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_StandIdle");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_StandIdle");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_StandIdle");
				break;
			}
		}

	return CFM_Camper::EState::END;

}
