#pragma once
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Icon_Propile.h"
#include "UI_Play_Icon_SkillCheck.h"
#include "UI_Play_Gauge_Exit.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Icon_Passive.h"
#include "UI_Play_Icon_Perk.h"
#include "UI_Play_Icon_Propile.h"
#include "UI_Play_Score.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Panel_Second final : public CUI_Image
{
public:
	enum class ETexType { DEVIDE, GEN, HATCH, EXIT, FONT, END };
public:
	static CUI_Play_Panel_Second* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Panel_Second(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Panel_Second(const CUI_Play_Panel_Second& rhs);
	virtual ~CUI_Play_Panel_Second(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_FontCnt(const _int& iCnt, const _int& iLimCnt, const _bool& bAcc);
	void Set_FontStr(const _tchar* pText, const _bool& bAcc);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	SFontDesc m_sFont;
};
END_NAMESPACE

