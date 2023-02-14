#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Loading_Icon_Circle final : public CUI_Image
{
public:
	static CUI_Loading_Icon_Circle* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Loading_Icon_Circle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Loading_Icon_Circle(const CUI_Loading_Icon_Circle& rhs);
	virtual ~CUI_Loading_Icon_Circle(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	_double m_dTime = 0.;
	_bool m_bRender = false;
};
END_NAMESPACE

