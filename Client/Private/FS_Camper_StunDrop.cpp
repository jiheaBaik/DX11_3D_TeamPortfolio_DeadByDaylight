#include "pch.h"
#include "FS_Camper_StunDrop.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Icon_SkillCheck.h"

CFS_Camper_StunDrop::CFS_Camper_StunDrop(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_StunDrop::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_StunDrop::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pCamper_GamePlay->m_dWiggleGauge = 0.f;
	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);

	if (m_pCamper_GamePlay->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_Active(false);
	}
	return S_OK;
}

const _ubyte  CFS_Camper_StunDrop::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	
	_vector vPos;
	_float4x4 vSocket;
	CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
	vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vPos);


	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			m_pCamper_GamePlay->Set_Hp(1);
			return CFM_Camper::EState::HIT_IDLE;
		}
	}
	else{
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			m_pCamper_GamePlay->Set_Hp(1);
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_inj_StandIdle");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_inj_StandIdle");
				break;
			}
		}
	}



	return CFM_Camper::EState::END;
}
