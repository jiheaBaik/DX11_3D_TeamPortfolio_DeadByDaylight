#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CSetting_CamperSlot final : public CObject {
public:
	static CSetting_CamperSlot* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CSetting_CamperSlot(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CSetting_CamperSlot(const CSetting_CamperSlot& rhs);
	virtual ~CSetting_CamperSlot(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
public:
	void Set_EffectRender(const _bool bRender);
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;

	class CEffect_Camper_Lobby* m_pEffect[4]{nullptr};
private:
	_bool m_bRender = true;
};
END_NAMESPACE