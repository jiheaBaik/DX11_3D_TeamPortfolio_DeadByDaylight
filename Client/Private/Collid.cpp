#include "pch.h"
#include "Collid.h"

CCollid* const CCollid::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCollid* pInstnace = new CCollid(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCollid::Clone(void* const& pArg) {
	CCollid* pInstnace = new CCollid(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCollid::CCollid(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CCollid::CCollid(const CCollid& rhs)
	: CObject(rhs) {
}

void CCollid::Delete(void){
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}

const HRESULT CCollid::Init_Create(void) {
	return S_OK;
}

const HRESULT CCollid::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	return hr;
}

const _ubyte CCollid::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CCollid::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	m_pCollider->Update_Edit(1);
}

void CCollid::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CCollid::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}
