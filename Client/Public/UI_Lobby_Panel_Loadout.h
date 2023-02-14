#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Lobby_Panel_Loadout final : public CUI_Image
{
public:
	enum class ETexType { LAYOUT = -1, TAG, BACK, ICON, FX, TEXT, END };
	struct SHud {
		SImageDesc* pImage = nullptr;
		_int iIndex = -1;
		_bool bClick = false;
		_bool bDown = false;
		_bool bHover = false;
		_float2 vSpread = { 0.5f, 0.f };
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
		SHud() {};
		void Set_Hud(const _int& iIndex, const _bool& bClick, const _float4& vColor) {
			this->iIndex = iIndex;
			this->bClick = bClick;
			this->vColor = vColor;
		}
	};
	struct SHudFX {
		CTexture* pTexture = nullptr;
		_bool bRedner = true;
		_uint iPass = 1;
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
		SHudFX() {};
		void Reset() {
			pTexture = nullptr;
			bRedner = true;
			iPass = 1;
			vColor = { 1.f, 1.f, 1.f, 1.f };
		}
	};

public:
	static CUI_Lobby_Panel_Loadout* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Lobby_Panel_Loadout(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Lobby_Panel_Loadout(const CUI_Lobby_Panel_Loadout& rhs);
	virtual ~CUI_Lobby_Panel_Loadout(void) override = default;
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
	const _uint m_iRow = 3, m_iCol = 4;
	_uint m_iPerkNum = m_iRow * m_iCol;
	// back
	SHud m_sSlots[4];
	SHud m_sInvens[12];
	// hover
	SHud m_sExplains[12];
	_uint m_iHover_Inven = 0;
	_uint m_iHover_Slot = 0;
	// icon, fx
	vector<SHudFX> m_vecHud; 
	SHudFX m_sHudFXs[2];

#ifdef _DEBUG
	SImageDesc* m_pLayouts[2] = { nullptr, };
#endif // _DEBUG

};
END_NAMESPACE

