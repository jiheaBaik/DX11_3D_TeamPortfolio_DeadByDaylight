#include "Graphic.h"

IMPLEMENT_SINGLETON(CGraphic)

CGraphic::CGraphic(void)
	: CBase() {
}

void CGraphic::Delete(void) {
	m_pSwapChain->SetFullscreenState(false, NULL);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CGraphic::Init(const HWND& hWnd, const SGraphic& sGraphic) {
	_uint iFlags = 0;
#ifdef _DEBUG
	iFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	//±×·¡ÇÈ ÀåÄ¡ ÃÊ±âÈ­
	D3D_FEATURE_LEVEL Freature_Level;
	ZeroMemory(&Freature_Level, sizeof(D3D_FEATURE_LEVEL));
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlags, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &Freature_Level, &m_pContext)))
		return E_FAIL;

	//½º¿Ò Ã¼ÀÎ ÁØºñ
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDevice));
	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pAdapter));
	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory));
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferDesc.Width = sGraphic.iWidth;
	SwapChainDesc.BufferDesc.Height = sGraphic.iHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.Windowed = sGraphic.bWindowed;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChainDesc, &m_pSwapChain)))
		return E_FAIL;
	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	//·»´õ Å¸°Ù ºä ¼³Á¤
	ID3D11Texture2D* pRenderTargetTexture = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pRenderTargetTexture))))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateRenderTargetView(pRenderTargetTexture, nullptr, &m_pRenderTargetView)))
		return E_FAIL;
	Safe_Release(pRenderTargetTexture);

	//µª½º½ºÅÄ½Çºä ¼³Á¤
	ID3D11Texture2D* pDepthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC Texture2DDesc;
	ZeroMemory(&Texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2DDesc.Width = sGraphic.iWidth;
	Texture2DDesc.Height = sGraphic.iHeight;
	Texture2DDesc.MipLevels = 1;
	Texture2DDesc.ArraySize = 1;
	Texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Texture2DDesc.SampleDesc.Count = 1;
	Texture2DDesc.SampleDesc.Quality = 0;
	Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	Texture2DDesc.CPUAccessFlags = 0;
	Texture2DDesc.MiscFlags = 0;
	if (FAILED(m_pDevice->CreateTexture2D(&Texture2DDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	/* ÀåÄ¡¿¡ ¹ÙÀÎµåÇØ³õÀ» ·»´õÅ¸°Ùµé°ú µª½º½ºÅÙ½Çºä¸¦ ¼ÂÆÃÇÑ´Ù. */
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = static_cast<FLOAT>(sGraphic.iWidth);
	Viewport.Height = static_cast<FLOAT>(sGraphic.iHeight);
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	m_pContext->RSSetViewports(1, &Viewport);

	return S_OK;
}

const HRESULT CGraphic::Set_RenderTarget(void) {
	if (nullptr == m_pContext)
		return E_FAIL;
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	return S_OK;
}


const HRESULT CGraphic::Clear_RenderTargetView(const _float4& fColorRGBA) {
	if (nullptr == m_pContext)
		return E_FAIL;
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, (_float*)&fColorRGBA);
	return S_OK;
}

const HRESULT CGraphic::Clear_DepthStencilView(void) {
	if (nullptr == m_pContext)
		return E_FAIL;
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	return S_OK;
}

const HRESULT CGraphic::Present(void) {
	if (nullptr == m_pSwapChain)
		return E_FAIL;
	return m_pSwapChain->Present(0, 0);
}