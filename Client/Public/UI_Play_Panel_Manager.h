#pragma once
#include "UI_Play_Panel_First.h"
#include "UI_Play_Panel_Second.h"
#include "UI_End_Panel_First.h"
#include "UI_End_Panel_Second.h"
#include "UI_End_Panel_FX.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Panel_Manager final : public CUI_Image
{
public:
	enum class EPanel { NONE=-1, PLAY1, PLAY2, END1, END2, FADE, END };
public:
	static CUI_Play_Panel_Manager* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Panel_Manager(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Panel_Manager(const CUI_Play_Panel_Manager& rhs);
	virtual ~CUI_Play_Panel_Manager(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	CUI_Image* const Get_Panel(const EPanel& ePanel);
	void Set_Panel(const EPanel& ePanel, const _bool& bActive, const EPanel& eOther = EPanel::NONE);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	CUI_Image* m_pPanels[static_cast<_uint>(EPanel::END)] = { nullptr, };
};
END_NAMESPACE

