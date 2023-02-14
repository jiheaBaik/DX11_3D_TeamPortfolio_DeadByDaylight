#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Lobby_Btn_Select_To_Start final : public CUI_Image
{
public:
	enum class ETexType { FRAME, ICON, FX, TEXT1, TEXT2, END };
public:
	static CUI_Lobby_Btn_Select_To_Start* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Lobby_Btn_Select_To_Start(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Lobby_Btn_Select_To_Start(const CUI_Lobby_Btn_Select_To_Start& rhs);
	virtual ~CUI_Lobby_Btn_Select_To_Start(void) override = default;
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
	_uint m_iFXIdx =0;
};
END_NAMESPACE

