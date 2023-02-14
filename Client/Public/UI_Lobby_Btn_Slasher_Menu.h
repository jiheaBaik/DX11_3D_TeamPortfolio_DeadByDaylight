#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Lobby_Btn_Slasher_Menu final : public CUI_Image
{
public:
	enum class EMenuType { ONE, TWO, THREE, FOUR, END };
	enum class ETexType { BRIDGE, BRIDGE2, ICON, ICON2, TEXT, FRAME, END };
public:
	static CUI_Lobby_Btn_Slasher_Menu* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Lobby_Btn_Slasher_Menu(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Lobby_Btn_Slasher_Menu(const CUI_Lobby_Btn_Slasher_Menu& rhs);
	virtual ~CUI_Lobby_Btn_Slasher_Menu(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	// set selected idx
	static void Set_MenuItmes(const _tchar* pLayer = nullptr, const _int& iIndex = -1);
	static void Set_OnClicked(const _int& iBtnIdx) { s_iOnClicked = iBtnIdx; }
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	static _int s_iOnClicked;
	static list<class CObject*>* s_pMenuItems;
	_tchar m_szMenuItems[_MAX_PATH] = L"";
	_float2 m_vSpread = { 0.5f, 0.f };
};
END_NAMESPACE

