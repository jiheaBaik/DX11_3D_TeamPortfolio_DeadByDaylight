#pragma once
#include "Light.h"

BEGIN_NAMESPACE(Client)
class CLight_Slasher_Eye final : public CLight {
public:
	static CLight_Slasher_Eye* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CLight_Slasher_Eye(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CLight_Slasher_Eye(const CLight_Slasher_Eye& rhs);
	virtual ~CLight_Slasher_Eye(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	class CTransform* m_pTransform_Eye = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
};
END_NAMESPACE

