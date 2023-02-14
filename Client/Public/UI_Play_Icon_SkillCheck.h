#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Icon_SkillCheck final : public CUI_Image
{
public:
	enum class EState { NONE, STOP, PLAY, FAIL, SUCCESS, GREAT, END };
	enum class ECheck { START, RUN, FAILED, END };
	const _char* EState_ToString(const EState& eType) {
		switch (eType)
		{
		case EState::NONE: return "NONE";
		case EState::STOP: return "STOP";
		case EState::PLAY: return "PLAY";
		case EState::FAIL: return "FAIL";
		case EState::SUCCESS: return "SUCCESS";
		}
		return nullptr;
	}
	enum class ETexType { BACK, HUD, POINT, DIV, END };
public:
	static CUI_Play_Icon_SkillCheck* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Icon_SkillCheck(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Icon_SkillCheck(const CUI_Play_Icon_SkillCheck& rhs);
	virtual ~CUI_Play_Icon_SkillCheck(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	const EState& Get_State() { return m_eState; }
	void Set_Off();
	void Set_HitCount(const _uint& iHitCnt, const _double& dTime, const ECheck& eCheck, const _float& fPerAng = 200.f);
private:
	void Reset_Info(const _bool& bActive);
	_float Clamp_Angle(const _float& fAng, const _float& fMin, const _float& fMax);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	_uint m_iHit = 0, m_iMaxHit = 0;
	_float m_fTime = 0.f, m_fMaxTime = 0.f;
	_double m_dHitTimes[30] = { 0.0, };
	_uint m_iHitTimes[30] = { 0 };

	_float m_fSpeed = 200.f;
	_float m_fPerAng = 0.f;
	_float m_fUnit = 0.f;

	EState m_eState = EState::NONE;
	EState m_ePreState = EState::NONE;
	_float m_fCurAngle = 0.f, m_fRanAngle = 0.f;
	_float2 m_vAngleRange = { 0.f, 0.f }; // normal, hit

	CTexture* m_pFXTexture[2] = { nullptr, };
	_bool m_bRun = false;
	_bool m_bFailed = false;
	//class CAudio* m_pAudio = nullptr;
};
END_NAMESPACE

