#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CFont final : public CBase {
public:
	static CFont* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath);
private:
	explicit CFont(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CFont(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const _tchar* const& pFilePath);
	const HRESULT Render(const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fScale = 1.f, const _float& fRotation = 0.f);
	const HRESULT Render(const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fBorder, _fvector vBorderColor, const _float& fRotation = 0.f);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	SpriteFont* m_pSpriteFont = nullptr;
	SpriteBatch* m_pSpriteBatch = nullptr;
};
END_NAMESPACE
