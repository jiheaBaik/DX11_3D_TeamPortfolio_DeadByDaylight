#include "pch.h"
#include "Setting_CamperPos.h"

CSetting_CamperPos* const CSetting_CamperPos::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CSetting_CamperPos* pInstnace = new CSetting_CamperPos(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CSetting_CamperPos::Clone(void* const& pArg) {
	CSetting_CamperPos* pInstnace = new CSetting_CamperPos(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CSetting_CamperPos::CSetting_CamperPos(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CSetting_CamperPos::CSetting_CamperPos(const CSetting_CamperPos& rhs)
	: CObject(rhs) {
}

void CSetting_CamperPos::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}

const HRESULT CSetting_CamperPos::Init_Create(void) {
	return S_OK;
}

const HRESULT CSetting_CamperPos::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	m_pCollider->Set_Extents(_float3{ 0.1f, 0.1f, 0.1f });
	return hr;
}

const _ubyte CSetting_CamperPos::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CSetting_CamperPos::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CSetting_CamperPos::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CSetting_CamperPos::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CSetting_CamperPos::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CSetting_CamperPos::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
}
