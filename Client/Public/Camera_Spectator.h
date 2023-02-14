#pragma once
#include "Camera.h"

BEGIN_NAMESPACE(Client)
class CCamera_Spectator final : public CCamera {
public:
	static CCamera_Spectator* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCamera_Spectator(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCamera_Spectator(const CCamera_Spectator& rhs);
	virtual ~CCamera_Spectator(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Shift_Camper(const _bool& m_bLeftRight);
private:
	class CTransform* m_pTransform_Orbit = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CAudio* m_pAudio = nullptr;
	class CNavi* m_pNavi = nullptr;
private:
	_uint m_iCamper = 0;
	vector<class CObject*> m_vecCamper;
private:
	_float m_faddY = 0.f;
	_float m_fDist = 2.f;
};
END_NAMESPACE