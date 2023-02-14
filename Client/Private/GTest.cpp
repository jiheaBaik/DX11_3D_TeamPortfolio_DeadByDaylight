#include "pch.h"
#include "GTest.h"

CGTest* const CGTest::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CGTest* pInstnace = new CGTest(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CGTest* const CGTest::Clone(void* const& pArg) {
	CGTest* pInstnace = new CGTest(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CGTest::CGTest(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CGTest::CGTest(const CGTest& rhs)
	: CObject(rhs) {
}

void CGTest::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
}

const HRESULT CGTest::Init_Create(void) {
	return S_OK;
}

const HRESULT CGTest::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Tex"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Random"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));
	return hr;
}

const _ubyte CGTest::Update(const _double& dTimeDelta) {
	return OBJ_NOEVENT;
}

void CGTest::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	ImGui::DragFloat2("vGlow_Buffer", (float*)&m_vGlow_Buffer, 0.001f);
}

void CGTest::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::GLOW, this);
}

const HRESULT CGTest::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::GLOW: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_vGlow_Buffer", &m_vGlow_Buffer, sizeof(_float2));
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		m_pBuffer->Render(m_pShader, 3);
		break;
	}
	}
	return S_OK;
}
