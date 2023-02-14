#include "pch.h"
#include "Sky.h"
#include "Slasher_Huntress.h"

CSky* const CSky::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CSky* pInstnace = new CSky(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CSky::Clone(void* const& pArg) {
	CSky* pInstnace = new CSky(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CSky::CSky(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CSky::CSky(const CSky& rhs)
	: CObject(rhs) {
}

void CSky::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

const HRESULT CSky::Init_Create(void) {
	return S_OK;
}

const HRESULT CSky::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Model_Sky2"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	return hr;
}

const _ubyte CSky::Update(const _double& dTimeDelta) {	
	return OBJ_NOEVENT;
}

void CSky::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CSky::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	CTransform* pCamPos = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Transform")));
	if (nullptr != pCamPos)
		m_pTransform->Set_Row(CTransform::ERow::POS, pCamPos->Get_Row(CTransform::ERow::POS) - XMVectorSet(0.f, 1.f, 0.f, 0.f));
	
	m_pRenderer->Add_Object(CRenderer::EOrder::PRIORITY, this);
}

const HRESULT CSky::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::PRIORITY: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
			m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
				m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel->Render(i, m_pShader, 1);
			}
		break;
	}
	}
	return S_OK;
}

const HRESULT CSky::Save_Data(Json::Value& root){
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CSky::Load_Data(Json::Value& root){
	m_pTransform->Load_Data(root["Transform"]);
}
