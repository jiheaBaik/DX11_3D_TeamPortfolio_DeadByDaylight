#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CSlasher_Trail final : public CObject {
public:
	static CSlasher_Trail* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CSlasher_Trail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CSlasher_Trail(const CSlasher_Trail& rhs);
	virtual ~CSlasher_Trail(void) override = default;
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
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CTexture* m_pTexture = nullptr;
	class CBuffer_Trail* m_pBuffer = nullptr;
	class CCollider* m_pCollider = nullptr;
};
END_NAMESPACE