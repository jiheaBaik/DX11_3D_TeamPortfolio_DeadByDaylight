#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CCompute final : public CBase{
	DECLARE_SINGLETON(CCompute)
private:
	explicit CCompute(void);
	virtual ~CCompute(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _uint& iWidth, const _uint& iHeight);
public:
	void Update_Edit(const _int& iPushID);
public:
	const HRESULT Shadow(void);
	const HRESULT Blend(void);
	const HRESULT Back(void);
	const HRESULT Distortion(void);
	const HRESULT Glow(void);
	const HRESULT DOF(void);
	const HRESULT HDR(void);
	const HRESULT Back1(void);
	const HRESULT Back2(void);
	const HRESULT Bloom(void);
	const HRESULT MotionBlur(void);
	const HRESULT SSAO(void);
	const HRESULT Fog0(void);
	const HRESULT Fog1(void);
	const HRESULT Light(void);
	const HRESULT LumaSharpen(void);
	const HRESULT HSBC(void);

	const HRESULT Shadow_Enviro(void);
	const HRESULT Blend_Enviro(void);
	const HRESULT Back_Enviro(void);
	const HRESULT Distortion_Enviro(void);
	const HRESULT Glow_Enviro(void);
	const HRESULT DOF_Enviro(void);
	const HRESULT HDR_Enviro(void);
	const HRESULT Back1_Enviro(void);
	const HRESULT Back2_Enviro(void);
	const HRESULT Bloom_Enviro(void);
	const HRESULT MotionBlur_Enviro(void);
	const HRESULT SSAO_Enviro(void);
	const HRESULT Fog0_Enviro(void);
	const HRESULT Fog1_Enviro(void);
	const HRESULT Light_Enviro(void);
public:
	const _float& Get_Saturation(void);
	void Set_Saturation(const _float& fSaturation);
	void Add_Saturation(const _float& fSaturation);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
private:
	class CShader* m_pShader_Back = nullptr;
	class CShader* m_pShader_Blend = nullptr;
	class CShader* m_pShader_Blur = nullptr;
	class CShader* m_pShader_Glow = nullptr;
	class CShader* m_pShader_SSAO = nullptr;
	class CShader* m_pShader_DOF = nullptr;
	class CShader* m_pShader_HDR = nullptr;
	class CShader* m_pShader_Bloom = nullptr;
	class CShader* m_pShader_MotionBlur = nullptr;
	class CShader* m_pShader_Fog = nullptr;
	class CShader* m_pShader_Distortion = nullptr;
	class CShader* m_pShader_Light = nullptr;
	class CShader* m_pShader_LumaSharpen = nullptr;
	class CShader* m_pShader_HSBC = nullptr;

	class CTarget_Mgr* m_pTarget_Mgr = nullptr;
	class CTarget* m_pShadow0 = nullptr;
	class CTarget* m_pShadow1 = nullptr;
	class CTarget* m_pShadow2 = nullptr;
	class CTarget* m_pDiffuse = nullptr;
	class CTarget* m_pShade = nullptr;
	class CTarget* m_pSpecular = nullptr;
	class CTarget* m_pWorld = nullptr;
	class CTarget* m_pShadow = nullptr;
	class CTarget* m_pShadow_Bake = nullptr;
	class CTarget* m_pBlend = nullptr;
	class CTarget* m_pSSAO0 = nullptr;
	class CTarget* m_pBack = nullptr;
	class CTarget* m_pDistortion = nullptr;
	class CTarget* m_pBack0 = nullptr;
	class CTarget* m_pDistortion0 = nullptr;
	class CTarget* m_pGlow = nullptr;
	class CTarget* m_pGlow0 = nullptr;
	class CTarget* m_pGlow1 = nullptr;
	class CTarget* m_pGlow2 = nullptr;
	class CTarget* m_pDOF0 = nullptr;
	class CTarget* m_pDOF1 = nullptr;
	class CTarget* m_pDOF2 = nullptr;
	class CTarget* m_pFog1 = nullptr;
	class CTarget* m_pHDR0 = nullptr;
	class CTarget* m_pHDR1 = nullptr;
	class CTarget* m_pPenetrate = nullptr;
	class CTarget* m_pBloom1 = nullptr;
	class CTarget* m_pBack1 = nullptr;
	class CTarget* m_pBloom0 = nullptr;
	class CTarget* m_pBloom2 = nullptr;
	class CTarget* m_pMotionBlur = nullptr;
	class CTarget* m_pSSAO = nullptr;
	class CTarget* m_pSSAO1 = nullptr;
	class CTarget* m_pDepth = nullptr;
	class CTarget* m_pGodRay0 = nullptr;
	class CTarget* m_pFog0 = nullptr;
	class CTarget* m_pNormal = nullptr;
	class CTarget* m_pMaterial = nullptr;
	class CTarget* m_pLumaSharpen = nullptr;
	class CTarget* m_pHSBC = nullptr;

	class CTexture* m_pFogTexture = nullptr;
private:
	ID3D11ShaderResourceView* m_pClearSRV[128] = { nullptr };
	ID3D11UnorderedAccessView* m_pClearUAV[7] = { nullptr };
	public:
		void Set_FogEnd(const _float& fFogEnd) { m_fFogEnd = fFogEnd; }
private:
	_float2 m_fWinSize = { 0.f, 0.f };

	//Blend
	_float4 m_vShadowColor = { 0.7f, 0.7f, 0.7f, 1.f };

	//Distortion
	_float m_fDistortion = 10.f;

	//DOF
	_float m_fDepth = 0.2f;

	//Fog
	_float4 m_vFogColor = { 0.168f, 0.150f, 0.126f, 1.f };//{ 0.678f, 0.552f, 0.448f, 1.f };
	_float m_fFogStart = 0.01f;
	_float m_fFogEnd = 0.07f;

	_float4 m_vFogColor2 = { 0.180f, 0.166f, 0.151f, 1.f };//{ 0.691f, 0.552f, 0.448f, 1.f };
	_float m_fMaxHeight = 0.48f;
	_float m_fMinHeight = 0.f;
	_float m_fTexScale = 20.f;
	_float2 m_vTexel = { 0.f, 0.f };
	_float2 m_vTexel2 = { 0.f, 0.f };

	_float2 m_fCameraDist = { 5.f, 20.f };

	//HDR
	_float m_fMiddleGrey = 1.2f;//3.97f;
	_float m_fLumWhiteSqr = 1.239f;
	_float m_fBloomScale = 0.f;

	//Bloom
	_float m_fBloomThreshold = 0.f;

	//Oren-Nayar
	_bool m_bOnOff = true;
	_float m_fAlbedo = 3.14f;
	_float m_fRoughness = 0.275f;

	//Cook-Torrance
	_bool m_bOnOff2 = true;
	_float m_fMetalness = 0.f;

	//Glow
	_float2 m_fGlowPower = { 3.f, 3.f };

	//LumaSharpen
	_float m_fSharp_Strength = 1.25f;
	_float m_fSharp_Clamp = 0.035f;
	_int m_iPattern = 9;
	_float m_iOffset_bias = 1.f;
	_float2 m_fOffset = { 1.f, 1.f };
	_bool m_bShow_Sharpen = true;

	//HSBC
	_float m_fHue = 0.f;
	_float m_fSaturation = 1.2f;
	_float m_fBrightness = 0.f;
	_float m_fContrast = 1.f;
	_bool m_bShow_HSBC = true;
};
END_NAMESPACE
