#include "pch.h"
#include "FS_Camper_CrawIdle.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_ChestBox.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"
#include "Effect_BloodStain.h"
CFS_Camper_CrawIdle::CFS_Camper_CrawIdle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_CrawIdle::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_CrawIdle::Init(_char* pModelName) {

	CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_matrix m_fRootOffsetMatrix = XMLoadFloat4x4(&m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char"));
	_matrix pivot = m_fRootOffsetMatrix * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_Char") * matrix;
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, -pivot.r[0]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, -pivot.r[2]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, -pivot.r[1]);
	//m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f), 1.f);
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName, 0);
	m_pCamper_GamePlay->m_pModel->Update_Anim(0.f);

	return S_OK;
}

const _ubyte  CFS_Camper_CrawIdle::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {
		//if (pGame_Instance->Get_Input_KeyboardKey(DIK_1))//임시
		//{
		//	m_pCamper_GamePlay->m_iHP = 2;
		//	return CFM_Camper::EState::Idle;
		//}
		//if (pGame_Instance->Get_Input_KeyboardKey(DIK_2))//임시
		//{
		//	return CFM_Camper::EState::Crawl_Pick1;
		//}
		/*if (pGame_Instance->Get_Input_KeyboardKey(DIK_1))
			return CFM_Camper::EState::Idle;*/
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F8))
			return CFM_Camper::EState::IDLE;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W))
			return CFM_Camper::EState::CRAWL_WALK;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S))
			return CFM_Camper::EState::CRAWL_WALK;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A))
			return CFM_Camper::EState::CRAWL_WALK;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D))
			return CFM_Camper::EState::CRAWL_WALK;
		/*if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F9))
			return CFM_Camper::EState::Idle;*/
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_SPACE)) {
			switch (m_pCamper_GamePlay->Get_CollisionState())
			{
			case CCamper_GamePlay::ECollisionState::HATCH:
				return CFM_Camper::EState::HATCH_CRAWLOUT;
			}
		}
	}
	// effect - 221111 by JSY
	//m_fDist += XMVectorGetX(XMVector3Length(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) - XMLoadFloat3(&m_fPrePos)));
	//XMStoreFloat3(&m_fPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));
	//if (m_fDist > 3.f) {
	//	CEffect_BloodStain* pBloodStain = dynamic_cast<CEffect_BloodStain*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_BloodStain"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_BloodStain")));
	//	CTransform* pTransform = dynamic_cast<CTransform*>(pBloodStain->Get_Component(TEXT("Transform")));
	//	pTransform->Set_Row(CTransform::ERow::POS, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));
	//	m_fDist = 0.f;
	//}
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
