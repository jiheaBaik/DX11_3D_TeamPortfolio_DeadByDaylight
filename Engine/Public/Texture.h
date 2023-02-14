#pragma once
#include "Component.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CTexture final : public CComponent {
public:
	static CTexture* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFileName, const _uint& iNumTexture);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CTexture(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const _tchar* const& pFileName, const _uint& iTexture);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
public:
	const HRESULT Set_Resource(class CShader* const& pShader, const _char* const& pName, const _uint& iIndex);
public:
	vector<ID3D11ShaderResourceView*> Get_ResourceView() { return m_vecShaderResourceView; }
private:
	vector<ID3D11ShaderResourceView*> m_vecShaderResourceView;
};
END_NAMESPACE