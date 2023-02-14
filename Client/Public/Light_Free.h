#pragma once
#include "Light.h"

BEGIN_NAMESPACE(Client)
class CLight_Free final : public CLight {
public:
	static CLight_Free* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CLight_Free(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CLight_Free(const CLight_Free& rhs);
	virtual ~CLight_Free(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;

};
END_NAMESPACE

