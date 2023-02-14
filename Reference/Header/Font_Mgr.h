#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CFont_Mgr  final : public CBase {
	DECLARE_SINGLETON(CFont_Mgr)
private:
	explicit CFont_Mgr(void);
	virtual ~CFont_Mgr(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	const HRESULT Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fScale = 1.f, const _float& fRotation = 0.f);
	const HRESULT Render_Font(const _tchar* const& pFontKey, const _tchar* const& pText, const _float2& vPos, _fvector vColor, const _float& fBorder, _fvector vBorderColor, const _float& fRotation = 0.f);
public:
	const HRESULT Add_Font(const _tchar* const& pFontKey, const _tchar* const& pFilePath);
private:
	class CFont* const Find_Font(const _tchar* const& pFontKey);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	map<const _tchar*, class CFont*> m_mapFont;
};
END_NAMESPACE