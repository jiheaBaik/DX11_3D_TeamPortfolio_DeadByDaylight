#pragma once
#include "Camera.h"

BEGIN_NAMESPACE(Client)
class CCamera_Camper final : public CCamera {
public:
	enum class ECamState : _ubyte { INTRO, PLAY, HOOK_EVENT, ENDING_EXIT, MORI, END };
public:
	static CCamera_Camper* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCamera_Camper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCamera_Camper(const CCamera_Camper& rhs);
	virtual ~CCamera_Camper(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;

private:
	class CTransform* m_pTransform_Orbit = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CAudio* m_pAudio = nullptr;

	class CTransform* m_pTransform_Ending_Sour = nullptr;
	class CTransform* m_pTransform_Ending_Dest = nullptr;
public:
	_bool Get_IntroEnd() { return m_bisIntroFlag; }
	const _bool Get_AlmostFin() { return m_bAlmostFin; }
	void Set_Estate(ECamState CamState) { 
		eCamState = CamState;
		if (ECamState::ENDING_EXIT == eCamState) {
			m_pTransform_Ending_Sour->Set_Matrix(m_pTransform_Orbit->Get_Matrix());
		}
	}
	void Set_Fov(_float4x4 pro) { m_ProjFloat4x4 = pro; }

private:
	ECamState eCamState = ECamState::INTRO;
	_float m_fdecTime = 0.f;
	_bool m_bisIntroFlag = false;
	_bool m_bIntroBgm = true;

	class CNavi* m_pNavi = nullptr;
	_float m_fDist = 2.f;
	_bool m_bAlmostFin = false;

private:
	_float m_faddY = 0.f;
	_float m_fTimeEnding = 0.f;
};
END_NAMESPACE
