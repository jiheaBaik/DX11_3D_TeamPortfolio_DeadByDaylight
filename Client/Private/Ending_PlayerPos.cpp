#include "pch.h"
#include "Ending_PlayerPos.h"

CEnding_PlayerPos* const CEnding_PlayerPos::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEnding_PlayerPos* pInstnace = new CEnding_PlayerPos(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEnding_PlayerPos::Clone(void* const& pArg) {
	CEnding_PlayerPos* pInstnace = new CEnding_PlayerPos(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEnding_PlayerPos::CEnding_PlayerPos(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEnding_PlayerPos::CEnding_PlayerPos(const CEnding_PlayerPos& rhs)
	: CObject(rhs) {
}

void CEnding_PlayerPos::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}

const HRESULT CEnding_PlayerPos::Init_Create(void) {
	return S_OK;
}

const HRESULT CEnding_PlayerPos::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	return hr;
}

const _ubyte CEnding_PlayerPos::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CEnding_PlayerPos::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CEnding_PlayerPos::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CEnding_PlayerPos::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CEnding_PlayerPos::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CEnding_PlayerPos::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
}

void CEnding_PlayerPos::Set_Using(const _bool& bUsing) {
	m_bUsing = bUsing;
}

const _bool& CEnding_PlayerPos::Get_Using(void) {
	return m_bUsing;
}
