#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Play_Icon_Propile final : public CUI_Image
{
public:
	enum class EState { PROPILE=0, HURT1=1, HURT2, HOOK, DEAD, ESCAPE, END };
	struct SHudFX {
		SImageDesc* pImage = nullptr;
		SImageDesc* pFXImage = nullptr;
		_float fAlpha = 0.f;
		_int iState = -1;
		SHudFX() {};
		void Init(const _int& iState, SImageDesc* const& pImage) {
			this->iState = iState;
			this->pImage = pImage;
		}
	};
	struct SProfile {
		CTexture* pTexture = nullptr;
		CRectTransform* pRectTransform = nullptr;
		SImageDesc* pGaugeImg = nullptr;
		SImageDesc* pHookImg[2] = { nullptr, };
		_bool bState = false;
		_int iState = -1;
		_float fSpiderTime = 0.f;
		_uint iHookCnt = 0;
	};
public:
	static CUI_Play_Icon_Propile* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Play_Icon_Propile(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Play_Icon_Propile(const CUI_Play_Icon_Propile& rhs);
	virtual ~CUI_Play_Icon_Propile(void) override = default;
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
	SHudFX m_sHuds[static_cast<_uint>(EState::END)-1];
	SProfile m_sProfiles[4];
	CTexture* m_pHpTextures[2] = { nullptr, };
	_uint m_iState = 0;
	_uint m_iChaIndex = 0;
};
END_NAMESPACE

