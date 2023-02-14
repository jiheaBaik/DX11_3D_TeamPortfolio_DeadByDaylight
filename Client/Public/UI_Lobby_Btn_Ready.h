#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Lobby_Btn_Ready final : public CUI_Image
{
public:
	enum class ETexType { FRAME, ICON, FX, TEXT1, TEXT2, END };
public:
	static CUI_Lobby_Btn_Ready* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Lobby_Btn_Ready(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Lobby_Btn_Ready(const CUI_Lobby_Btn_Ready& rhs);
	virtual ~CUI_Lobby_Btn_Ready(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	const _uint& Get_ReadyCnt() { return s_iReadyCnt; }
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	static _uint s_iReadyCnt;
	_bool m_bReady = false;
	_uint m_iFXIdx = 0;
};
END_NAMESPACE

