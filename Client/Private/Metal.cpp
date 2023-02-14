#include "pch.h"
#include "Metal.h"

CMetal* const CMetal::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CMetal* pInstnace = new CMetal(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CMetal::Clone(void* const& pArg) {
	CMetal* pInstnace = new CMetal(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CMetal::CMetal(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CMetal::CMetal(const CMetal& rhs)
	: CObject(rhs) {
}

void CMetal::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

const HRESULT CMetal::Init_Create(void) {
	return S_OK;
}

const HRESULT CMetal::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Model_Sphere"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	return S_OK;
}

const _ubyte CMetal::Update(const _double& dTimeDelta) {
	return OBJ_NOEVENT;
}

void CMetal::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	ImGui::DragFloat("Roughness", &m_fRoughness, 0.001f);
	ImGui::DragFloat("Metalness", &m_fMetalness, 0.001f);
}

void CMetal::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
}

const HRESULT CMetal::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
				m_pModel->Render(i, m_pShader, 0);
			else
				m_pModel->Render(i, m_pShader, 8);
		}
		break;
	}
	}
	return S_OK;
}
