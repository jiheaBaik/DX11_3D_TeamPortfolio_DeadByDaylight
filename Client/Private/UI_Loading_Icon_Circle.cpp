#include "pch.h"
#include "UI_Loading_Icon_Circle.h"

CUI_Loading_Icon_Circle* const CUI_Loading_Icon_Circle::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Loading_Icon_Circle* pInstnace = new CUI_Loading_Icon_Circle(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Loading_Icon_Circle::Clone(void* const& pArg) {
	CUI_Loading_Icon_Circle* pInstnace = new CUI_Loading_Icon_Circle(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Loading_Icon_Circle::CUI_Loading_Icon_Circle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Loading_Icon_Circle::CUI_Loading_Icon_Circle(const CUI_Loading_Icon_Circle& rhs)
	: CUI_Image(rhs) {
}

void CUI_Loading_Icon_Circle::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Loading_Icon_Circle::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Loading_Icon_Circle::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone();
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), Get_TextureKey(L"icon_loading").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
	
	m_pRectTransform->Set_Edge(CRectTransform::EEdge::RBOT);
	m_pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-40.f, -35.f, 0.f));
	m_pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.25f, 0.25f, 0.25f));

	m_iPass = 1;
	m_vColor = { 0.8f, 0.8f, 0.8f, 0.5f };
	m_dTime = 0;
	this->Set_Active(false);

	return hr;
}

const _ubyte CUI_Loading_Icon_Circle::Update(const _double& dTimeDelta) {
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;
	if (!m_bPreActive && m_bActiveSelf) {
		m_dTime = 0.;
		m_vColor.w = 1.f;
		m_pRectTransform->Set_State(CRectTransform::EState::ROT, _float3(0.f, 0.f, 0.f));
	}

	m_dTime += dTimeDelta;
	if (3.0 <= m_dTime) {
		if (0 >= m_vColor.w) {
			m_dTime = 0;
			m_vColor.w = 1.f;
			this->Set_Active(false);
			m_pRectTransform->Set_State(CRectTransform::EState::ROT, _float3(0.f, 0.f, 0.f));
		}
		else {
			m_vColor.w -= (_float)dTimeDelta;
		}
	}
	_float fDegree = m_pRectTransform->Get_Desc().vRotation.x;
	fDegree -= (_float)dTimeDelta * 100.f;
	m_pRectTransform->Set_State(CRectTransform::EState::ROT, _float3(0.f, 0.f, fDegree), true);

	return OBJ_NOEVENT;
}

void CUI_Loading_Icon_Circle::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();
}

void CUI_Loading_Icon_Circle::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Loading_Icon_Circle::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	hr |= __super::Render(eOrder);
	hr |= m_pVBRect->Render(m_pShader, m_iPass);
	return hr;
}

