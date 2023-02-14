#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Lobby_Panel_Explain final : public CUI_Image
{
public:
	enum class EMenuType { ONE, TWO, THREE, FOUR, END };
	enum class ETexType { BACK, TAG, TEXT1, TEXT2, FLOW1, FLOW2, END };
public:
	static CUI_Lobby_Panel_Explain* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Lobby_Panel_Explain(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Lobby_Panel_Explain(const CUI_Lobby_Panel_Explain& rhs);
	virtual ~CUI_Lobby_Panel_Explain(void) override = default;
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
	_float2 m_vTrans = { 0.f, 0.f };
	CTexture* m_pFlowTextures[3] = { nullptr, };
};
END_NAMESPACE

