#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CFrustum final : public CBase {
	DECLARE_SINGLETON(CFrustum);
private:
	explicit CFrustum(void);
	virtual ~CFrustum(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
public:
	void Transform_ToWorldSpace(void);
	const _bool IsIn_Frustum_World(_fvector vPoint, const _float& fRange = 0.f);
private:
	_float3 m_vOrigin_Point[8];
	_float3 m_vWorld_Point[8];

	_float4 m_vPlaneInWorld[6];
};
END_NAMESPACE