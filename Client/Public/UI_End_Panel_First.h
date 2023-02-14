#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_End_Panel_First final : public CUI_Image
{
public:
	enum class ECamper { ESCAPE, SACRIFICE, END };
	enum class ESlasher { MURDER, PEACEFUL, END };
public:
	static CUI_End_Panel_First* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_End_Panel_First(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_End_Panel_First(const CUI_End_Panel_First& rhs);
	virtual ~CUI_End_Panel_First(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_State(const _uint& iState, const _bool& bOneOff = false, const _bool& bActive = false);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	CTexture* m_pDissolveTexture = nullptr;
	_float m_fDissolve = 1.f;
};
END_NAMESPACE
