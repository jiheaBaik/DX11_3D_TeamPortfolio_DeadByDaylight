#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_End_Panel_Second final : public CUI_Image
{
public:
	enum class ETexType { PANEL1, PANEL2, PANEL3, ICON, TEXT, END };
	enum class EScoreType { ONE, TWO, THREE, FOUR, RES, END };
	struct SHud {
		SImageDesc* pIconImg = nullptr;
		SImageDesc* pExpImg = nullptr;
		SFontDesc sFont;
		_uint* pScore = nullptr;
		_uint iScore = 0;
		_bool m_bHover = false;

		SFontDesc sFonts[5];
		_uint iDigits[5] = { 0, };
		_bool bOneOff = false;
		_uint iIdx = 0;
	};
public:
	static CUI_End_Panel_Second* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_End_Panel_Second(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_End_Panel_Second(const CUI_End_Panel_Second& rhs);
	virtual ~CUI_End_Panel_Second(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_FontCnt(const EScoreType& eScore, const _int& iCnt, const _bool& bAcc = false);
	void Set_FontStr(const EScoreType& eScore, const _tchar* pText, const _bool& bAcc = false);
private:
	void Calculate_Score(_uint* const& pDigit, const _uint& iScore, _bool* const& bRenders);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	_float* m_pAlpha = nullptr;
	_bool m_bActiveSelf2 = true; // render
	// btn
	SHud m_sHuds[5];
	_float m_fBtnAlpha = -1.f;
};
END_NAMESPACE

