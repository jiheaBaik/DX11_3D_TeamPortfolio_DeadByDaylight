#include "UI.h"
#include "Shader.h"

_float4x4 CUI::s_ViewMatrix;
_float4x4 CUI::s_ProjMatrix;

CUI::CUI(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CUI::CUI(const CUI& rhs)
	: CObject(rhs) {
}

void CUI::Delete(void) {
	__super::Delete();
}

const HRESULT CUI::Set_RawValue(CShader* const& pShader) {
	pShader->Set_RawValue("g_ViewMatrix", &s_ViewMatrix, sizeof(_float4x4));
	pShader->Set_RawValue("g_ProjMatrix", &s_ProjMatrix, sizeof(_float4x4));
	return S_OK;
}

const HRESULT CUI::Init(const _float& fViewWidth, const _float& fViewHeight) {
	XMStoreFloat4x4(&s_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&s_ProjMatrix, XMMatrixOrthographicLH(fViewWidth, fViewHeight, 0.f, 1.f));
	return S_OK;
}

const _int& CUI::Get_Depth(void) const {
	return m_iDepth;
}

_matrix CUI::Get_ViewMatrix(void) {
	return XMLoadFloat4x4(&s_ViewMatrix);
}

_matrix CUI::Get_ProjMatrix(void) {
	return XMLoadFloat4x4(&s_ProjMatrix);
}