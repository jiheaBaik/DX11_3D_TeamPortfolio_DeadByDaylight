#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CGraphic final : public CBase {
	DECLARE_SINGLETON(CGraphic)
public:
	struct SGraphic {
		_uint iWidth = 0;
		_uint iHeight = 0;
		BOOL bWindowed = FALSE;
	};
private:
	explicit CGraphic(void);
	virtual ~CGraphic(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const HWND& hWnd, const SGraphic& sGraphic);
public:
	ID3D11Device* const& Get_Device(void) const { return m_pDevice; }
	ID3D11DeviceContext* const& Get_Context(void) const { return m_pContext; }
	const HRESULT Set_RenderTarget(void);
	const HRESULT Clear_RenderTargetView(const _float4& fColorRGBA);
	const HRESULT Clear_DepthStencilView(void);
	const HRESULT Present(void);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;

	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
};
END_NAMESPACE