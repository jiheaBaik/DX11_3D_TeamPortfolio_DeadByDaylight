#include "Font.h"

CFont* const CFont::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath) {
	CFont* pInstnace = new CFont(pDevice, pContext);
	if (FAILED(pInstnace->Init(pFilePath)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CFont::CFont(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) 
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CFont::Delete(void) {
	Safe_Delete(m_pSpriteBatch);
	Safe_Delete(m_pSpriteFont);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CFont::Init(const _tchar* const& pFilePath) {
	m_pSpriteFont = new SpriteFont(m_pDevice, pFilePath);
	m_pSpriteBatch = new SpriteBatch(m_pContext);
	return S_OK;
}

const HRESULT CFont::Render(const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fScale, const _float& fRotation)
{
	if (nullptr == m_pSpriteFont || nullptr == m_pSpriteBatch)
		return E_FAIL;
	m_pContext->GSSetShader(nullptr, nullptr, 0);
	m_pSpriteBatch->Begin();
	m_pSpriteFont->DrawString(m_pSpriteBatch, pText, vPos, vColor, fRotation, _float2(0.f, 0.f), fScale);
	m_pSpriteBatch->End();
	return S_OK;
}

const HRESULT CFont::Render(const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fBorder, _fvector vBorderColor, const _float& fRotation) {
	_float2 vBorderPos = vPos;
	vBorderPos.x += fBorder;
	Render(pText, vBorderPos, vBorderColor, fRotation);
	vBorderPos = vPos;
	vBorderPos.x -= fBorder;
	Render(pText, vBorderPos, vBorderColor, fRotation);
	vBorderPos = vPos;
	vBorderPos.y += fBorder;
	Render(pText, vBorderPos, vBorderColor, fRotation);
	vBorderPos = vPos;
	vBorderPos.y -= fBorder;
	Render(pText, vBorderPos, vBorderColor, fRotation);

	Render(pText, vPos, vColor, fRotation);
	return S_OK;
}
