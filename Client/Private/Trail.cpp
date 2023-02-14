#include "pch.h"
#include "Trail.h"

CTrail* const CTrail::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CTrail* pInstnace = new CTrail(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CTrail::Clone(void* const& pArg) {
	CTrail* pInstnace = new CTrail(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CTrail::CTrail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CTrail::CTrail(const CTrail& rhs)
	: CObject(rhs) {
}

void CTrail::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pCollider);
}

const HRESULT CTrail::Init_Create(void) {
	return S_OK;
}

const HRESULT CTrail::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Tex"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Trail"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Trail"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	return hr;
}

const _ubyte CTrail::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CTrail::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
	_float3 vPos2;
	XMStoreFloat3(&vPos2, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, -0.5f, 0.f, 0.f));
	m_pBuffer->Set_Trail(vPos, vPos2);
	/*if (ImGui::Button("Create")) {
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
		_float3 vPos2;
		XMStoreFloat3(&vPos2, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, -0.5f, 0.f, 0.f));
		m_pBuffer->Set_Trail(vPos, vPos2);
	}*/
}

void CTrail::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::GLOW, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CTrail::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::GLOW: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		m_pBuffer->Render(m_pShader, 2);
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
