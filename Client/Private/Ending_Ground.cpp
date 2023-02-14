#include "pch.h"
#include "Ending_Ground.h"

CEnding_Ground* const CEnding_Ground::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEnding_Ground* pInstnace = new CEnding_Ground(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEnding_Ground::Clone(void* const& pArg) {
	CEnding_Ground* pInstnace = new CEnding_Ground(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEnding_Ground::CEnding_Ground(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEnding_Ground::CEnding_Ground(const CEnding_Ground& rhs)
	: CObject(rhs) {
}

void CEnding_Ground::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

const HRESULT CEnding_Ground::Init_Create(void) {
	return S_OK;
}

const HRESULT CEnding_Ground::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Ending_Ground"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	return hr;
}

const _ubyte CEnding_Ground::Update(const _double& dTimeDelta) {
	if (m_bActive) {
		m_pTransform->Translate(XMVectorSet(-1.f, 0.f, 0.f, 0.f), m_fSpeed, dTimeDelta);
		if (XMVectorGetX(m_pTransform->Get_Row(CTransform::ERow::POS)) < m_fGoal)
			m_bActive = false;
	}
	return OBJ_NOEVENT;
}

void CEnding_Ground::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CEnding_Ground::Late_Update(const _double& dTimeDelta) {
	if (m_bActive)
		m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
}

const HRESULT CEnding_Ground::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
		m_pShader->Set_RawValue("g_fRefract", &m_fRefract, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			m_pModel->Render(i, m_pShader, 9);
		}
		break;
	}
	}
	return S_OK;
}

void CEnding_Ground::Set_Active(const _bool& bActive) {
	m_bActive = bActive;
}

const _bool& CEnding_Ground::Get_Active(void) {
	return m_bActive;
}
