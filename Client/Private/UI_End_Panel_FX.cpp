#include "pch.h"
#include "UI_End_Panel_FX.h"
#include "Camera_Lobby.h"

CUI_End_Panel_FX* const CUI_End_Panel_FX::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_End_Panel_FX* pInstnace = new CUI_End_Panel_FX(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_End_Panel_FX::Clone(void* const& pArg) {
	CUI_End_Panel_FX* pInstnace = new CUI_End_Panel_FX(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_End_Panel_FX::CUI_End_Panel_FX(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_End_Panel_FX::CUI_End_Panel_FX(const CUI_End_Panel_FX& rhs)
	: CUI_Image(rhs) {
}

void CUI_End_Panel_FX::Delete(void) {
	__super::Delete();
	Safe_Release(m_pDestTexture);
}

const HRESULT CUI_End_Panel_FX::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_End_Panel_FX::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("back"));
	m_iDepth = -2;
	m_iPass = 1;
	m_vColor.w = 0.f;
	return hr;
}

const _ubyte CUI_End_Panel_FX::Update(const _double& dTimeDelta) {
	if (!m_bStart) {
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf) {
		// reset
		if (m_bPreActive) {
			m_bRender = true;		
		}
		return OBJ_NOEVENT;
	}	
	// fade in
	if (m_bRender)
		this->Set_Fade(true, 0.f, 1.f, (_float)dTimeDelta * 0.5f, !m_bPreActive);
	// fade out
	else {
		m_iDepth = 0;
		this->Set_Fade(false, 0.f, 1.f, (_float)dTimeDelta * 0.5f, !m_bFade);
		if (!m_bFade)
			this->Set_Active(false);
	}
		
	return OBJ_NOEVENT;
}

void CUI_End_Panel_FX::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	ImGui::Checkbox("render", &m_bRender);

	Update_Edit_Images();
}

void CUI_End_Panel_FX::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_End_Panel_FX::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	hr |= __super::Set_RawValue(m_pShader);
	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &m_pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
	hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
	hr |= m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	hr |= m_pVBRect->Render(m_pShader, m_iPass);
#ifdef _DEBUG
	if (m_bCollider)
		m_pCollider->Render();
#endif // _DEBUG

	return hr;
}

const HRESULT CUI_End_Panel_FX::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_End_Panel_FX::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	//m_pRectTransform->Set_Size((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	m_pRectTransform->Set_Size(1920.f, 1080.f);
}
