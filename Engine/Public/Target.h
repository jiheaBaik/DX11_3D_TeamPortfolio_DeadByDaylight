#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CTarget final : public CBase{
public:
	static CTarget* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext,
		D3D11_BUFFER_DESC* const& pBuffer_Desc,
		D3D11_TEXTURE2D_DESC* const& pTexture2D_Desc,
		D3D11_SHADER_RESOURCE_VIEW_DESC* const& pShader_Resource_View_Desc,
		D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUnordered_Access_View_Desc,
		const _float4 & vClearColor);
private:
	explicit CTarget(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CTarget(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(
		D3D11_BUFFER_DESC* const& pBuffer_Desc,
		D3D11_TEXTURE2D_DESC* const& pTexture2D_Desc,
		D3D11_SHADER_RESOURCE_VIEW_DESC* const& pShader_Resource_View_Desc,
		D3D11_UNORDERED_ACCESS_VIEW_DESC* const& pUnordered_Access_View_Desc,
		const _float4 & vClearColor);
public:
	const HRESULT Clear_RenderTargetView(void);
	ID3D11RenderTargetView* const& Get_RenderTargetView(void) const;
	const HRESULT Set_Resource(class CShader* const& pShader, const char* const& pName);

	ID3D11Buffer* const& Get_Buffer(void);
	ID3D11Texture2D* const& Get_Texture(void);
	ID3D11RenderTargetView* const& Get_RenderTargetView(void);
	ID3D11ShaderResourceView* const& Get_ShaderResourceView(void);
	ID3D11UnorderedAccessView* const& Get_UnorderedAccessView(void);
	const D3D11_BUFFER_DESC& Get_Buffer_Desc(void);
	const D3D11_TEXTURE2D_DESC& Get_Texture2d_Desc(void);
	const D3D11_SHADER_RESOURCE_VIEW_DESC& Get_Shader_Resource_View_Desc(void);
	const D3D11_UNORDERED_ACCESS_VIEW_DESC& Get_Unordered_Access_View_Desc(void);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	ID3D11Buffer* m_pBuffer = nullptr;
	ID3D11Texture2D* m_pTexture = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
	ID3D11UnorderedAccessView* m_pUnorderedAccessView = nullptr;

	D3D11_TEXTURE2D_DESC m_Texture2D_Desc;
	D3D11_BUFFER_DESC m_Buffer_Desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_Shader_Resource_View_Desc;
	D3D11_UNORDERED_ACCESS_VIEW_DESC m_Unordered_Access_View_Desc;
private:
	_float4 m_vClearColor{ 0.f, 0.f, 0.f, 0.f };
};
END_NAMESPACE

