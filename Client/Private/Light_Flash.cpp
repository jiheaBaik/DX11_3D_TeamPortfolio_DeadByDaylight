#include "pch.h"
#include "Light_Flash.h"

CLight_Flash* const CLight_Flash::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CLight_Flash* pInstnace = new CLight_Flash(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CLight_Flash::Clone(void* const& pArg) {
	CLight_Flash* pInstnace = new CLight_Flash(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CLight_Flash::CLight_Flash(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CLight(pDevice, pContext, pNetwork) {
}

CLight_Flash::CLight_Flash(const CLight_Flash& rhs)
	: CLight(rhs) {
}

void CLight_Flash::Delete(void) {
	__super::Delete();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}

const HRESULT CLight_Flash::Init_Create(void) {
	return S_OK;
}

const HRESULT CLight_Flash::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	m_bOnOff = false;
	return hr;
}

const _ubyte CLight_Flash::Update(const _double& dTimeDelta) {
	//m_pCollider->Update(m_pTransform->Get_Matrix());
	if (m_bAttack) {
		m_sDesc.fAngle.x = 0.75f;
		m_dAttackTime += dTimeDelta;
		if (m_dAttackTime < 1.5) {
			m_sDesc.fAngle.x += static_cast<_float>(dTimeDelta * 0.075);
			if (m_sDesc.fAngle.x > 0.95)
				m_sDesc.fAngle.x = 0.95f;
		}
		else if (m_dAttackTime < 1.8) {
			m_sDesc.fAngle.x -= static_cast<_float>(dTimeDelta * 3 / 2);
			if (m_sDesc.fAngle.x < 0.75f)
				m_sDesc.fAngle.x = 0.75f;
		}
		else if (m_dAttackTime < 2.) {
			m_sDesc.fAngle.x += static_cast<_float>(dTimeDelta);
			if (m_sDesc.fAngle.x > 0.95f)
				m_sDesc.fAngle.x = 0.95f;
		}

		if (m_dAttackTime >= 2.f) {
			m_sDesc.fAngle.x = 0.95f;
			m_bAttack = false;
		}
	}
	
	return OBJ_NOEVENT;
}

void CLight_Flash::Late_Update(const _double& dTimeDelta) {	
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CLight_Flash::Render(const CRenderer::EOrder& eOrder) {
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	//m_pCollider->Render(pCamera);
	return S_OK;
}
const HRESULT CLight_Flash::Save_Data(Json::Value& root) {
	return E_FAIL;
}

void CLight_Flash::Load_Data(Json::Value& root) {
}
