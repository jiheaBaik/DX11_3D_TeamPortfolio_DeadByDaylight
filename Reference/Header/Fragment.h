#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CFragment final : public CBase {
	DECLARE_SINGLETON(CFragment)
private:
	explicit CFragment(void);
	virtual ~CFragment(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
public:
	void Update_Edit(const _int & iPushID);
public:
	const HRESULT Defferd(void);
	const HRESULT SSAO(void);
	const HRESULT GodRay(void);
	const HRESULT Light(void);

	const HRESULT Defferd_Enviro(void);
	const HRESULT SSAO_Enviro(void);
	const HRESULT GodRay_Enviro(void);
public:
	void Set_GodRayPos(const _float3& vGodRayPos);
	void Set_GodRayPos(_fvector vGodRayPos);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
private:
	class CTarget_Mgr* m_pTarget_Mgr = nullptr;
	class CShader* m_pShader_Deferred = nullptr;
	class CShader* m_pShader_SSAO = nullptr;
	class CShader* m_pShader_GodRay = nullptr;
	class CShader* m_pShader_Light = nullptr;

	class CTarget* m_pBack0 = nullptr;
	class CTarget* m_pWorld = nullptr;
	class CTarget* m_pNormal = nullptr;
	class CTarget* m_pGodRay = nullptr;
	class CTarget* m_pLumaSharpen = nullptr;
	class CTarget* m_pHSBC = nullptr;

	class CBuffer_Rect* m_pBuffer = nullptr;
private:
	_float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_float4x4 m_WorldMatrix_Enviro, m_ViewMatrix_Enviro, m_ProjMatrix_Enviro;
public:
	//GodRay
	list<_float3> m_listGodRayPos;
	_float4 m_vLightShaftValue = { 1.096f, 0.959f, 0.014f, 0.748f };

	//SSAO
	_float m_fSample_rad = 0.371f;
	_float m_fIntensity = 10.026f;
	_float m_fScale = 6.981f;
	_float m_fBias = 0.486f;
};
END_NAMESPACE
