#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Gauge_Exit final : public CUI_Image
{
public:
	enum class ETexType { FILLED, FIX_ANC, MOVE_ANC, END };
public:
	static CUI_Play_Gauge_Exit* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Gauge_Exit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Gauge_Exit(const CUI_Play_Gauge_Exit& rhs);
	virtual ~CUI_Play_Gauge_Exit(void) override = default;
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
	_float m_fGauge = 100.f;
	_bool m_bDecrease = false;
	_float3 m_vInitAnc = { 0.f, 0.f, 0.f };
	// woojin
	_bool m_bEndGameCreate = false;
};
END_NAMESPACE

