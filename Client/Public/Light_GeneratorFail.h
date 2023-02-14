#pragma once
#include "Light.h"

BEGIN_NAMESPACE(Client)
class CLight_GeneratorFail final : public CLight {
public:
	static CLight_GeneratorFail* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CLight_GeneratorFail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CLight_GeneratorFail(const CLight_GeneratorFail& rhs);
	virtual ~CLight_GeneratorFail(void) override = default;
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
public:
	void Set_LightOnPos(_fvector pos, CLight::SDesc sDesc);
private:
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
private:
	_bool m_bBlink = false;
	_double m_dBlinkTime = 0.;
	_double m_dOffTime = 0.;
};
END_NAMESPACE

