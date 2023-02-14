#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CEffect abstract : public CObject{
public:
	virtual CObject* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CEffect(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEffect(const CEffect& rhs);
	virtual ~CEffect(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const _ubyte Update(const _double & dTimeDelta) PURE;
	virtual void Late_Update(const _double & dTimeDelta) PURE;
	virtual const HRESULT Render(const CRenderer::EOrder & eOrder) PURE;
public:
	class CTransform* Get_EffectTransform();
protected:
	class CTransform* m_pEffect_Transform = nullptr;
};
END_NAMESPACE