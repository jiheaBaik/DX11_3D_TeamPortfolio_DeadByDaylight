#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CUI abstract : public CObject{
public:
	virtual CObject* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CUI(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CUI(const CUI& rhs);
	virtual ~CUI(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const _ubyte Update(const _double & dTimeDelta) PURE;
	virtual void Late_Update(const _double & dTimeDelta) PURE;
	virtual const HRESULT Render(const CRenderer::EOrder & eOrder) PURE;
public:
	const HRESULT Set_RawValue(class CShader* const& pShader);
	const _int& Get_Depth(void) const;
protected:
	_int m_iDepth = 0;
public:
	static const HRESULT Init(const _float & fViewWidth, const _float & fViewHeight);
public:
	static _matrix Get_ViewMatrix(void);
	static _matrix Get_ProjMatrix(void);
protected:
	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjMatrix;
protected:
	static _float4x4 s_ViewMatrix;
	static _float4x4 s_ProjMatrix;
};
END_NAMESPACE