#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Lobby_Icon_Ready_Camper final : public CUI_Image
{
public:
	enum class EType { READY, PLUS, END  };
	const _char* EType_ToString(const EType& eType) {
		switch (eType)
		{
		case EType::READY: return "READY";
		case EType::PLUS: return "PLUS";
		}
		return nullptr;
	}
public:
	static CUI_Lobby_Icon_Ready_Camper* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Lobby_Icon_Ready_Camper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Lobby_Icon_Ready_Camper(const CUI_Lobby_Icon_Ready_Camper& rhs);
	virtual ~CUI_Lobby_Icon_Ready_Camper(void) override = default;
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
	_bool m_bSelected = false;
	_char m_szTexKey[_MAX_PATH] = "";
};
END_NAMESPACE

