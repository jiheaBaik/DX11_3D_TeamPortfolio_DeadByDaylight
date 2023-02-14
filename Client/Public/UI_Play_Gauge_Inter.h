#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Gauge_Inter final : public CUI_Image
{
public: 
	enum class ETexType { BACK, ICON, END };
	enum class EInter { SLA_SKILL=-4, DESTROY=-3, HANG=-2, SONSANG=-1, NONE=0, GEN, CHEST, TOTEM, HEALSEL, HEALOTH, RESQUE, EXIT, END };
	struct SHud {
		SImageDesc* pHudImage = nullptr;
		_bool bRender = false;
		_bool bInter = false;
		_int iGauge = -1;
		_float fGauge = 0.f;
		SHud() {};
		void Reset() {
			bRender = false;
			bInter = false;
			fGauge = 0.f;
			iGauge = -1;
		}
	};
public:
	static CUI_Play_Gauge_Inter* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Gauge_Inter(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Gauge_Inter(const CUI_Play_Gauge_Inter& rhs);
	virtual ~CUI_Play_Gauge_Inter(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_Gauge(const EInter& eInter, const _float& fGauge, const _float& fAccel = 1.f, const _bool& bAcc = false);
	void Set_Gauge(const EInter& eInter, const _float& fGauge, const _bool& bReset);
	void End_Gauge(const EInter& eInter);
	const _bool Get_Render(const EInter& eInter, const _bool& bInter);
	const _bool Get_Inter(const EInter& eInter);
	const _float Get_Gauge(const EInter& eInter);
	void Set_Render(const EInter& eInter, const _bool& bRender, const _bool& bInter);
	void Set_RenderAll(const _bool& bRender, const _bool& bInter);
	void Set_Inter(const EInter& eInter, const _bool& bInter);
	void Set_InterAll(const _bool& bInter);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	// allocate inters cnt
	SHud m_sHuds[static_cast<_uint>(EInter::END)]; 
	CTexture* m_pFilledTexs[3] = { nullptr, };
	class CUI_Play_Icon_Hud* m_pHud = nullptr;
};
END_NAMESPACE

