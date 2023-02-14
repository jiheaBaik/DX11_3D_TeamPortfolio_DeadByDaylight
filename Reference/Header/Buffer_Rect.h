#pragma once
#include "Buffer.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CBuffer_Rect final : public CBuffer{
public:
	static CBuffer_Rect* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual CComponent* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CBuffer_Rect(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CBuffer_Rect(const CBuffer_Rect& rhs);
	virtual ~CBuffer_Rect(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
};
END_NAMESPACE