#include "Target.h"
#include "Shader.h"
#include "Buffer_Rect.h"

CTarget* const CTarget::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, D3D11_BUFFER_DESC* const& pBuffer_Desc, D3D11_TEXTURE2D_DESC* const& pTexture2D_Desc, D3D11_SHADER_RESOURCE_VIEW_DESC* const& pShader_Resource_View_Desc, D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUnordered_Access_View_Desc, const _float4& vClearColor) {
	CTarget* pInstnace = new CTarget(pDevice, pContext);
	if (FAILED(pInstnace->Init(pBuffer_Desc, pTexture2D_Desc, pShader_Resource_View_Desc, pUnordered_Access_View_Desc, vClearColor)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CTarget::CTarget(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	ZeroMemory(&m_Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_Texture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	ZeroMemory(&m_Shader_Resource_View_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ZeroMemory(&m_Unordered_Access_View_Desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

}

void CTarget::Delete(void) {
	Safe_Release(m_pUnorderedAccessView);
	Safe_Release(m_pShaderResourceView);
	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CTarget::Init(D3D11_BUFFER_DESC* const& Buffer_Desc, D3D11_TEXTURE2D_DESC* const& pTexture2D_Desc, D3D11_SHADER_RESOURCE_VIEW_DESC* const& pShader_Resource_View_Desc, D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUnordered_Access_View_Desc, const _float4& vClearColor) {
	HRESULT hr = S_OK;
	if (nullptr != Buffer_Desc) {
		memcpy(&m_Buffer_Desc, Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
		if (FAILED(m_pDevice->CreateBuffer(&m_Buffer_Desc, nullptr, &m_pBuffer)))
			return E_FAIL;
		if (nullptr != pShader_Resource_View_Desc)
			memcpy(&m_Shader_Resource_View_Desc, pShader_Resource_View_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		if (m_Buffer_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
			hr |= m_pDevice->CreateShaderResourceView(m_pBuffer, pShader_Resource_View_Desc, &m_pShaderResourceView);

		if (nullptr != pUnordered_Access_View_Desc)
			memcpy(&m_Unordered_Access_View_Desc, pUnordered_Access_View_Desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		if (m_Buffer_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			hr |= m_pDevice->CreateUnorderedAccessView(m_pBuffer, pUnordered_Access_View_Desc, &m_pUnorderedAccessView);
	}
	else if (nullptr != pTexture2D_Desc) {
		memcpy(&m_Texture2D_Desc, pTexture2D_Desc, sizeof(D3D11_TEXTURE2D_DESC));
		if (FAILED(m_pDevice->CreateTexture2D(&m_Texture2D_Desc, nullptr, &m_pTexture)))
			return E_FAIL;
		if (m_Texture2D_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
			hr |= m_pDevice->CreateRenderTargetView(m_pTexture, nullptr, &m_pRenderTargetView);

		if (nullptr != pShader_Resource_View_Desc)
			memcpy(&m_Shader_Resource_View_Desc, pShader_Resource_View_Desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		if (m_Texture2D_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
			hr |= m_pDevice->CreateShaderResourceView(m_pTexture, pShader_Resource_View_Desc, &m_pShaderResourceView);

		if (nullptr != pUnordered_Access_View_Desc)
			memcpy(&m_Unordered_Access_View_Desc, pUnordered_Access_View_Desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		if (m_Texture2D_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			hr |= m_pDevice->CreateUnorderedAccessView(m_pTexture, pUnordered_Access_View_Desc, &m_pUnorderedAccessView);
	}
	m_vClearColor = vClearColor;
	return hr;
}

const HRESULT CTarget::Clear_RenderTargetView(void) {
	if (nullptr == m_pContext)
		return E_FAIL;
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, reinterpret_cast<_float*>(&m_vClearColor));
	return S_OK;
}

ID3D11RenderTargetView* const& CTarget::Get_RenderTargetView(void) const {
	return m_pRenderTargetView;
}

const HRESULT CTarget::Set_Resource(CShader* const& pShader, const char* const& pName) {
	if (nullptr == pShader || nullptr == m_pShaderResourceView)
		return E_FAIL;
	return pShader->Set_Resource(pName, m_pShaderResourceView);
}

ID3D11Buffer* const& CTarget::Get_Buffer(void) {
	return m_pBuffer;
}

ID3D11Texture2D* const& CTarget::Get_Texture(void) {
	return m_pTexture;
}

ID3D11RenderTargetView* const& CTarget::Get_RenderTargetView(void) {
	return m_pRenderTargetView;
}

ID3D11ShaderResourceView* const& CTarget::Get_ShaderResourceView(void) {
	return m_pShaderResourceView;
}

ID3D11UnorderedAccessView* const& CTarget::Get_UnorderedAccessView(void) {
	return m_pUnorderedAccessView;
}

const D3D11_TEXTURE2D_DESC& CTarget::Get_Texture2d_Desc(void) {
	return m_Texture2D_Desc;
}

const D3D11_BUFFER_DESC& CTarget::Get_Buffer_Desc(void) {
	return m_Buffer_Desc;
}

const D3D11_SHADER_RESOURCE_VIEW_DESC& CTarget::Get_Shader_Resource_View_Desc(void) {
	return m_Shader_Resource_View_Desc;
}

const D3D11_UNORDERED_ACCESS_VIEW_DESC& CTarget::Get_Unordered_Access_View_Desc(void) {
	return m_Unordered_Access_View_Desc;
}
