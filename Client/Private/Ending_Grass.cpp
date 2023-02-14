#include "pch.h"
#include "Ending_Grass.h"

CEnding_Grass* const CEnding_Grass::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEnding_Grass* pInstnace = new CEnding_Grass(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEnding_Grass::Clone(void* const& pArg) {
	CEnding_Grass* pInstnace = new CEnding_Grass(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEnding_Grass::CEnding_Grass(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEnding_Grass::CEnding_Grass(const CEnding_Grass& rhs)
	: CObject(rhs) {
}

void CEnding_Grass::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

const HRESULT CEnding_Grass::Init_Create(void) {
	return S_OK;
}

const HRESULT CEnding_Grass::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelInst"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Inst_grass03"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));

	m_pModel->Set_Inst_Random(50, 15, 15);
	m_pModel->Set_Inst_Rotate();
	m_pModel->Set_Inst_Scale(_float3{ 1.f,1.f,1.f }, _float3{ 1.2f,1.2f, 1.2f });
	return hr;
}

const _ubyte CEnding_Grass::Update(const _double& dTimeDelta) {
	if (m_bActive) {
		m_pTransform->Translate(XMVectorSet(-1.f, 0.f, 0.f, 0.f), m_fSpeed, dTimeDelta);
		if (XMVectorGetX(m_pTransform->Get_Row(CTransform::ERow::POS)) < m_fGoal)
			m_bActive = false;
	}
	return OBJ_NOEVENT;
}

void CEnding_Grass::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CEnding_Grass::Late_Update(const _double& dTimeDelta) {
	if (m_bActive)
		m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
}

const HRESULT CEnding_Grass::Render(const CRenderer::EOrder& eOrder) {
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
			if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
				m_pModel->Render(i, m_pShader, 0);
			else
				m_pModel->Render(i, m_pShader, 2);
		}
		break;
	}
	}
	return S_OK;
}

void CEnding_Grass::Set_Active(const _bool& bActive) {
	m_bActive = bActive;
}

const _bool& CEnding_Grass::Get_Active(void) {
	return m_bActive;
}
