#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Icon_Hud final : public CUI_Image
{
public:
	enum class EKey { NONE = -1, MOUSE_L, SPACE, MOUSE_R, EKEY, RKEY, END };
	enum class EDiv { NONE = -1, LEFT, CENTER, RIGHT, END }; // division
	struct SHud {
		SImageDesc* pHudImg = nullptr;
		_int iKey = -1;
		_bool bDelay = false;
		SHud() { this->Reset(); }
		void Init(SImageDesc* const& pHudImg, const _int& iKey) {
			this->pHudImg = pHudImg;
			this->iKey = iKey;
		}
		void Reset() {
			pHudImg = nullptr;
			iKey = -1;
		}
	};
	struct SHudFX {
		CTexture* pTexture = nullptr;
		_int iDiv = -1;
		_int iType = -1;
		_bool bRender = false;
		SHudFX() { this->Reset(); }
		void Init(const _int& iDiv, const _bool& bRender) {
			this->iDiv = iDiv;
			this->bRender = bRender;
		}
		void Reset() {
			iDiv = -1;
			iType = -1;
			bRender = false;
		}
	};
	struct SPos {
		_float2 vPivot = { 0.f, 0.f };
		_float3 vHudPos = { 0.f, 0.f, 0.f };
		SPos() {};
		void Reset(const _float2& vPivot, const _float3& vHudPos) {
			this->vPivot = vPivot;
			this->vHudPos = vHudPos;
		}
	};
	enum class ESlasher { CANCLE, PARKOUR, FIND, DESTROY, SONSANG, HANG, LIFT, CLOSE, KILL, PERK1/*, PERK2*/, PASSIVE1, END };
	enum class ECamper { CANCLE, PARKOUR, NURMD, HIDE, REPAIR, RESCUE, ESCTRY, ESCAPE, HEALSEL, HEALOTH, UNLOCK, GET, TOTEM, WIGGLE, PERK1, PERK2, END };
public:
	static CUI_Play_Icon_Hud* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Icon_Hud(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Icon_Hud(const CUI_Play_Icon_Hud& rhs);
	virtual ~CUI_Play_Icon_Hud(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	SImageDesc* const Get_HudImage(const _int& iType);
	SImageDesc* const Get_HudImage(const EDiv& eDiv);
	const _bool Get_IsEmptyDiv(const EDiv& eDiv, const _int& iType = -1);

	void Set_HudOn(const _int& iType, const EKey& eKey, const EDiv& eDiv, const _bool& bDelay = false);
	void Set_HudOn(const _int& iType, const EKey& eKey, const EDiv& eSour, const EDiv& eDest, const _bool& bDelay = false);
	void Set_HudOff(const _uint& iType, const _bool& bUnBind, const _bool& bDelay = false);
	void Set_HudOff(const EDiv& eDiv, const _bool& bUnBind, const _bool& bDelay = false);
	void Set_AllHuds(const _bool& bRender, const _int& iNullType, const _bool& bDelay = false);
	void Set_AllHuds(const _bool& bRender, const EDiv& eDiv = EDiv::NONE, const _bool& bDelay = false);
	void Set_AllHudsOn(const ESlasher& eLeft, const EKey& eLKey, const ESlasher& eCenter, const EKey& eCKey, const ESlasher& eRight, const EKey& eRKey);
	void Set_AllHudsOn(const ECamper& eLeft, const EKey& eLKey, const ECamper& eCenter, const EKey& eCKey, const ECamper& eRight, const EKey& eRKey);
	void Set_Delay() { m_bDelay = true; }
private:
	void Bind_Hud(const _uint& iType, const _uint& iKey, const _uint& iDiv);
	void Bind_Pos(const EDiv& eSour, const EDiv& eDest = EDiv::NONE, const _bool& bRender = true);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	// common
	SImageDesc m_sKeys[static_cast<_uint>(EKey::END)];
	SHud m_sHuds[static_cast<_uint>(EDiv::END)];
	SPos m_sPoses[static_cast<_uint>(EDiv::END)];
	_bool m_bDelay = false;
};
END_NAMESPACE

