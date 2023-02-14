#pragma once
#include "Buffer.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CBuffer_Cube final : public CBuffer{
public:
	static CBuffer_Cube* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CBuffer_Cube(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CBuffer_Cube(const CBuffer_Cube& rhs);
	virtual ~CBuffer_Cube(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
};
END_NAMESPACE