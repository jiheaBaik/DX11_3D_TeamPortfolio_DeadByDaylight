#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Icon_Perk final : public CUI_Image
{
public:
	enum class ETexType { BACK, ICON, LVL, HUB, END };
	struct SHudFX {
		SImageDesc* pImage = nullptr;
		CTexture* pIconTex = nullptr;
		CPerk* pPerk = nullptr;
		_bool bRedner = true;
		_uint iPass = 1;
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
		SHudFX() {};
		void Reset() {
			//pTexture = nullptr;
			//pPerk = nullptr;
			bRedner = true;
			iPass = 1;
			vColor = { 1.f, 1.f, 1.f, 1.f };
		}
	};
public:
	static CUI_Play_Icon_Perk* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Icon_Perk(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Icon_Perk(const CUI_Play_Icon_Perk& rhs);
	virtual ~CUI_Play_Icon_Perk(void) override = default;
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
	SHudFX m_sHuds[4]; // fx
	CTexture* m_pGaugeTexs[2] = { nullptr, };
};
END_NAMESPACE

