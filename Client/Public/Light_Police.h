#pragma once
#include "Light.h"

BEGIN_NAMESPACE(Client)
class CLight_Police final : public CLight {
public:
	static CLight_Police* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CLight_Police(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CLight_Police(const CLight_Police& rhs);
	virtual ~CLight_Police(void) override = default;
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
private:
	_bool m_bBlink = false;
	_double m_dBlinkTime = 0.;
};
END_NAMESPACE

