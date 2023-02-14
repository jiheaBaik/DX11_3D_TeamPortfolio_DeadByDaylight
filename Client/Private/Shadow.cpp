#include "pch.h"
#include "Shadow.h"

CShadow* const CShadow::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CShadow* pInstnace = new CShadow(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CShadow::Clone(void* const& pArg) {
	CShadow* pInstnace = new CShadow(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CShadow::CShadow(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CShadow::CShadow(const CShadow& rhs)
	: CObject(rhs) {
}

void CShadow::Delete(void) {
	__super::Delete();
	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
}

const HRESULT CShadow::Init_Create(void) {
	return S_OK;
}

const HRESULT CShadow::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 800.f / 600.f, 0.2f, 1000.f));
	return hr;
}

const _ubyte CShadow::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CShadow::Update_Edit(void) {
	ImGui::InputFloat("Dist", &m_fDist);
	m_pTransform->Update_Edit(0);
}

void CShadow::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CLight* pLight = CLight::Get_Light(TEXT("Dir"));
	if (nullptr == pLight) return;
	CTransform* pLightTransform = dynamic_cast<CTransform*>(pLight->Get_Component(TEXT("Transform")));
	m_pTransform->Set_Matrix(pLightTransform->Get_Matrix());

	//CObject* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	//if (nullptr == pCamera) return;
	//CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Transform")));
	//m_pTransform->Set_Row(CTransform::ERow::POS, pCameraTransform->Get_Row(CTransform::ERow::POS) - pLightTransform->Get_Row(CTransform::ERow::LOOK) * m_fDist);

}

const HRESULT CShadow::Render(const CRenderer::EOrder& eOrder) {
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	//m_pCollider->Render(pCamera);
	return S_OK;
}

const HRESULT CShadow::Set_RawValue(CShader* const& pShader) {
	pShader->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(m_pTransform->Get_Matrix_Inverse()), sizeof(_float4x4));
	pShader->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));
	return S_OK;
}

const HRESULT CShadow::Save_Data(Json::Value& root) {
	root["Dist"] = m_fDist;
	return S_OK;
}

void CShadow::Load_Data(Json::Value& root) {
	m_fDist = root["Dist"].asFloat();
}
