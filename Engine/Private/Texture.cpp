#include "Texture.h"
#include "Shader.h"

CTexture* const CTexture::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFileName, const _uint& iNumTexture) {
	CTexture* pInstance = new CTexture(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(pFileName, iNumTexture)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CTexture::Clone(void* const& pArg) {
	CTexture* pInstance = new CTexture(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CTexture::CTexture(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_vecShaderResourceView(rhs.m_vecShaderResourceView) {
	for (auto& iter : m_vecShaderResourceView)
		Safe_AddRef(iter);
}

void CTexture::Delete(void) {
	__super::Delete();
	for (auto& iter : m_vecShaderResourceView)
		Safe_Release(iter);
	m_vecShaderResourceView.clear();
}

const HRESULT CTexture::Init_Create(const _tchar* const& pFileName, const _uint& iTexture) {
	for (_uint i = 0; i < iTexture; ++i) {
		_tchar szExt[MAX_PATH] = TEXT("");
		_tchar szFullPath[MAX_PATH] = TEXT("");

		wsprintf(szFullPath, pFileName, i);
		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		ID3D11ShaderResourceView* pShaderResourceView = nullptr;
		HRESULT hr = 0;
		if (false == lstrcmp(szExt, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pShaderResourceView);
		else
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pShaderResourceView);

		if (FAILED(hr))
			return E_FAIL;
		m_vecShaderResourceView.emplace_back(pShaderResourceView);
	}
	return S_OK;
}

const HRESULT CTexture::Init_Clone(void* const& pArg) {
	return S_OK;
}

const HRESULT CTexture::Set_Resource(CShader* const& pShader, const char* const& pName, const _uint& iIndex) {
	if (nullptr == pShader || iIndex >= m_vecShaderResourceView.size())
		return E_FAIL;
	if (nullptr == m_vecShaderResourceView[iIndex])
		return E_FAIL;
	return pShader->Set_Resource(pName, m_vecShaderResourceView[iIndex]);
}
