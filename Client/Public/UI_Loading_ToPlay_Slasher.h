#pragma once
#include "UI_Image.h"

BEGIN_NAMESPACE(Client)
class CUI_Loading_ToPlay_Slasher final : public CUI_Image
{
public:
	struct STip {
		SImageDesc* pImage = nullptr;
		_uint iIndex = 0;
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	};
public:
	static CUI_Loading_ToPlay_Slasher* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CUI_Loading_ToPlay_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI_Loading_ToPlay_Slasher(const CUI_Loading_ToPlay_Slasher& rhs);
	virtual ~CUI_Loading_ToPlay_Slasher(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	SImageDesc* const Add_Image(const _tchar* pTexKey, const _int& iDepth, const _float3& vPos = { 0.f, 0.f, 0.f }, const CRectTransform::EEdge& eEdge = CRectTransform::EEdge::CEN, const _bool& bSizeOfTex = true);
private:
	_uint m_iNumImages = 0;
	vector<SImageDesc*> m_vecTips;
	_uint m_iTip = 0;
	_float m_fGauge = 0.f;

};
END_NAMESPACE

