#pragma once
#include "Light.h"

BEGIN_NAMESPACE(Client)
class CLight_Flash final : public CLight {
public:
	static CLight_Flash* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CLight_Flash(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CLight_Flash(const CLight_Flash& rhs);
	virtual ~CLight_Flash(void) override = default;
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
	void Set_Attack() {	m_bAttack = true; }
private:
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
private:
	_bool m_bBlink = false;
	_bool m_bAttack = false;
	_double m_dAttackTime = 0.;
};
END_NAMESPACE

