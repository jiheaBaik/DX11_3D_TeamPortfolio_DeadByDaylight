#include "Target_Mgr.h"
#include "Target.h"
#include "Buffer_Rect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CTarget_Mgr)

CTarget_Mgr::CTarget_Mgr(void)
	: CBase() {
}

void CTarget_Mgr::Delete(void) {
	for (auto& pair : m_mapMultiTarget) {
		for (auto& iter : pair.second)
			Safe_Release(iter);
		pair.second.clear();
	}
	m_mapMultiTarget.clear();

	for (auto& pair : m_mapTarget)
		Safe_Release(pair.second);
	m_mapTarget.clear();

	for (auto& pair : m_mapDepthStencilView)
		Safe_Release(pair.second);
	m_mapDepthStencilView.clear();

	m_mapViewport.clear();

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CTarget_Mgr::Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	D3D11_VIEWPORT Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
	_uint iNumViewport = 1;
	pContext->RSGetViewports(&iNumViewport, &Viewport);
	m_Viewport = Viewport;

	D3D11_BUFFER_DESC Buffer_Desc;
	D3D11_TEXTURE2D_DESC Texture2D_Desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC Shader_Resource_View_Desc;
	D3D11_UNORDERED_ACCESS_VIEW_DESC Unordered_Access_View_Desc;

	HRESULT hr = S_OK;

	//Back
	hr |= Add_DepthStencilView(TEXT("DepthStencilView_Back"), static_cast<_uint>(Viewport.Width), static_cast<_uint>(Viewport.Height));
	hr |= Add_Viewport(TEXT("Viewport_Back"), static_cast<_uint>(Viewport.Width), static_cast<_uint>(Viewport.Height));
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Back0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Back1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Deferred
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Diffuse"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Normal"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("World"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Depth"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Material"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//LightAcc
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shade"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Specular"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Shadow
	hr |= Add_DepthStencilView(TEXT("DepthStencilView_Shadow"), 8000, 6000);
	hr |= Add_Viewport(TEXT("Viewport_Shadow"), 8000, 6000);
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 8000; Texture2D_Desc.Height = 6000;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	//Shadow_Bake
	hr |= Add_DepthStencilView(TEXT("DepthStencilView_Shadow_Bake"), 8000, 6000);
	hr |= Add_Viewport(TEXT("Viewport_Shadow_Bake"), 8000, 6000);
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 8000; Texture2D_Desc.Height = 6000;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow_Bake"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	//Glow
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow_Buffer"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Distortion
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Distortion"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//SSAO
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("SSAO"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("GodRay"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("GodRay0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Penetrate"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Compute
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Blend"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow2"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("SSAO0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("SSAO1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow2"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow_Buffer0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow_Buffer1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Distortion0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("DOF0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4.f); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("DOF1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("DOF2"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Buffer_Desc.StructureByteStride = sizeof(float);
	Buffer_Desc.ByteWidth = static_cast<_uint>(Viewport.Width) * Buffer_Desc.StructureByteStride;
	Buffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ZeroMemory(&Shader_Resource_View_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	Shader_Resource_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Shader_Resource_View_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	Shader_Resource_View_Desc.Buffer.NumElements = static_cast<_uint>(Viewport.Width);
	ZeroMemory(&Unordered_Access_View_Desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	Unordered_Access_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Unordered_Access_View_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	Unordered_Access_View_Desc.Buffer.NumElements = static_cast<_uint>(Viewport.Width);
	hr |= Add_Target(TEXT("HDR0"), CTarget::Create(m_pDevice, m_pContext, &Buffer_Desc, nullptr, &Shader_Resource_View_Desc, &Unordered_Access_View_Desc, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Buffer_Desc.StructureByteStride = sizeof(float);
	Buffer_Desc.ByteWidth = sizeof(_float);
	Buffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ZeroMemory(&Shader_Resource_View_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	Shader_Resource_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Shader_Resource_View_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	Shader_Resource_View_Desc.Buffer.NumElements = 1;
	ZeroMemory(&Unordered_Access_View_Desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	Unordered_Access_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Unordered_Access_View_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	Unordered_Access_View_Desc.Buffer.NumElements = 1;
	hr |= Add_Target(TEXT("HDR1"), CTarget::Create(m_pDevice, m_pContext, &Buffer_Desc, nullptr, &Shader_Resource_View_Desc, &Unordered_Access_View_Desc, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Bloom0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Bloom1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width / 4); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height / 4);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Bloom2"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("MotionBlur"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Fog0"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Fog1"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("LumaSharpen"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(Viewport.Width); Texture2D_Desc.Height = static_cast<_uint>(Viewport.Height);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("HSBC"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//MultTarget
	hr |= Add_MultiTarget(TEXT("Back0"), TEXT("Back0"));
	hr |= Add_MultiTarget(TEXT("Back1"), TEXT("Back1"));
	hr |= Add_MultiTarget(TEXT("Deferred"), TEXT("Diffuse"));
	hr |= Add_MultiTarget(TEXT("Deferred"), TEXT("Normal"));
	hr |= Add_MultiTarget(TEXT("Deferred"), TEXT("World"));
	hr |= Add_MultiTarget(TEXT("Deferred"), TEXT("Depth"));
	hr |= Add_MultiTarget(TEXT("Deferred"), TEXT("Material"));
	hr |= Add_MultiTarget(TEXT("Deferred2"), TEXT("Diffuse"));
	hr |= Add_MultiTarget(TEXT("Deferred3"), TEXT("Back0"));
	hr |= Add_MultiTarget(TEXT("Deferred3"), TEXT("World"));
	hr |= Add_MultiTarget(TEXT("Deferred3"), TEXT("Depth"));
	hr |= Add_MultiTarget(TEXT("Fog1"), TEXT("Fog1"));
	hr |= Add_MultiTarget(TEXT("LightAcc"), TEXT("Shade"));
	hr |= Add_MultiTarget(TEXT("LightAcc"), TEXT("Specular"));
	hr |= Add_MultiTarget(TEXT("Shadow"), TEXT("Shadow"));
	hr |= Add_MultiTarget(TEXT("Shadow_Bake"), TEXT("Shadow_Bake"));
	hr |= Add_MultiTarget(TEXT("Glow"), TEXT("Glow"));
	hr |= Add_MultiTarget(TEXT("Glow"), TEXT("Glow_Buffer"));
	hr |= Add_MultiTarget(TEXT("Distortion"), TEXT("Distortion"));
	hr |= Add_MultiTarget(TEXT("SSAO"), TEXT("SSAO"));
	hr |= Add_MultiTarget(TEXT("GodRay"), TEXT("GodRay"));
	hr |= Add_MultiTarget(TEXT("GodRay0"), TEXT("GodRay0"));
	hr |= Add_MultiTarget(TEXT("Penetrate"), TEXT("Penetrate"));

	//Back
	hr |= Add_DepthStencilView(TEXT("DepthStencilView_Back_Enviro"), 512, 512);
	hr |= Add_Viewport(TEXT("Viewport_Back_Enviro"), 512, 512);
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Back0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Back1_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Deferred
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Diffuse_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Normal_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("World_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Depth_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Material_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//LightAcc
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shade_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Specular_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Shadow
	hr |= Add_DepthStencilView(TEXT("DepthStencilView_Shadow_Enviro"), 8000, 6000);
	hr |= Add_Viewport(TEXT("Viewport_Shadow_Enviro"), 8000, 6000);
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 8000; Texture2D_Desc.Height = 6000;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	//Shadow_Bake
	hr |= Add_DepthStencilView(TEXT("DepthStencilView_Shadow_Bake_Enviro"), 8000, 6000);
	hr |= Add_Viewport(TEXT("Viewport_Shadow_Bake_Enviro"), 8000, 6000);
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 8000; Texture2D_Desc.Height = 6000;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow_Bake_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	//Glow
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Distortion
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Distortion_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//SSAO
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("SSAO_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("GodRay_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("GodRay0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Penetrate_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//Compute
	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Blend_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 1.f, 1.f, 1.f, 1.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow1_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Shadow2_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("SSAO0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("SSAO1_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow1_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Glow2_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Distortion0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("DOF0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4.f); Texture2D_Desc.Height = static_cast<_uint>(512 / 4.f);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("DOF1_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("DOF2_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Buffer_Desc.StructureByteStride = sizeof(float);
	Buffer_Desc.ByteWidth = 512 * Buffer_Desc.StructureByteStride;
	Buffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ZeroMemory(&Shader_Resource_View_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	Shader_Resource_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Shader_Resource_View_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	Shader_Resource_View_Desc.Buffer.NumElements = 512;
	ZeroMemory(&Unordered_Access_View_Desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	Unordered_Access_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Unordered_Access_View_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	Unordered_Access_View_Desc.Buffer.NumElements = 512;
	hr |= Add_Target(TEXT("HDR0_Enviro"), CTarget::Create(m_pDevice, m_pContext, &Buffer_Desc, nullptr, &Shader_Resource_View_Desc, &Unordered_Access_View_Desc, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Buffer_Desc.StructureByteStride = sizeof(float);
	Buffer_Desc.ByteWidth = sizeof(_float);
	Buffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ZeroMemory(&Shader_Resource_View_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	Shader_Resource_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Shader_Resource_View_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	Shader_Resource_View_Desc.Buffer.NumElements = 1;
	ZeroMemory(&Unordered_Access_View_Desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	Unordered_Access_View_Desc.Format = DXGI_FORMAT_UNKNOWN;
	Unordered_Access_View_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	Unordered_Access_View_Desc.Buffer.NumElements = 1;
	hr |= Add_Target(TEXT("HDR1_Enviro"), CTarget::Create(m_pDevice, m_pContext, &Buffer_Desc, nullptr, &Shader_Resource_View_Desc, &Unordered_Access_View_Desc, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Bloom0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4); Texture2D_Desc.Height = static_cast<_uint>(512 / 4);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Bloom1_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = static_cast<_uint>(512 / 4); Texture2D_Desc.Height = static_cast<_uint>(512 / 4);
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Bloom2_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("MotionBlur_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Fog0_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	ZeroMemory(&Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2D_Desc.Width = 512; Texture2D_Desc.Height = 512;
	Texture2D_Desc.MipLevels = 1; Texture2D_Desc.ArraySize = 1; Texture2D_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Texture2D_Desc.SampleDesc.Count = 1; Texture2D_Desc.SampleDesc.Quality = 0; Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Texture2D_Desc.CPUAccessFlags = 0; Texture2D_Desc.MiscFlags = 0;
	hr |= Add_Target(TEXT("Fog1_Enviro"), CTarget::Create(m_pDevice, m_pContext, nullptr, &Texture2D_Desc, nullptr, nullptr, _float4{ 0.f, 0.f, 0.f, 0.f }));

	//MultTarget
	hr |= Add_MultiTarget(TEXT("Back0_Enviro"), TEXT("Back0_Enviro"));
	hr |= Add_MultiTarget(TEXT("Back1_Enviro"), TEXT("Back1_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred_Enviro"), TEXT("Diffuse_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred_Enviro"), TEXT("Normal_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred_Enviro"), TEXT("World_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred_Enviro"), TEXT("Depth_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred_Enviro"), TEXT("Material_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred2_Enviro"), TEXT("Diffuse_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred3_Enviro"), TEXT("Back0_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred3_Enviro"), TEXT("World_Enviro"));
	hr |= Add_MultiTarget(TEXT("Deferred3_Enviro"), TEXT("Depth_Enviro"));
	hr |= Add_MultiTarget(TEXT("Fog1_Enviro"), TEXT("Fog1_Enviro"));
	hr |= Add_MultiTarget(TEXT("LightAcc_Enviro"), TEXT("Shade_Enviro"));
	hr |= Add_MultiTarget(TEXT("LightAcc_Enviro"), TEXT("Specular_Enviro"));
	hr |= Add_MultiTarget(TEXT("Shadow_Enviro"), TEXT("Shadow_Enviro"));
	hr |= Add_MultiTarget(TEXT("Shadow_Bake_Enviro"), TEXT("Shadow_Bake_Enviro"));
	hr |= Add_MultiTarget(TEXT("Glow_Enviro"), TEXT("Glow_Enviro"));
	hr |= Add_MultiTarget(TEXT("Distortion_Enviro"), TEXT("Distortion_Enviro"));
	hr |= Add_MultiTarget(TEXT("SSAO_Enviro"), TEXT("SSAO_Enviro"));
	hr |= Add_MultiTarget(TEXT("GodRay_Enviro"), TEXT("GodRay_Enviro"));
	hr |= Add_MultiTarget(TEXT("GodRay0_Enviro"), TEXT("GodRay0_Enviro"));
	hr |= Add_MultiTarget(TEXT("Penetrate_Enviro"), TEXT("Penetrate_Enviro"));

	return S_OK;
}

CTarget* const CTarget_Mgr::Get_Target(const _tchar* const& pTargetKey) {
	CTarget* pTarget = Find_Target(pTargetKey);
	return pTarget;
}

map<wstring, class CTarget*>& CTarget_Mgr::Get_MapTarget(void) {
	return m_mapTarget;
}

const HRESULT CTarget_Mgr::Add_Target(const _tchar* const& pTargetKey, CTarget* const& pTarget) {
	if (nullptr != Find_Target(pTargetKey))
		return E_FAIL;
	m_mapTarget.emplace(pTargetKey, pTarget);
	return S_OK;
}

const HRESULT CTarget_Mgr::Add_DepthStencilView(const _tchar* const& pDepthStancilViewKey, const _uint& iWidth, const _uint& iHeight) {
	ID3D11DepthStencilView* pDepthStancilView;

	ID3D11Texture2D* pTexture = nullptr;
	D3D11_TEXTURE2D_DESC Texture2DDesc;
	ZeroMemory(&Texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture2DDesc.Width = iWidth;
	Texture2DDesc.Height = iHeight;
	Texture2DDesc.MipLevels = 1;
	Texture2DDesc.ArraySize = 1;
	Texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Texture2DDesc.SampleDesc.Quality = 0;
	Texture2DDesc.SampleDesc.Count = 1;
	Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	Texture2DDesc.CPUAccessFlags = 0;
	Texture2DDesc.MiscFlags = 0;
	if (FAILED(m_pDevice->CreateTexture2D(&Texture2DDesc, nullptr, &pTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pTexture, nullptr, &pDepthStancilView)))
		return E_FAIL;
	Safe_Release(pTexture);

	m_mapDepthStencilView.emplace(pDepthStancilViewKey, pDepthStancilView);
	return S_OK;
}

const HRESULT CTarget_Mgr::Add_Viewport(const _tchar* const& pViewportKey, const _uint& iWidth, const _uint& iHeight) {
	D3D11_VIEWPORT Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = static_cast<FLOAT>(iWidth);
	Viewport.Height = static_cast<FLOAT>(iHeight);
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;

	m_mapViewport.emplace(pViewportKey, Viewport);
	return S_OK;
}

const HRESULT CTarget_Mgr::Add_MultiTarget(const _tchar* const& pMultiKey, const _tchar* const& pTargetKey) {
	CTarget* pTarget = Find_Target(pTargetKey);
	if (nullptr == pTarget)
		return E_FAIL;

	list<CTarget*>* pMultiTarget = Find_MultiTarget(pMultiKey);
	if (nullptr == pMultiTarget) {
		list<CTarget*> MultiTarget;
		MultiTarget.push_back(pTarget);
		m_mapMultiTarget.emplace(pMultiKey, MultiTarget);
	}
	else
		pMultiTarget->push_back(pTarget);
	Safe_AddRef(pTarget);
	return S_OK;
}

const HRESULT CTarget_Mgr::Clear_DepthStencilView(const _tchar* pDepthStencilViewKey) {
	ID3D11DepthStencilView* pDepthStencilView = Find_DepthStencilView(pDepthStencilViewKey);
	if (nullptr == pDepthStencilView)
		return E_FAIL;
	m_pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	return S_OK;
}

const HRESULT CTarget_Mgr::Begin_MultiTarget(const _tchar* const& pMultiKey, const _tchar* const& pDepthStancilViewKey, const _tchar* const& pViewportKey, const _bool& bClear_RenderTargetView) {
	list<CTarget*>* pMultiTarget = Find_MultiTarget(pMultiKey);
	if (nullptr == pMultiTarget)
		return E_FAIL;
	ID3D11DepthStencilView* pDepthStencilView = Find_DepthStencilView(pDepthStancilViewKey);
	if (nullptr == pDepthStancilViewKey)
		return E_FAIL;
	D3D11_VIEWPORT* pViewport = Find_Viewport(pViewportKey);
	if (nullptr == pViewport)
		return E_FAIL;

	ID3D11RenderTargetView* pRenderTargetView[8] = { nullptr };
	_uint iNumView = 0;
	for (auto& iter : *pMultiTarget) {
		if (true == bClear_RenderTargetView)
			iter->Clear_RenderTargetView();
		pRenderTargetView[iNumView++] = iter->Get_RenderTargetView();
	}
	if (0 <= iNumView && 8 >= iNumView)
		m_pContext->OMSetRenderTargets(iNumView, pRenderTargetView, pDepthStencilView);

	m_pContext->RSSetViewports(1, pViewport);
	return S_OK;
}

const HRESULT CTarget_Mgr::End_MultiTarget(void) {
	ID3D11ShaderResourceView* pShaderResourceView[8] = { nullptr };
	m_pContext->PSSetShaderResources(0, 8, pShaderResourceView);
	ID3D11RenderTargetView* pRenderTargetView[8] = { nullptr };
	m_pContext->OMSetRenderTargets(8, pRenderTargetView, m_pDepthStencilView);

	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	m_pContext->RSSetViewports(1, &m_Viewport);
	return S_OK;
}

CTarget* const CTarget_Mgr::Find_Target(const _tchar* const& pTargetKey) {
	auto iter = m_mapTarget.find(pTargetKey);
	//auto iter = find_if(m_mapTarget.begin(), m_mapTarget.end(), [&](map<wstring, class CTarget*>::value_type sour) {
	//	return sour.first == pTargetKey;
	//	});
	if (m_mapTarget.end() == iter)
		return nullptr;
	return iter->second;
}

list<class CTarget*>* CTarget_Mgr::Find_MultiTarget(const _tchar* const& pMultiKey) {
	auto iter = m_mapMultiTarget.find(pMultiKey);
	//auto iter = find_if(m_mapMultiTarget.begin(), m_mapMultiTarget.end(), CMapFinder(pMultiKey));
	if (m_mapMultiTarget.end() == iter)
		return nullptr;
	return &iter->second;
}

ID3D11DepthStencilView* const CTarget_Mgr::Find_DepthStencilView(const _tchar* const& pTargetKey) {
	auto iter = m_mapDepthStencilView.find(pTargetKey);
	//auto iter = find_if(m_mapDepthStencilView.begin(), m_mapDepthStencilView.end(), CMapFinder(pTargetKey));
	if (m_mapDepthStencilView.end() == iter)
		return nullptr;
	return iter->second;
}

D3D11_VIEWPORT* const CTarget_Mgr::Find_Viewport(const _tchar* const& pTargetKey) {
	auto iter = m_mapViewport.find(pTargetKey);
	//auto iter = find_if(m_mapViewport.begin(), m_mapViewport.end(), CMapFinder(pTargetKey));
	if (m_mapViewport.end() == iter)
		return nullptr;
	return &iter->second;
}