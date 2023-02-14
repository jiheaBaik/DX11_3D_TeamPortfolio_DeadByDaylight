#pragma once
#include "Camera.h"

BEGIN_NAMESPACE(Client)
class CCamera_Slasher final : public CCamera {
public:
	enum class ECamState {
		INTRO, PLAY, ENDING, END
	};
public:
	static CCamera_Slasher* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCamera_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCamera_Slasher(const CCamera_Slasher& rhs);
	virtual ~CCamera_Slasher(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	class CTransform* m_pTransform_Intro = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CAudio* m_pAudio = nullptr;
public:
	_bool Get_IntroEnd() { return m_bisIntroFlag; }
	const _bool Get_AlmostFin() { return m_bAlmostFin; }

	const ECamState& Get_CamState(void);
private:
	ECamState m_eCamState = ECamState::INTRO;

	_float m_fdecTime = 0.f;
	_bool m_bisIntroFlag = false;
	_bool m_bIntroBGM = false;
	_bool m_bPreSet = false;
	_float3 m_vPreLook;
	_float3 m_vPrePos;

	_bool m_bAlmostFin = false;
	_double m_dIntroTime = 0.;
};
END_NAMESPACE
