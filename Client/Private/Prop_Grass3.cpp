#include "pch.h"
#include "Prop_Grass3.h"
#include "Camper_GamePlay.h"

CProp_Grass3* const CProp_Grass3::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CProp_Grass3* pInstnace = new CProp_Grass3(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CProp_Grass3::Clone(void* const& pArg) {
	CProp_Grass3* pInstnace = new CProp_Grass3(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CProp_Grass3::CProp_Grass3(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CProp_Grass3::CProp_Grass3(const CProp_Grass3& rhs)
	: CObject(rhs) {
}

void CProp_Grass3::Delete(void) {
	__super::Delete();
	Safe_Release(m_pAudio);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
}

const HRESULT CProp_Grass3::Init_Create(void) {
	return S_OK;
}

const HRESULT CProp_Grass3::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_grass01"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_Sphere"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));
	return hr;
}

const _ubyte CProp_Grass3::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_bSavePos == false) {
		m_bSavePos = true;
		m_vPos = m_pTransform->Get_Row(CTransform::ERow::POS);
		m_mMatirx = m_pTransform->Get_Matrix();
	}

	m_pCollider->Update(m_pTransform->Get_Matrix());
	//collsion
	list<CObject*>* plistCamper = pGame_Instance->Get_Object_ListClone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
	if (nullptr == plistCamper)
		return OBJ_NOEVENT;
	for (auto& iter : *plistCamper) {
		CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
		CCollider* pCamperCollider = dynamic_cast<CCollider*>(pCamper->Get_Component(TEXT("Collider")));
		CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
		if (true == m_pCollider->Is_Collision(pCamperCollider)) {
			m_bRestore = true;

			if (false == m_bKeep) {
				m_fKeep += static_cast<_float>(dTimeDelta);
				if (m_fKeep > 0.2f) {
					m_fKeep = 0.f;
					m_bKeep = true;
				}

				_vector vDist = m_pTransform->Get_Row(CTransform::ERow::POS) - pCamperTransform->Get_Row(CTransform::ERow::POS);
				_float fMaxDist = max(XMVectorGetX(XMVector3Length(vDist)), 0.2f);
				if (fMaxDist >= 0.8f)
					fMaxDist = 35.f;
				else if (fMaxDist >= 0.4f)
					fMaxDist = 25.f;
				else if (fMaxDist >= 0.2f)
					fMaxDist = 20.f;
				_vector vLook = XMVector3Normalize(vDist);
				_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
				_vector vSourRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_pTransform->Get_Matrix()));
				_vector vDestRot = XMQuaternionNormalize(XMQuaternionRotationNormal(vRight, XMConvertToRadians(fMaxDist)));
				_matrix vResultMat = XMMatrixRotationQuaternion(XMQuaternionSlerp(vSourRot, vDestRot, 0.1f));
				m_pTransform->Set_Matrix(vResultMat);
				m_pTransform->Set_Row(CTransform::ERow::POS, m_vPos);
				if (m_bAudio == false) {
					m_pAudio->Play_Sound_Rand(TEXT("sfx_tallgrass"), 10, -1, 0.2f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
					m_bAudio = true;
				}
			}
			else {
				_vector vSourRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_pTransform->Get_Matrix()));
				_vector vDestRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_mMatirx));
				_matrix vResultMat = XMMatrixRotationQuaternion(XMQuaternionSlerp(vSourRot, vDestRot, 0.05f));
				m_pTransform->Set_Matrix(vResultMat);
				m_pTransform->Set_Row(CTransform::ERow::POS, m_vPos);
				_float fDot = XMVectorGetX(XMQuaternionDot(vSourRot, vDestRot));
			}
		}
		else {
			m_fKeep = 0.f;
			m_bKeep = false;
			if (m_bRestore == true) {
				_vector vSourRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_pTransform->Get_Matrix()));
				_vector vDestRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_mMatirx));
				_matrix vResultMat = XMMatrixRotationQuaternion(XMQuaternionSlerp(vSourRot, vDestRot, 0.05f));
				m_pTransform->Set_Matrix(vResultMat);
				m_pTransform->Set_Row(CTransform::ERow::POS, m_vPos);
				_float fDot = XMVectorGetX(XMQuaternionDot(vSourRot, vDestRot));
				if (fDot == 1.f) {
					m_bRestore = false;
					m_bAudio = false;
				}
			}
		}
	}

	//CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//if (m_bSavePos == false) {
	//	m_bSavePos = true;
	//	m_vPos = m_pTransform->Get_Row(CTransform::ERow::POS);
	//	m_mMatirx = m_pTransform->Get_Matrix();
	//}

	//m_pCollider->Update(m_pTransform->Get_Matrix());
	////collsion
	//list<CObject*>* plistObject = pGame_Instance->Get_Object_ListClone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_Cheryl"));
	//if (nullptr == plistObject)
	//	return OBJ_NOEVENT;
	//for (auto& iter : *plistObject) {
	//	CCamper_Cheryl* pCamper = dynamic_cast<CCamper_Cheryl*>(iter);
	//	CCollider* pCamperCollider = dynamic_cast<CCollider*>(pCamper->Get_Component(TEXT("Collider")));
	//	CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
	//	if (true == m_pCollider->Is_Collision(pCamperCollider)) {
	//		m_bRestore = true;
	//		_vector vDist = m_pTransform->Get_Row(CTransform::ERow::POS) - pCamperTransform->Get_Row(CTransform::ERow::POS);
	//		_float fMaxDist = max(XMVectorGetX(XMVector3Length(vDist)), 0.2f);
	//		if (fMaxDist >= 0.8f)
	//			fMaxDist = 20.f;
	//		else if (fMaxDist >= 0.4f)
	//			fMaxDist = 20.f;
	//		else if (fMaxDist >= 0.2f)
	//			fMaxDist = 20.f;
	//		_vector vLook = XMVector3Normalize(vDist);
	//		_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	//		_vector vSourRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_pTransform->Get_Matrix()));
	//		_vector vDestRot = XMQuaternionNormalize(XMQuaternionRotationNormal(vRight, XMConvertToRadians(fMaxDist)));
	//		_matrix vResultMat = XMMatrixRotationQuaternion(XMQuaternionSlerp(vSourRot, vDestRot, 0.1f));
	//		m_pTransform->Set_Matrix(vResultMat);
	//		m_pTransform->Set_Row(CTransform::ERow::POS, m_vPos);
	//	}
	//	else {
	//		if (m_bRestore == true) {
	//			_vector vSourRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_pTransform->Get_Matrix()));
	//			_vector vDestRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(m_mMatirx));
	//			_matrix vResultMat = XMMatrixRotationQuaternion(XMQuaternionSlerp(vSourRot, vDestRot, 0.05f));
	//			m_pTransform->Set_Matrix(vResultMat);
	//			m_pTransform->Set_Row(CTransform::ERow::POS, m_vPos);
	//			_float fDot = XMVectorGetX(XMQuaternionDot(vSourRot, vDestRot));
	//			if (fDot == 1.0f)
	//				m_bRestore = false;
	//		}
	//	}
	//}

	return OBJ_NOEVENT;
}

void CProp_Grass3::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	m_pCollider->Update_Edit(1);
}

void CProp_Grass3::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == pGame_Instance->IsIn_Frustum_World(m_pTransform->Get_Row(CTransform::ERow::POS), 15.f))
		m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	//m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CProp_Grass3::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
				m_pModel->Render(i, m_pShader, 0);
			else
				m_pModel->Render(i, m_pShader, 8);
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CProp_Grass3::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CProp_Grass3::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
}