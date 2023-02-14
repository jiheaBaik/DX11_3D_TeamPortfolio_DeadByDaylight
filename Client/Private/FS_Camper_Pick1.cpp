#include "pch.h"
#include "FS_Camper_Pick1.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Slasher_Huntress.h"


CFS_Camper_Pick1::CFS_Camper_Pick1(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Pick1::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Pick1::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pHuntress = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	return S_OK;
}

const _ubyte  CFS_Camper_Pick1::Update(const _double& dTimeDelta) {
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
			SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_CarryLT_01") * pHuntressTransform->Get_Matrix();
			break;
		}
		m_pCamper_GamePlay->m_pTransform->Set_Matrix(SocketMatrix);

	}
	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
			return CFM_Camper::EState::CRAWL_PICK2;
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_TT_PickUp");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_TT_PickUp");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_TT_PickUP_IN");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_TT_PickUP_IN");
				break;
			}
		}
	}

	return CFM_Camper::EState::END;
}
