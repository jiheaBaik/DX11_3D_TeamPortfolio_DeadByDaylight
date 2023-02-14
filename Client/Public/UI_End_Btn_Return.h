#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_End_Btn_Return final : public CUI_Image
{
public:
	enum class ETexType { FRAME, ICON, FX, TEXT, END };
public:
	static CUI_End_Btn_Return* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_End_Btn_Return(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_End_Btn_Return(const CUI_End_Btn_Return& rhs);
	virtual ~CUI_End_Btn_Return(void) override = default;
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
	_uint m_iFXIdx = 0;
};
END_NAMESPACE

