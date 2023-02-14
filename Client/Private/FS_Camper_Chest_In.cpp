#include "pch.h"
#include "FS_Camper_Chest_In.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_ChestBox.h"

CFS_Camper_Chest_In::CFS_Camper_Chest_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Chest_In::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Chest_In::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);

	if (m_pCamper_GamePlay->m_bControl) {
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_ChestBox"), m_pCamper_GamePlay->m_iTarObjID);
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
	}

	m_pChestBoxTransform = dynamic_cast<CTransform*>(m_pChestBox->Get_Component(TEXT("Transform")));
	m_pChestBoxTransform2 = dynamic_cast<CTransform*>(m_pChestBox->Get_Component(TEXT("ColTransform")));
	return S_OK;
}

const _ubyte  CFS_Camper_Chest_In::Update(const _double& dTimeDelta) {
	XMMATRIX ChestMatrix = m_pChestBoxTransform2->Get_Matrix() * m_pChestBoxTransform->Get_Matrix();
	_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLerpPos = ChestMatrix.r[3] * 0.2f + vPos * 0.8f;
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

	_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
	_vector vChestQaut = XMQuaternionRotationMatrix(ChestMatrix);
	_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
	_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

	if (m_pCamper_GamePlay->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			m_pChestBox->Set_State(CInter_ChestBox::ESTATE::IDLE);
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}
		//랜덤 아이템 줍기
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_pChestBox->Set_State(CInter_ChestBox::ESTATE::OPEN_STEP2);
			return CFM_Camper::EState::CHEST_LOOP;
		}
	}
	else {
		//랜덤 아이템 줍기
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Chest_Out");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Chest_Out");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Chest_Out");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Chest_Out");
				break;
			}
		}
	}
	return CFM_Camper::EState::END;
}
