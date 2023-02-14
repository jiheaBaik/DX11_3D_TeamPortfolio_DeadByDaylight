#pragma once
#include "Buffer.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CTriangle final : public CBuffer{
public:
	static CTriangle* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _float3* const& pPoint);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CTriangle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CTriangle(const CTriangle& rhs);
	virtual ~CTriangle(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const _float3* const& pPoint);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
public:
	void Set_Vertex(const _float3* const& vPoint);
};
END_NAMESPACE