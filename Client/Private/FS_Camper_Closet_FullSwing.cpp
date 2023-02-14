#include "pch.h"
#include "FS_Camper_Closet_FullSwing.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"

CFS_Camper_Closet_FullSwing::CFS_Camper_Closet_FullSwing(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Closet_FullSwing::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Closet_FullSwing::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	m_pCamper_GamePlay->m_bFlag = true;

	if (m_pCamper_GamePlay->m_bControl) {
		m_pLocker = static_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Locker"), m_pCamper_GamePlay->m_iTarObjID);
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}

	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);
	return S_OK;
}

const _ubyte  CFS_Camper_Closet_FullSwing::Update(const _double& dTimeDelta) {

	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

			m_pLocker->Set_State(CInter_Locker::ESTATE::IDLE);
			if (2 == m_pCamper_GamePlay->Get_Hp()) {
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::RUN);
						m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
					}
				}
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
					m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::WALK_F);
					m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
				}
				else
					return CFM_Camper::EState::IDLE;
			}
			else if (1 == m_pCamper_GamePlay->Get_Hp()) {
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::HIT_RUN);
						m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
					}
				}
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
					m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::HIT_WALK);
					m_pCamper_GamePlay->m_pFM->Update_Network("Set_State");
				}
				else
					return CFM_Camper::EState::HIT_IDLE;
			}
		}
	}
	else
	{
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			if (2 == m_pCamper_GamePlay->Get_Hp()) {
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|MT_DSkeleton_REF");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_StandIdle");
					break;
				}
			}
			else if (1 == m_pCamper_GamePlay->Get_Hp()) {
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
	}
	_vector vPos;
	_float4x4 vSocket;
	CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
	vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vPos);

	return CFM_Camper::EState::END;
}
