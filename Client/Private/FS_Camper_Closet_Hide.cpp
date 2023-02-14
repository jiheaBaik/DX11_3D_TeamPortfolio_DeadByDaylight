#include "pch.h"
#include "FS_Camper_Closet_Hide.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"
#include "UI_Play_Icon_Hud.h"

CFS_Camper_Closet_Hide::CFS_Camper_Closet_Hide(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Closet_Hide::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Closet_Hide::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	m_pCamper_GamePlay->m_bFlag = true;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pLocker = static_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
		// set hud
		//dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ECamper::HIDE));
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Locker"), m_pCamper_GamePlay->m_iTarObjID);
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}
	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	//m_pLocker->Set_IsInCamper(true);
	m_pLockerTarnsform = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform")));
	m_pLockerTransform2 = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("ColTransform")));
	m_pCamper_GamePlay->m_pModel->Update_Anim(0, true);

	XMStoreFloat3(&m_fRootPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));
	return S_OK;
}

const _ubyte  CFS_Camper_Closet_Hide::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	XMMATRIX LockerMatrix = m_pLockerTransform2->Get_Matrix() * m_pLockerTarnsform->Get_Matrix();
	_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLerpPos = LockerMatrix.r[3] * 0.2f + XMLoadFloat3(&m_fRootPrePos) * 0.8f;
	XMStoreFloat3(&m_fRootPrePos, vLerpPos);
	//m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos); 
	_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
	_vector vLockerQaut = XMQuaternionRotationMatrix(LockerMatrix);
	_vector vResultQuat = XMQuaternionSlerp(vQuat, vLockerQaut, 0.2f);
	_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

	_vector vRootPos;
	_float4x4 vSocket;
	CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
	vRootPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vRootPos);

	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 1.2f, false)) {
			//m_pLocker->Set_State(CInter_Locker::ESTATE::IDLE);
			return CFM_Camper::EState::CLOSSET_IDLE;
		}
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_ClosetIdle");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_ClosetIdle");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_ClosetIdle");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_ClosetIdle");
				break;
			}
		}
	}
	return CFM_Camper::EState::END;
}
