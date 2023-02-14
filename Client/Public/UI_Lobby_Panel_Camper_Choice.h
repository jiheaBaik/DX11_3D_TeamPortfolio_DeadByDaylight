#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Lobby_Panel_Camper_Choice final : public CUI_Image
{
public:
	struct SHudFX {
		SImageDesc* pImage = nullptr;
		_int iIndex = -1;
		_bool bClick = false;
		_bool bDown = false;
		_float2 vSpread = { 0.5f, 0.f };
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
		void Set_Hud(const _int& iIndex, const _bool& bClick, const _float4& vColor) {
			this->iIndex = iIndex;
			this->bClick = bClick;
			this->vColor = vColor;
		}
	};
public:
	static CUI_Lobby_Panel_Camper_Choice* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Lobby_Panel_Camper_Choice(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Lobby_Panel_Camper_Choice(const CUI_Lobby_Panel_Camper_Choice& rhs);
	virtual ~CUI_Lobby_Panel_Camper_Choice(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	SImageDesc* const& Get_NameTag() { return m_vecNameTags[m_iChoice]; }
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	vector<SHudFX> m_vecHuds;
	CTexture* m_pFXTexture[2] = { nullptr, };

	_uint m_iChoice = 0;
	vector<SImageDesc*> m_vecNameTags;
};
END_NAMESPACE

