#include "Font_Mgr.h"
#include "Font.h"

IMPLEMENT_SINGLETON(CFont_Mgr)

CFont_Mgr::CFont_Mgr(void) 
	: CBase() {
}

void CFont_Mgr::Delete(void) {
	for (auto& pair : m_mapFont)
		Safe_Release(pair.second);
	m_mapFont.clear();
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CFont_Mgr::Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	return S_OK;
}

const HRESULT CFont_Mgr::Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fScale, const _float& fRotation) {
	CFont* pFont = Find_Font(pFontKey);
	if (nullptr == pFont)
		return E_FAIL;
	return pFont->Render(pText, vPos, vColor, fScale, fRotation);
}


const HRESULT CFont_Mgr::Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fBorder, _fvector vBorderColor, const _float& fRotation) {
	CFont* pFont = Find_Font(pFontKey);
	if (nullptr == pFont)
		return E_FAIL;
	return pFont->Render(pText, vPos, vColor, fBorder, vBorderColor, fRotation);
}

const HRESULT CFont_Mgr::Add_Font(const _tchar* const& pFontKey, const _tchar* const& pFilePath) {
	if (nullptr != Find_Font(pFontKey))
		return E_FAIL;
	CFont* pFont = CFont::Create(m_pDevice, m_pContext, pFilePath);
	if (nullptr == pFont)
		return E_FAIL;
	m_mapFont.emplace(pFontKey, pFont);
	return S_OK;
}

CFont* const CFont_Mgr::Find_Font(const _tchar* const& pFontKey) {
	auto pair = find_if(m_mapFont.begin(), m_mapFont.end(), CMapFinder(pFontKey));
	if (m_mapFont.end() == pair)
		return nullptr;
	return pair->second;
}
