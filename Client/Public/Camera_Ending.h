#pragma once
#include "Camera.h"

BEGIN_NAMESPACE(Client)
class CCamera_Ending final : public CCamera {
public:
	static CCamera_Ending* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCamera_Ending(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCamera_Ending(const CCamera_Ending& rhs);
	virtual ~CCamera_Ending(void) override = default;
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
private:
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CAudio* m_pAudio = nullptr;
};
END_NAMESPACE
