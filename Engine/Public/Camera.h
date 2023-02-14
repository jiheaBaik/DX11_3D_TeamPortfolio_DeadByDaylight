#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CCamera abstract : public CObject{
public:
	struct SDesc {
		const _tchar* pKey = nullptr;
		_float fFovy = XMConvertToRadians(60.0f), fAspect = 1280.f / 720.f , fNear = 0.2f, fFar = 500.f;
	};
public:
	virtual CObject* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CCamera(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double & dTimeDelta) PURE;
	virtual void Late_Update(const _double & dTimeDelta) PURE;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) PURE;
public:
	_vector Get_Pos(void) const;
	_matrix Get_ViewMatrix(void) const;
	_matrix Get_ViewMatrix_Transpose(void) const;
	_matrix Get_ViewMatrix_Inverse(void) const;
	_matrix Get_ProjMatrix(void) const;
	_matrix Get_ProjMatrix_Transpose(void) const;
	_matrix Get_ProjMatrix_Inverse(void) const;
	void Set_OldViewMatrix(void);
	void Set_ViewMatrix(void);
	const HRESULT Set_RawValue(class CShader* const& pShader);
public:
	void Set_Camera(const _uint & iCamera);
protected:
	class CTransform* m_pTransform = nullptr;
	_float4x4 m_ViewFloat4x4;
	_float4x4 m_ProjFloat4x4;
	_float4x4 m_OldViewFloat4x4;
public:
	static class CCamera* const Get_Camera(const _uint & iCamera);
	static class CCamera* const Get_Camera(const _tchar* const& pCameraKey);
protected:
	static vector<class CCamera*> s_vecCamera;
	static map<wstring, class CCamera*> s_mapCamera;
};
END_NAMESPACE