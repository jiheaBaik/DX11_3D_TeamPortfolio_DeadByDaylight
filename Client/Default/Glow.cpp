#include "pch.h"
#include "Glow.h"

CGlow* const CGlow::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CGlow* pInstnace = new CGlow(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CGlow::Clone(void* const& pArg) {
	CGlow* pInstnace = new CGlow(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CGlow::CGlow(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CGlow::CGlow(const CGlow& rhs)
	: CObject(rhs) {
}

void CGlow::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
}

const HRESULT CGlow::Init_Create(void) {
	return S_OK;
}

const HRESULT CGlow::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Tex"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_loadings"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));
	return hr;
}

const _ubyte CGlow::Update(const _double& dTimeDelta) {
	return OBJ_NOEVENT;
}

void CGlow::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	ImGui::DragFloat4("Color", (float*)&m_fColor, 0.001f);
}

void CGlow::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::GLOW, this);
}

const HRESULT CGlow::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::GLOW: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_fColor", &m_fColor, sizeof(_float4));
		m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		m_pBuffer->Render(m_pShader, 0);

		break;
	}
	}
	return S_OK;
}
