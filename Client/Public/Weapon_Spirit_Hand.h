#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CWeapon_Spirit_Hand final : public CObject {
public:
	static CWeapon_Spirit_Hand* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CWeapon_Spirit_Hand(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CWeapon_Spirit_Hand(const CWeapon_Spirit_Hand& rhs);
	virtual ~CWeapon_Spirit_Hand(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CTransform* m_pTransform_FPV = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
	class CModel* m_pModel_FPV = nullptr;

	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CTransform* m_pSlasher_Transform = nullptr;
};
END_NAMESPACE