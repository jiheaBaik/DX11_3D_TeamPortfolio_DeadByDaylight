#include "pch.h"
#include "Camera_Ending.h"

CCamera_Ending* const CCamera_Ending::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCamera_Ending* pInstnace = new CCamera_Ending(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCamera_Ending::Clone(void* const& pArg) {
	CCamera_Ending* pInstnace = new CCamera_Ending(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCamera_Ending::CCamera_Ending(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CCamera(pDevice, pContext, pNetwork) {
}

CCamera_Ending::CCamera_Ending(const CCamera_Ending& rhs)
	: CCamera(rhs) {
}

void CCamera_Ending::Delete(void) {
	__super::Delete();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
	Safe_Release(m_pAudio);
}

const HRESULT CCamera_Ending::Init_Create(void) {
	return S_OK;
}

const HRESULT CCamera_Ending::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));

	s_vecCamera.emplace_back(this);
	return hr;
}

const _ubyte CCamera_Ending::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pCollider->Update(m_pTransform->Get_Matrix());

	if (s_vecCamera.front() == this)
		m_pAudio->Set_Listener(m_pTransform);
	return OBJ_NOEVENT;
}

void CCamera_Ending::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CCamera_Ending::Late_Update(const _double& dTimeDelta) {
	Set_ViewMatrix();
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CCamera_Ending::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CCamera_Ending::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
};

void CCamera_Ending::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
	s_vecCamera.emplace_back(this);
	s_mapCamera.emplace(TEXT("Ending"), this);
};