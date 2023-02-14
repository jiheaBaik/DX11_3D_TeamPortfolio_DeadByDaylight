#pragma once
#include "Buffer.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CBuffer_Trail final : public CBuffer{
public:
	static CBuffer_Trail* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CBuffer_Trail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CBuffer_Trail(const CBuffer_Trail& rhs);
	virtual ~CBuffer_Trail(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	const HRESULT Render1(CShader* const& pShader, const _uint & iPass);
public:
	void Set_Trail(_float3 vPos, _float3 vPos2);
private:
	_int m_iIndex = 0;
	_int m_iVertexCount = 0;
};
END_NAMESPACE