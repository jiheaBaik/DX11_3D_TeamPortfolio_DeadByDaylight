#include "pch.h"
#include "UI_Play_Panel_Second.h"
#include "Camera_Camper.h"
#include "Camera_Slasher.h"

CUI_Play_Panel_Second* const CUI_Play_Panel_Second::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Panel_Second* pInstnace = new CUI_Play_Panel_Second(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Panel_Second::Clone(void* const& pArg) {
	CUI_Play_Panel_Second* pInstnace = new CUI_Play_Panel_Second(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Panel_Second::CUI_Play_Panel_Second(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Panel_Second::CUI_Play_Panel_Second(const CUI_Play_Panel_Second& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Panel_Second::Delete(void) {
	__super::Delete();
	m_sFont.Release();
}

const HRESULT CUI_Play_Panel_Second::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Play_Panel_Second::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("08test"));
	return hr;
}

const _ubyte CUI_Play_Panel_Second::Update(const _double& dTimeDelta) {
	if (!m_bStart) {
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CUI_Play_Panel_Second::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	Update_Edit_Font(&m_sFont);
	Update_Edit_Images();
}

void CUI_Play_Panel_Second::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
	if (nullptr != m_sFont.pRectTransform)
		m_sFont.pRectTransform->Update_Desc();
}

const HRESULT CUI_Play_Panel_Second::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_vecImages) {
		if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
			continue;
		if (iter.bCollider)
			iter.pCollider->Render();

		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);

		if (ETexType::GEN == static_cast<ETexType>(iter.iDepth) && nullptr != m_sFont.pRectTransform)
			Render_Font(&m_sFont, false);
	}

	return hr;
}

void CUI_Play_Panel_Second::Set_FontCnt(const _int& iCnt, const _int& iLimCnt, const _bool& bAcc) {
	if (iCnt <= iLimCnt) { // generator cnt
		m_vecImages[static_cast<_uint>(ETexType::GEN)].bRender = false;
		m_vecImages[static_cast<_uint>(ETexType::EXIT)].bRender = true;
		return;
	}
	__super::Set_FontCnt(&m_sFont, iCnt, bAcc);
}

void CUI_Play_Panel_Second::Set_FontStr(const _tchar* pText, const _bool& bAcc) {
	__super::Set_FontStr(&m_sFont, pText, bAcc);
}

const HRESULT CUI_Play_Panel_Second::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	Save_AddData(L"UI_Play_Panel_Second");
	return S_OK;
}

void CUI_Play_Panel_Second::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_pRectTransform->Set_Size(1920.f, 1080.f);

	Load_AddData(L"UI_Play_Panel_Second");

	// gen cnt
	m_sFont = Add_Font(L"Gothic32", true, _float2(64.f, 64.f), CRectTransform::EEdge::LBOT);
	m_sFont.pRectTransform->Set_State(CRectTransform::EState::POS, _float3(93.f, -170.f, 0.f));
}
