#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CShader final : public CComponent{
public:
	struct SDesc {
		ID3DX11EffectPass* pPass = nullptr;
		ID3D11InputLayout* pInputLayout = nullptr;
	};
public:
	static CShader* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath);
	static CShader* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath, D3D11_INPUT_ELEMENT_DESC* const& pInput_Element_Desc, const _uint& iNumElement);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CShader(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CShader(const CShader& rhs);
	virtual ~CShader(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const _tchar* const& pFileName);
	const HRESULT Init_Create(const _tchar* const& pFileName, D3D11_INPUT_ELEMENT_DESC* const& pInput_Element_Desc, const _uint & iNumElement);
	const HRESULT Init_Clone(void* const& pArg);
public:
	const HRESULT Set_RawValue(const char* const& pName, const void* const& pData,const _uint& iByteCount);
	const HRESULT Set_Resource(const char* const& pName, ID3D11ShaderResourceView* const& pShaderResourceView);
	const HRESULT Set_Resource(const char* const& pName, ID3D11UnorderedAccessView* const& pUnorderedAccessView);
	const HRESULT Apply(const _uint & iDesc);
private:
	ID3DX11Effect* m_pEffect = nullptr;
	vector<SDesc> m_vecsDesc;
};
END_NAMESPACE

