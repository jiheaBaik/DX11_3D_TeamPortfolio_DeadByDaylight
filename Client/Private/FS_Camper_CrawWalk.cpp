#include "pch.h"
#include "FS_Camper_CrawWalk.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"
#include "Effect_BloodStain.h"

CFS_Camper_CrawWalk::CFS_Camper_CrawWalk(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_CrawWalk::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_CrawWalk::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	return S_OK;
}

const _ubyte  CFS_Camper_CrawWalk::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl)
	{
		CObject* pObj = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Camera_Camper"));
		CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(TEXT("Transform")));

		if (pTransform == nullptr)
			return CFM_Camper::EState::IDLE;

		_bool bDir[4] = { 0, };

		_vector vLook = pTransform->Get_Row(CTransform::ERow::LOOK) * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_vector vRight = pTransform->Get_Row(CTransform::ERow::RIGHT) * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
			vDir += vLook;
			bDir[0] = 1;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S)) {
			vDir -= vLook;
			bDir[1] = 1;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A)) {
			vDir -= vRight;
			bDir[2] = 1;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D)) {
			vDir += vRight;
			bDir[3] = 1;
		}

		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W))
			bDir[0] = 0;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S))
			bDir[1] = 0;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A))
			bDir[2] = 0;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D))
			bDir[3] = 0;

		if (!bDir[0] && !bDir[1] && !bDir[2] && !bDir[3])
			return CFM_Camper::EState::CRAWL_IDLE;


		vDir = XMVector3Normalize(vDir);
		_float fDotDir = XMVectorGetX(XMVector3Dot(XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f), vDir));
		if (fDotDir != 1.f) {
			XMStoreFloat3(&m_pCamper_GamePlay->m_vDotDir, vDir);
			m_pCamper_GamePlay->Get_FM_Camper()->Update_Network("Set_Dir");
		}
		XMStoreFloat3(&m_pCamper_GamePlay->m_vDotDir, vDir);
		if (false == m_pCamper_GamePlay->m_bCollider_Move)
			m_pCamper_GamePlay->m_pTransform->Translate(vDir, 0.3f, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCamper_GamePlay->m_vCollider_Move), vDir));
			if (fDotTarget < 0)
				m_pCamper_GamePlay->m_pTransform->Translate(vDir, 0.3f, dTimeDelta);
		}

		_vector vDot = XMVector3Dot(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);
		_vector vCross = XMVector3Cross(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);

		if (XMVectorGetY(vCross) > 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
		}
		else if (XMVectorGetY(vCross) < 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), -abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
		}
	}
	else {
		if (false == m_pCamper_GamePlay->m_bCollider_Move)
			m_pCamper_GamePlay->m_pTransform->Translate(XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f), 0.3f, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCamper_GamePlay->m_vCollider_Move), XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f)));
			if (fDotTarget < 0)
				m_pCamper_GamePlay->m_pTransform->Translate(XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f), 0.3f, dTimeDelta);
		}

		_vector vDir = XMLoadFloat3(&m_pCamper_GamePlay->m_vDotDir);
		_vector vDot = XMVector3Dot(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);
		_vector vCross = XMVector3Cross(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);

		if (XMVectorGetY(vCross) > 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
		}
		else if (XMVectorGetY(vCross) < 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), -abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
		}
	}
	// effect - 221111 by JSY
	//m_dBloodStainTime += dTimeDelta;
	//if (m_dBloodStainTime > 2.f) {
	//	CEffect_BloodStain* pBloodStain = dynamic_cast<CEffect_BloodStain*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_BloodStain"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_BloodStain")));
	//	CTransform* pTransform = dynamic_cast<CTransform*>(pBloodStain->Get_Component(TEXT("Transform")));
	//	pTransform->Set_Row(CTransform::ERow::POS, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));
	//	m_dBloodStainTime = 0.f;
	//}
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
