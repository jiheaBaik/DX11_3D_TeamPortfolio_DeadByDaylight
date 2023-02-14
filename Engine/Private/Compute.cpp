#include "Compute.h"
#include "Target_Mgr.h"
#include "Shader.h"
#include "Game_Instance.h"
#include "Enviro.h"

IMPLEMENT_SINGLETON(CCompute)

CCompute::CCompute(void)
	: CBase() {
}

void CCompute::Delete(void) {
	Safe_Release(m_pShader_HSBC);
	Safe_Release(m_pShader_LumaSharpen);
	Safe_Release(m_pShader_Light);
	Safe_Release(m_pShader_Distortion);
	Safe_Release(m_pShader_Fog);
	Safe_Release(m_pShader_MotionBlur);
	Safe_Release(m_pShader_Bloom);
	Safe_Release(m_pShader_HDR);
	Safe_Release(m_pShader_DOF);
	Safe_Release(m_pShader_SSAO);
	Safe_Release(m_pShader_Glow);
	Safe_Release(m_pShader_Blur);
	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pShader_Back);
	Safe_Release(m_pTarget_Mgr);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CCompute::Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _uint& iWidth, const _uint& iHeight) {
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	m_fWinSize.x = static_cast<_float>(iWidth);
	m_fWinSize.y = static_cast<_float>(iHeight);

	m_pTarget_Mgr = CTarget_Mgr::Get_Instance();
	Safe_AddRef(m_pTarget_Mgr);
	m_pShader_Back = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Back.hlsl"));
	m_pShader_Blend = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Blend.hlsl"));
	m_pShader_Blur = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Blur.hlsl"));
	m_pShader_Glow = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Glow.hlsl"));
	m_pShader_SSAO = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/SSAO.hlsl"));
	m_pShader_DOF = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/DOF.hlsl"));
	m_pShader_HDR = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/HDR.hlsl"));
	m_pShader_Bloom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Bloom.hlsl"));
	m_pShader_MotionBlur = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/MotionBlur.hlsl"));
	m_pShader_Fog = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Fog.hlsl"));
	m_pShader_Distortion = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Distortion.hlsl"));
	m_pShader_Light = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Light.hlsl"));
	m_pShader_LumaSharpen = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/LumaSharpen.hlsl"));
	m_pShader_HSBC = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/HSBC.hlsl"));

	m_pShadow0 = m_pTarget_Mgr->Get_Target(TEXT("Shadow0"));
	m_pShadow1 = m_pTarget_Mgr->Get_Target(TEXT("Shadow1"));
	m_pShadow2 = m_pTarget_Mgr->Get_Target(TEXT("Shadow2"));
	m_pDiffuse = m_pTarget_Mgr->Get_Target(TEXT("Diffuse"));
	m_pShade = m_pTarget_Mgr->Get_Target(TEXT("Shade"));
	m_pSpecular = m_pTarget_Mgr->Get_Target(TEXT("Specular"));
	m_pWorld = m_pTarget_Mgr->Get_Target(TEXT("World"));
	m_pShadow = m_pTarget_Mgr->Get_Target(TEXT("Shadow"));
	m_pShadow_Bake = m_pTarget_Mgr->Get_Target(TEXT("Shadow_Bake"));
	m_pBlend = m_pTarget_Mgr->Get_Target(TEXT("Blend"));
	m_pSSAO0 = m_pTarget_Mgr->Get_Target(TEXT("SSAO0"));
	m_pBack = m_pTarget_Mgr->Get_Target(TEXT("Back0"));
	m_pDistortion = m_pTarget_Mgr->Get_Target(TEXT("Distortion"));
	m_pBack0 = m_pTarget_Mgr->Get_Target(TEXT("Back0"));
	m_pDistortion0 = m_pTarget_Mgr->Get_Target(TEXT("Distortion0"));
	m_pGlow = m_pTarget_Mgr->Get_Target(TEXT("Glow"));
	m_pGlow0 = m_pTarget_Mgr->Get_Target(TEXT("Glow0"));
	m_pGlow1 = m_pTarget_Mgr->Get_Target(TEXT("Glow1"));
	m_pGlow2 = m_pTarget_Mgr->Get_Target(TEXT("Glow2"));
	m_pDOF0 = m_pTarget_Mgr->Get_Target(TEXT("DOF0"));
	m_pDOF1 = m_pTarget_Mgr->Get_Target(TEXT("DOF1"));
	m_pDOF2 = m_pTarget_Mgr->Get_Target(TEXT("DOF2"));
	m_pFog1 = m_pTarget_Mgr->Get_Target(TEXT("Fog1"));
	m_pHDR0 = m_pTarget_Mgr->Get_Target(TEXT("HDR0"));
	m_pHDR1 = m_pTarget_Mgr->Get_Target(TEXT("HDR1"));
	m_pPenetrate = m_pTarget_Mgr->Get_Target(TEXT("Penetrate"));
	m_pBloom1 = m_pTarget_Mgr->Get_Target(TEXT("Bloom1"));
	m_pBack1 = m_pTarget_Mgr->Get_Target(TEXT("Back1"));
	m_pBloom0 = m_pTarget_Mgr->Get_Target(TEXT("Bloom0"));
	m_pBloom2 = m_pTarget_Mgr->Get_Target(TEXT("Bloom2"));
	m_pMotionBlur = m_pTarget_Mgr->Get_Target(TEXT("MotionBlur"));
	m_pSSAO = m_pTarget_Mgr->Get_Target(TEXT("SSAO"));
	m_pSSAO1 = m_pTarget_Mgr->Get_Target(TEXT("SSAO1"));
	m_pDepth = m_pTarget_Mgr->Get_Target(TEXT("Depth"));
	m_pGodRay0 = m_pTarget_Mgr->Get_Target(TEXT("GodRay0"));
	m_pFog0 = m_pTarget_Mgr->Get_Target(TEXT("Fog0"));
	m_pNormal = m_pTarget_Mgr->Get_Target(TEXT("Normal"));
	m_pMaterial = m_pTarget_Mgr->Get_Target(TEXT("Material"));
	m_pLumaSharpen = m_pTarget_Mgr->Get_Target(TEXT("LumaSharpen"));
	m_pHSBC = m_pTarget_Mgr->Get_Target(TEXT("HSBC"));
	return S_OK;
}

void CCompute::Update_Edit(const _int& iPushID) {
	ImGui::PushID(iPushID);
	ImGui::Separator();
	ImGui::Text("Compute");

	ImGui::Text("Depth");
	ImGui::DragFloat("Depth", &m_fDepth, 0.01f, 0.f, 1.f);

	ImGui::Text("Blend");
	ImGui::DragFloat4("ShadowColor", (_float*) & m_vShadowColor, 0.01f, 0.f, 1.f);
	

	ImGui::Text("Distortion");
	ImGui::DragFloat("Distortion", &m_fDistortion, 0.01f);

	ImGui::Text("Fog0");
	ImGui::DragFloat4("FogColor", (_float*)&m_vFogColor, 0.001f);
	ImGui::DragFloat("FogStart", &m_fFogStart, 0.001f);
	ImGui::DragFloat("FogEnd", &m_fFogEnd, 0.001f);

	ImGui::Text("Fog1");
	ImGui::DragFloat4("FogColor2", (float*)&m_vFogColor2, 0.001f);
	ImGui::DragFloat("MaxHeight", &m_fMaxHeight, 0.001f);
	ImGui::DragFloat("MinHeight", &m_fMinHeight, 0.001f);
	ImGui::DragFloat("TexScale", &m_fTexScale, 0.001f);
	ImGui::DragFloat2("m_Texel", (float*)&m_vTexel, 0.001f);
	ImGui::DragFloat2("m_Texel2", (float*)&m_vTexel2, 0.001f);
	ImGui::DragFloat2("m_fCameraDist", (float*)&m_fCameraDist, 0.001f);

	ImGui::Text("HDR");
	ImGui::DragFloat("MiddleGrey", &m_fMiddleGrey, 0.001f);
	ImGui::DragFloat("LumWhiteSqr", &m_fLumWhiteSqr, 0.001f);
	ImGui::DragFloat("BloomScale", &m_fBloomScale, 0.001f);

	ImGui::Text("Bloom");
	ImGui::DragFloat("m_fBloomThreshold", &m_fBloomThreshold, 0.001f);

	ImGui::Text("Oren-Nayar");
	ImGui::Checkbox("OnOff", &m_bOnOff);
	ImGui::DragFloat("m_fAlbedo", &m_fAlbedo, 0.001f);
	ImGui::DragFloat("m_fRoughness", &m_fRoughness, 0.001f);

	ImGui::Text("Cook-Torrance");
	ImGui::Checkbox("OnOff2", &m_bOnOff2);
	ImGui::DragFloat("m_fMetalness", &m_fMetalness, 0.001f);

	ImGui::Text("Glow");
	ImGui::DragFloat2("m_fGlowPower", (float*)&m_fGlowPower, 0.001f);

	ImGui::Text("LumaSharpen");
	ImGui::DragFloat("Sharp_Strength", &m_fSharp_Strength, 0.001f);
	ImGui::DragFloat("Sharp_Clamp", &m_fSharp_Clamp, 0.001f);
	ImGui::DragInt("Pattern", &m_iPattern);
	ImGui::DragFloat("Offset_bias", &m_iOffset_bias, 1.f);
	ImGui::DragFloat2("Offset", (float*)&m_fOffset, 0.001f);
	ImGui::Checkbox("Show_Sharpen", &m_bShow_Sharpen);

	ImGui::Text("HSBC");
	ImGui::DragFloat("Hue", &m_fHue, 0.001f);
	ImGui::DragFloat("Saturation", &m_fSaturation, 0.001f);
	ImGui::DragFloat("Brightness", &m_fBrightness, 0.001f);
	ImGui::DragFloat("Contrast", &m_fContrast, 0.001f);
	ImGui::Checkbox("Show_HSBC", &m_bShow_HSBC);

	ImGui::Separator();
	ImGui::PopID();
}

const HRESULT CCompute::Shadow(void) {
	ID3D11ShaderResourceView* pShadow0SRV = m_pShadow0->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pShadow1SRV = m_pShadow1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pShadow1UAV = m_pShadow1->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pShadow2SRV = m_pShadow2->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pShadow2UAV = m_pShadow2->Get_UnorderedAccessView();

	//DownScale
	HRESULT hr = S_OK;
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow0SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow1UAV);
	hr |= m_pShader_Blur->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow1SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow2UAV);
	hr |= m_pShader_Blur->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(m_fWinSize.y / 4.f)), 1);	//x 모르겠음
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow2SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow1UAV);
	hr |= m_pShader_Blur->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / (128.f - 12.f))), 1);	//y 모르겠음
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Blend(void) {
	ID3D11ShaderResourceView* pDiffuseSRV = m_pDiffuse->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pShadeSRV = m_pShade->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pSpecularSRV = m_pSpecular->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pWorldSRV = m_pWorld->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pShadowSRV = m_pShadow->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pShadow_BakeSRV = m_pShadow_Bake->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBlendUAV = m_pBlend->Get_UnorderedAccessView();
	m_pShadow0->Clear_RenderTargetView();
	ID3D11UnorderedAccessView* m_pShadow0UAV = m_pShadow0->Get_UnorderedAccessView();

	_matrix	ViewMatrix = XMMatrixIdentity();
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CObject* pObject = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow"));
	if (nullptr != pObject) {
		CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Transform")));
		ViewMatrix = pTransform->Get_Matrix_Inverse();
	}
	_matrix	ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 800.f / 600.f, 0.2f, 1000.f);

	HRESULT hr = S_OK;
	hr |= m_pShader_Blend->Set_Resource("g_InputDiffuseTexture", pDiffuseSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputShadeTexture", pShadeSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputSpecularTexture", pSpecularSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputWorldTexture", pWorldSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputShadowTexture", pShadowSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputShadowBakeTexture", pShadow_BakeSRV);

	hr |= m_pShader_Blend->Set_Resource("g_OutputBlendTexture", pBlendUAV);
	hr |= m_pShader_Blend->Set_Resource("g_OutputShadowTexture", m_pShadow0UAV);

	hr |= m_pShader_Blend->Set_RawValue("g_vShadowColor", &m_vShadowColor, sizeof(_float4));
	hr |= m_pShader_Blend->Set_RawValue("g_ShadowViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_float4x4));
	hr |= m_pShader_Blend->Set_RawValue("g_ShadowProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_float4x4));

	hr |= m_pShader_Blend->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Back(void) {
	ID3D11ShaderResourceView* pBlendSRV = m_pBlend->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pShadow1SRV = m_pShadow1->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pSSAO0SRV = m_pSSAO0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBackUAV = m_pBack->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Back->Set_RawValue("g_fWinSize", &m_fWinSize, sizeof(_float2));
	hr |= m_pShader_Back->Set_Resource("g_InputBlendTexture", pBlendSRV);
	hr |= m_pShader_Back->Set_Resource("g_InputShadowTexture", pShadow1SRV);
	hr |= m_pShader_Back->Set_Resource("g_InputSSAOTexture", pSSAO0SRV);
	hr |= m_pShader_Back->Set_Resource("g_OutputBackTexture", pBackUAV);

	hr |= m_pShader_Back->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Distortion(void) {
	ID3D11ShaderResourceView* pDistortionSRV = m_pDistortion->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pBack0SRV = m_pBack0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pDistortion0UAV = m_pDistortion0->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Distortion->Set_Resource("g_InputDistortionTexture", pDistortionSRV);
	hr |= m_pShader_Distortion->Set_Resource("g_InputBackTexture", pBack0SRV);
	hr |= m_pShader_Distortion->Set_Resource("g_OutputTexture", pDistortion0UAV);
	hr |= m_pShader_Distortion->Set_RawValue("g_fDistortion", &m_fDistortion, sizeof(_float));
	hr |= m_pShader_Distortion->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Glow(void) {
	ID3D11ShaderResourceView* pGlowSRV = m_pGlow->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pGlow0SRV = m_pGlow0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pGlow0UAV = m_pGlow0->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pGlow1SRV = m_pGlow1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pGlow1UAV = m_pGlow1->Get_UnorderedAccessView();
	ID3D11UnorderedAccessView* pGlow2UAV = m_pGlow2->Get_UnorderedAccessView();

	CTarget* pGlow_Buffer = m_pTarget_Mgr->Get_Target(TEXT("Glow_Buffer"));
	CTarget* pGlow_Buffer0 = m_pTarget_Mgr->Get_Target(TEXT("Glow_Buffer0"));
	CTarget* pGlow_Buffer1 = m_pTarget_Mgr->Get_Target(TEXT("Glow_Buffer1"));
	ID3D11ShaderResourceView* pGlow_BufferSRV = pGlow_Buffer->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pGlow_Buffer0SRV = pGlow_Buffer0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pGlow_Buffer0UAV = pGlow_Buffer0->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pGlow_Buffer1SRV = pGlow_Buffer1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pGlow_Buffer1UAV = pGlow_Buffer1->Get_UnorderedAccessView();

	//DownScale
	HRESULT hr = S_OK;
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlowSRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow0UAV);
	hr |= m_pShader_Glow->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlow0SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow1UAV);
	hr |= m_pShader_Glow->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(m_fWinSize.y / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlow1SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow0UAV);
	hr |= m_pShader_Glow->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	//------------------------------------------------------------------------------------------------
	//DownScale
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlow_BufferSRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow_Buffer0UAV);
	hr |= m_pShader_Glow->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlow_Buffer0SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow_Buffer1UAV);
	hr |= m_pShader_Glow->Apply(4);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(m_fWinSize.y / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlow_Buffer1SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow_Buffer0UAV);
	hr |= m_pShader_Glow->Apply(5);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);


	hr |= m_pShader_Glow->Set_RawValue("g_fGlowPower", &m_fGlowPower, sizeof(_float2));
	hr |= m_pShader_Glow->Set_RawValue("g_fWinSize", &m_fWinSize, sizeof(_float2));
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlowSRV);
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture2", pGlow0SRV);
	hr |= m_pShader_Glow->Set_Resource("g_InputBufferTexture", pGlow_BufferSRV);
	hr |= m_pShader_Glow->Set_Resource("g_InputBufferTexture2", pGlow_Buffer0SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow2UAV);
	hr |= m_pShader_Glow->Apply(3);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::DOF(void) {
	ID3D11ShaderResourceView* pBack0SRV = m_pBack0->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pDOF0SAV = m_pDOF0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pDOF0UAV = m_pDOF0->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pDOF1SAV = m_pDOF1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pDOF1UAV = m_pDOF1->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pWorldSAV = m_pWorld->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pDOF2UAV = m_pDOF2->Get_UnorderedAccessView();

	//DownScale
	HRESULT hr = S_OK;
	hr |= m_pShader_DOF->Set_Resource("g_InputTexture", pBack0SRV);
	hr |= m_pShader_DOF->Set_Resource("g_OutputTexture", pDOF0UAV);
	hr |= m_pShader_DOF->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_DOF->Set_Resource("g_InputTexture", pDOF0SAV);
	hr |= m_pShader_DOF->Set_Resource("g_OutputTexture", pDOF1UAV);
	hr |= m_pShader_DOF->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(320.f / (128.f - 12.f))), static_cast<_uint>(ceil(m_fWinSize.y / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_DOF->Set_Resource("g_InputTexture", pDOF1SAV);
	hr |= m_pShader_DOF->Set_Resource("g_OutputTexture", pDOF0UAV);
	hr |= m_pShader_DOF->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f)), static_cast<_uint>(ceil(180.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//DOF
	hr |= m_pShader_DOF->Set_RawValue("g_fDepth", &m_fDepth, sizeof(_float));
	hr |= m_pShader_DOF->Set_Resource("g_InputBackTexture", pBack0SRV);
	hr |= m_pShader_DOF->Set_Resource("g_InputBlurTexture", pDOF0SAV);
	hr |= m_pShader_DOF->Set_Resource("g_InputWorldTexture", pWorldSAV);
	hr |= m_pShader_DOF->Set_Resource("g_OutputTexture", pDOF2UAV);
	hr |= m_pShader_DOF->Apply(3);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}


const HRESULT CCompute::HDR(void) {
	ID3D11ShaderResourceView* pFog1SRV = m_pFog1->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pHDR0SAV = m_pHDR0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pHDR0UAV = m_pHDR0->Get_UnorderedAccessView();
	ID3D11UnorderedAccessView* pHDR1UAV = m_pHDR1->Get_UnorderedAccessView();

	_uint vRes[2] = { static_cast<_uint>(m_fWinSize.x / 4), static_cast<_uint>(m_fWinSize.y / 4) };
	HRESULT hr = S_OK;
	hr |= m_pShader_HDR->Set_RawValue("g_vRes", vRes, sizeof(_uint) * 2);
	_uint iDomain = vRes[0] * vRes[1];
	hr |= m_pShader_HDR->Set_RawValue("g_iDomain", &iDomain, sizeof(_uint));
	hr |= m_pShader_HDR->Set_Resource("g_InputTexture", pFog1SRV);
	hr |= m_pShader_HDR->Set_Resource("g_OutputAvgLum", pHDR0UAV);
	m_pShader_HDR->Apply(0);
	m_pContext->Dispatch((UINT)ceil((_float)(m_fWinSize.x / 4.f * m_fWinSize.y / 4.f) / 1024.f), 1, 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Second
	_uint iGroupSize = (UINT)ceil((_float)(m_fWinSize.x / 4.f * m_fWinSize.y / 4.f) / 1024.f);
	hr |= m_pShader_HDR->Set_RawValue("g_iGroupSize", &iGroupSize, sizeof(_uint));
	hr |= m_pShader_HDR->Set_Resource("g_InputAvgLum", pHDR0SAV);
	hr |= m_pShader_HDR->Set_Resource("g_OutputAvgLum", pHDR1UAV);
	m_pShader_HDR->Apply(1);
	m_pContext->Dispatch(1, 1, 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Back1(void) {
	CTarget* pHDR1 = m_pTarget_Mgr->Get_Target(TEXT("HDR1"));
	ID3D11ShaderResourceView* pHDR1SAV = pHDR1->Get_ShaderResourceView();
	CTarget* pBack = m_pTarget_Mgr->Get_Target(TEXT("Fog1"));
	ID3D11ShaderResourceView* pBackSAV = pBack->Get_ShaderResourceView();
	CTarget* pBack1 = m_pTarget_Mgr->Get_Target(TEXT("Back1"));
	ID3D11UnorderedAccessView* pBack1UAV = pBack1->Get_UnorderedAccessView();

	HRESULT hr = S_OK;

	hr |= m_pShader_Back->Set_RawValue("MiddleGrey", &m_fMiddleGrey, sizeof(_float));
	_float fWhite = m_fLumWhiteSqr;
	fWhite *= m_fMiddleGrey;
	fWhite *= fWhite;
	hr |= m_pShader_Back->Set_RawValue("LumWhiteSqr", &fWhite, sizeof(_float));
	hr |= m_pShader_Back->Set_RawValue("BloomScale", &m_fBloomScale, sizeof(_float));
	hr |= m_pShader_Back->Set_Resource("g_HDR", pHDR1SAV);

	hr |= m_pShader_Back->Set_Resource("g_InputBackTexture", pBackSAV);
	hr |= m_pShader_Back->Set_Resource("g_OutputBackTexture", pBack1UAV);

	hr |= m_pShader_Back->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Back2(void) {
	ID3D11ShaderResourceView* pPenetrateSRV = m_pPenetrate->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pBloom1SRV = m_pBloom1->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pBack1SRV = m_pBack1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBack0UAV = m_pBack0->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Back->Set_RawValue("g_fWinSize", &m_fWinSize, sizeof(_float2));
	hr |= m_pShader_Back->Set_Resource("g_InputPenetrateTexture", pPenetrateSRV);
	hr |= m_pShader_Back->Set_Resource("g_InputBloomTexture", pBloom1SRV);
	hr |= m_pShader_Back->Set_Resource("g_InputBackTexture", pBack1SRV);
	hr |= m_pShader_Back->Set_Resource("g_OutputBackTexture", pBack0UAV);
	m_pShader_Back->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Bloom(void) {
	ID3D11ShaderResourceView* pBack1SRV = m_pBack1->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pHDR0SAV = m_pHDR0->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pBloom0SAV = m_pBloom0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBloom0UAV = m_pBloom0->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pBloom1SAV = m_pBloom1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBloom1UAV = m_pBloom1->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pBloom2SAV = m_pBloom2->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBloom2UAV = m_pBloom2->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Bloom->Set_RawValue("g_fBloomThreshold", &m_fBloomThreshold, sizeof(_float));
	hr |= m_pShader_Bloom->Set_Resource("g_InputTexture", pBack1SRV);
	hr |= m_pShader_Bloom->Set_Resource("AvgLum", pHDR0SAV);
	hr |= m_pShader_Bloom->Set_Resource("g_OutputTexture", pBloom0UAV);
	hr |= m_pShader_Bloom->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//DownScale
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pBloom0SAV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pBloom1UAV);
	hr |= m_pShader_Blur->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pBloom1SAV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pBloom2UAV);
	hr |= m_pShader_Blur->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(m_fWinSize.y / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pBloom2SAV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pBloom1UAV);
	hr |= m_pShader_Blur->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::MotionBlur(void) {
	ID3D11ShaderResourceView* pBack0SRV = m_pBack0->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pWorldSRV = m_pWorld->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pMotionBlurUAV = m_pMotionBlur->Get_UnorderedAccessView();

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera)
		pCamera->Set_RawValue(m_pShader_MotionBlur);

	HRESULT hr = S_OK;
	hr |= m_pShader_MotionBlur->Set_Resource("g_InputTexture0", pBack0SRV);
	hr |= m_pShader_MotionBlur->Set_Resource("g_InputTexture1", pWorldSRV);
	hr |= m_pShader_MotionBlur->Set_Resource("g_OutputTexture", pMotionBlurUAV);

	hr |= m_pShader_MotionBlur->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return S_OK;
}

const HRESULT CCompute::SSAO(void) {
	ID3D11ShaderResourceView* pSSAOSRV = m_pSSAO->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pSSAO0SRV = m_pSSAO0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pSSAO0UAV = m_pSSAO0->Get_UnorderedAccessView();
	ID3D11ShaderResourceView* pSSAO1SRV = m_pSSAO1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pSSAO1UAV = m_pSSAO1->Get_UnorderedAccessView();

	//DownScale
	HRESULT hr = S_OK;
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pSSAOSRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pSSAO0UAV);
	hr |= m_pShader_Blur->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pSSAO0SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pSSAO1UAV);
	hr |= m_pShader_Blur->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(m_fWinSize.y / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pSSAO1SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pSSAO0UAV);
	hr |= m_pShader_Blur->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 4.f)), static_cast<_uint>(ceil(m_fWinSize.y / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Fog0(void) {
	ID3D11ShaderResourceView* pWorldSRV = m_pWorld->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pBack0SRV = m_pBack0->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pDepthSRV = m_pDepth->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pGlow2SRV = m_pGlow2->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pGodRay0SRV = m_pGodRay0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pFog0UAV = m_pFog0->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Fog->Set_Resource("g_InputWorldTexture", pWorldSRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputBackTexture", pBack0SRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputGlowTexture", pGlow2SRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputGodRayTexture", pGodRay0SRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputDepthTexture", pDepthSRV);
	hr |= m_pShader_Fog->Set_Resource("g_OutputTexture", pFog0UAV);

	hr |= m_pShader_Fog->Set_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4));
	hr |= m_pShader_Fog->Set_RawValue("g_fFogStart", &m_fFogStart, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_fFogEnd", &m_fFogEnd, sizeof(_float));

	hr |= m_pShader_Fog->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Fog1(void) {
	m_vTexel.x += 0.1f;
	m_vTexel2.y += 0.1f;
	ID3D11ShaderResourceView* pWorldSRV = m_pWorld->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pFog0SRV = m_pFog0->Get_ShaderResourceView();
	if (m_pFogTexture == nullptr)
		m_pFogTexture = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->Get_Component_Prototype(0, TEXT("Texture_Fog")));
	ID3D11ShaderResourceView* pFogTextureSRV = m_pFogTexture->Get_ResourceView()[0];
	ID3D11UnorderedAccessView* m_pFog1UAV = m_pFog1->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Fog->Set_Resource("g_InputWorldTexture", pWorldSRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputBackTexture", pFog0SRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputFogTexture", pFogTextureSRV);
	hr |= m_pShader_Fog->Set_Resource("g_OutputTexture", m_pFog1UAV);

	hr |= m_pShader_Fog->Set_RawValue("g_vFogColor2", &m_vFogColor2, sizeof(_float4));
	hr |= m_pShader_Fog->Set_RawValue("g_fMaxHeight", &m_fMaxHeight, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_fMinHeight", &m_fMinHeight, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_fTexScale", &m_fTexScale, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_vTexel", &m_vTexel, sizeof(_float2));
	hr |= m_pShader_Fog->Set_RawValue("g_vTexel2", &m_vTexel2, sizeof(_float2));
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera) {
		_matrix CameraMatrix = pCamera->Get_ViewMatrix_Inverse();
		hr |= m_pShader_Fog->Set_RawValue("g_vCameraPos", &pCamera->Get_Pos(), sizeof(_float3));
		hr |= m_pShader_Fog->Set_RawValue("g_fCameraDist", &m_fCameraDist, sizeof(_float2));
	}

	hr |= m_pShader_Fog->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Light(void) {
	ID3D11ShaderResourceView* pDiffuseSRV = m_pDiffuse->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pWorldSRV = m_pWorld->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pNormalSRV = m_pNormal->Get_ShaderResourceView();
	ID3D11ShaderResourceView* pMaterialSRV = m_pMaterial->Get_ShaderResourceView();

	ID3D11UnorderedAccessView* pShadeUAV = m_pShade->Get_UnorderedAccessView();
	ID3D11UnorderedAccessView* pSpecularUAV = m_pSpecular->Get_UnorderedAccessView();
	m_pShade->Clear_RenderTargetView();
	m_pSpecular->Clear_RenderTargetView();

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<CObject*>* plist = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_Scene(), TEXT("Reflect"));

	_float fDist = 9999.f;
	CEnviro* pEnviro = nullptr;
	if (nullptr != pCamera) {
		_vector vPos = pCamera->Get_Pos();
		if (nullptr != plist) {
			for (auto iter : *plist) {
				CEnviro* pEnviro2 = dynamic_cast<CEnviro*>(iter);
				CTransform* pEnviro2Transform = dynamic_cast<CTransform*>(pEnviro2->Get_Component(TEXT("Transform")));
				_float fDist2 = XMVectorGetX(XMVector3Length(vPos - pEnviro2Transform->Get_Row(CTransform::ERow::POS)));
				if (fDist > fDist2) {
					fDist = fDist2;
					pEnviro = pEnviro2;
				}
			}
		}
	}
	ID3D11ShaderResourceView* pCubeSRV = nullptr;
	if (nullptr != pEnviro)
		pCubeSRV = pEnviro->Get_CubeSRV();

	HRESULT hr = S_OK;
	hr |= m_pShader_Light->Set_Resource("g_DiffuseTexture", pDiffuseSRV);
	hr |= m_pShader_Light->Set_Resource("g_WorldTexture", pWorldSRV);
	hr |= m_pShader_Light->Set_Resource("g_NormalTexture", pNormalSRV);
	hr |= m_pShader_Light->Set_Resource("g_MaterialTexture", pMaterialSRV);
	hr |= m_pShader_Light->Set_Resource("g_CubeTexture", pCubeSRV);

	hr |= m_pShader_Light->Set_Resource("g_ShadeTexture", pShadeUAV);
	hr |= m_pShader_Light->Set_Resource("g_SpecularTexture", pSpecularUAV);

	hr |= m_pShader_Light->Set_RawValue("g_bOnOff", &m_bOnOff, sizeof(_bool));
	hr |= m_pShader_Light->Set_RawValue("g_bOnOff2", &m_bOnOff2, sizeof(_bool));
	_float m_fPI = static_cast<_float>(M_PI);
	hr |= m_pShader_Light->Set_RawValue("g_fPI", &m_fPI, sizeof(_float));
	hr |= m_pShader_Light->Set_RawValue("g_fAlbedo", &m_fAlbedo, sizeof(_float));
	hr |= m_pShader_Light->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
	hr |= m_pShader_Light->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));

	if (nullptr != pCamera) {
		hr |= m_pShader_Light->Set_RawValue("g_vCameraPos", &pCamera->Get_Pos(), sizeof(_float3));
		vector<CLight*> vecLight = CLight::Get_VecLight();
		for (auto& iter : vecLight) {
			if (false == iter->Get_OnOff())
				continue;
			iter->Set_RawValue(m_pShader_Light);
			switch (iter->Get_Type()) {
			case CLight::EType::DIR:
				hr |= m_pShader_Light->Apply(0);
				m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
				break;
			case CLight::EType::POINT:
				hr |= m_pShader_Light->Apply(1);
				m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
				break;
			case CLight::EType::SPOT:
				hr |= m_pShader_Light->Apply(2);
				m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
				break;
			}
		}
	}

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Shadow_Enviro(void) {
	CTarget* pShadow0 = m_pTarget_Mgr->Get_Target(TEXT("Shadow0_Enviro"));
	ID3D11ShaderResourceView* pShadow0SRV = pShadow0->Get_ShaderResourceView();

	CTarget* pShadow1 = m_pTarget_Mgr->Get_Target(TEXT("Shadow1_Enviro"));
	ID3D11ShaderResourceView* pShadow1SRV = pShadow1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pShadow1UAV = pShadow1->Get_UnorderedAccessView();

	CTarget* pShadow2 = m_pTarget_Mgr->Get_Target(TEXT("Shadow2_Enviro"));
	ID3D11ShaderResourceView* pShadow2SRV = pShadow2->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pShadow2UAV = pShadow2->Get_UnorderedAccessView();

	//DownScale
	HRESULT hr = S_OK;
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow0SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow1UAV);
	hr |= m_pShader_Blur->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / 32.f)), static_cast<_uint>(ceil(512 / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow1SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow2UAV);
	hr |= m_pShader_Blur->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(512 / 4.f)), 1);	//x 모르겠음
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow2SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow1UAV);
	hr |= m_pShader_Blur->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f)), static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), 1);	//y 모르겠음
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Blend_Enviro(void) {
	CTarget* pDiffuse = m_pTarget_Mgr->Get_Target(TEXT("Diffuse_Enviro"));
	ID3D11ShaderResourceView* pDiffuseSRV = pDiffuse->Get_ShaderResourceView();
	CTarget* pShade = m_pTarget_Mgr->Get_Target(TEXT("Shade_Enviro"));
	ID3D11ShaderResourceView* pShadeSRV = pShade->Get_ShaderResourceView();
	CTarget* pSpecular = m_pTarget_Mgr->Get_Target(TEXT("Specular_Enviro"));
	ID3D11ShaderResourceView* pSpecularSRV = pSpecular->Get_ShaderResourceView();
	CTarget* pWorld = m_pTarget_Mgr->Get_Target(TEXT("World_Enviro"));
	ID3D11ShaderResourceView* pWorldSRV = pWorld->Get_ShaderResourceView();
	CTarget* pShadow = m_pTarget_Mgr->Get_Target(TEXT("Shadow_Enviro"));
	ID3D11ShaderResourceView* pShadowSRV = pShadow->Get_ShaderResourceView();
	CTarget* pShadow_Bake = m_pTarget_Mgr->Get_Target(TEXT("Shadow_Bake_Enviro"));
	ID3D11ShaderResourceView* pShadow_BakeSRV = pShadow_Bake->Get_ShaderResourceView();

	CTarget* pBlend = m_pTarget_Mgr->Get_Target(TEXT("Blend_Enviro"));
	ID3D11UnorderedAccessView* pBlendUAV = pBlend->Get_UnorderedAccessView();
	CTarget* pShadow_Blur = m_pTarget_Mgr->Get_Target(TEXT("Shadow0_Enviro"));
	pShadow_Blur->Clear_RenderTargetView();
	ID3D11UnorderedAccessView* pShadow_BlurUAV = pShadow_Blur->Get_UnorderedAccessView();

	_matrix	ViewMatrix = XMMatrixIdentity();
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CObject* pObject = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow"));
	if (nullptr != pObject) {
		CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Transform")));
		ViewMatrix = pTransform->Get_Matrix_Inverse();
	}
	_matrix	ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 800.f / 600.f, 0.2f, 1000.f);

	HRESULT hr = S_OK;
	hr |= m_pShader_Blend->Set_Resource("g_InputDiffuseTexture", pDiffuseSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputShadeTexture", pShadeSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputSpecularTexture", pSpecularSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputWorldTexture", pWorldSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputShadowTexture", pShadowSRV);
	hr |= m_pShader_Blend->Set_Resource("g_InputShadowBakeTexture", pShadow_BakeSRV);

	hr |= m_pShader_Blend->Set_Resource("g_OutputBlendTexture", pBlendUAV);
	hr |= m_pShader_Blend->Set_Resource("g_OutputShadowTexture", pShadow_BlurUAV);

	hr |= m_pShader_Blend->Set_RawValue("g_ShadowViewMatrix", &XMMatrixTranspose(ViewMatrix), sizeof(_float4x4));
	hr |= m_pShader_Blend->Set_RawValue("g_ShadowProjMatrix", &XMMatrixTranspose(ProjMatrix), sizeof(_float4x4));

	hr |= m_pShader_Blend->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Back_Enviro(void) {
	CTarget* pBlend = m_pTarget_Mgr->Get_Target(TEXT("Blend_Enviro"));
	ID3D11ShaderResourceView* pBlendSRV = pBlend->Get_ShaderResourceView();
	CTarget* pShadow1 = m_pTarget_Mgr->Get_Target(TEXT("Shadow1_Enviro"));
	ID3D11ShaderResourceView* pShadow1SRV = pShadow1->Get_ShaderResourceView();
	CTarget* pSSAO0 = m_pTarget_Mgr->Get_Target(TEXT("SSAO0_Enviro"));
	ID3D11ShaderResourceView* pSSAO0SRV = pSSAO0->Get_ShaderResourceView();
	CTarget* pBack = m_pTarget_Mgr->Get_Target(TEXT("Back0_Enviro"));
	ID3D11UnorderedAccessView* pBackUAV = pBack->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	_float2 fWinSize = { 512.f, 512.f };
	hr |= m_pShader_Back->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
	hr |= m_pShader_Back->Set_Resource("g_InputBlendTexture", pBlendSRV);
	hr |= m_pShader_Back->Set_Resource("g_InputShadowTexture", pShadow1SRV);
	hr |= m_pShader_Back->Set_Resource("g_InputSSAOTexture", pSSAO0SRV);
	hr |= m_pShader_Back->Set_Resource("g_OutputBackTexture", pBackUAV);

	hr |= m_pShader_Back->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Distortion_Enviro(void) {
	CTarget* pDistortion = m_pTarget_Mgr->Get_Target(TEXT("Distortion_Enviro"));
	ID3D11ShaderResourceView* pDistortionSRV = pDistortion->Get_ShaderResourceView();
	CTarget* pBack0 = m_pTarget_Mgr->Get_Target(TEXT("Back0_Enviro"));
	ID3D11ShaderResourceView* pBack0SRV = pBack0->Get_ShaderResourceView();
	CTarget* pDistortion0 = m_pTarget_Mgr->Get_Target(TEXT("Distortion0_Enviro"));
	ID3D11UnorderedAccessView* pDistortion0UAV = pDistortion0->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Distortion->Set_Resource("g_InputDistortionTexture", pDistortionSRV);
	hr |= m_pShader_Distortion->Set_Resource("g_InputBackTexture", pBack0SRV);
	hr |= m_pShader_Distortion->Set_Resource("g_OutputTexture", pDistortion0UAV);
	hr |= m_pShader_Distortion->Set_RawValue("g_fDistortion", &m_fDistortion, sizeof(_float));
	hr |= m_pShader_Distortion->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Glow_Enviro(void) {
	CTarget* pGlow = m_pTarget_Mgr->Get_Target(TEXT("Glow_Enviro"));
	ID3D11ShaderResourceView* pGlowSRV = pGlow->Get_ShaderResourceView();
	CTarget* pGlow0 = m_pTarget_Mgr->Get_Target(TEXT("Glow0_Enviro"));
	ID3D11ShaderResourceView* pGlow0SRV = pGlow0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pGlow0UAV = pGlow0->Get_UnorderedAccessView();
	CTarget* pGlow1 = m_pTarget_Mgr->Get_Target(TEXT("Glow1_Enviro"));
	ID3D11ShaderResourceView* pGlow1SRV = pGlow1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pGlow1UAV = pGlow1->Get_UnorderedAccessView();
	CTarget* pGlow2 = m_pTarget_Mgr->Get_Target(TEXT("Glow2_Enviro"));
	ID3D11UnorderedAccessView* pGlow2UAV = pGlow2->Get_UnorderedAccessView();

	//DownScale
	HRESULT hr = S_OK;
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlowSRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow0UAV);
	hr |= m_pShader_Glow->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / 32.f)), static_cast<_uint>(ceil(512 / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlow0SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow1UAV);
	hr |= m_pShader_Glow->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(512 / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlow1SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow0UAV);
	hr |= m_pShader_Glow->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f)), static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	_float2 fWinSize = { 512.f, 512.f };
	hr |= m_pShader_Glow->Set_RawValue("g_fGlowPower", &m_fGlowPower, sizeof(_float2));
	hr |= m_pShader_Glow->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture", pGlowSRV);
	hr |= m_pShader_Glow->Set_Resource("g_InputTexture2", pGlow0SRV);
	hr |= m_pShader_Glow->Set_Resource("g_OutputTexture", pGlow2UAV);
	hr |= m_pShader_Glow->Apply(3);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::DOF_Enviro(void) {
	return S_OK;
}

const HRESULT CCompute::HDR_Enviro(void) {
	CTarget* pBack = m_pTarget_Mgr->Get_Target(TEXT("Fog1_Enviro"));
	ID3D11ShaderResourceView* pBackSRV = pBack->Get_ShaderResourceView();
	CTarget* pHDR0 = m_pTarget_Mgr->Get_Target(TEXT("HDR0_Enviro"));
	ID3D11ShaderResourceView* pHDR0SAV = pHDR0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pHDR0UAV = pHDR0->Get_UnorderedAccessView();
	CTarget* pHDR1 = m_pTarget_Mgr->Get_Target(TEXT("HDR1_Enviro"));
	ID3D11UnorderedAccessView* pHDR1UAV = pHDR1->Get_UnorderedAccessView();

	_uint vRes[2] = { static_cast<_uint>(512 / 4), static_cast<_uint>(512 / 4) };
	HRESULT hr = S_OK;
	hr |= m_pShader_HDR->Set_RawValue("g_vRes", vRes, sizeof(_uint) * 2);
	_uint iDomain = vRes[0] * vRes[1];
	hr |= m_pShader_HDR->Set_RawValue("g_iDomain", &iDomain, sizeof(_uint));
	hr |= m_pShader_HDR->Set_Resource("g_InputTexture", pBackSRV);
	hr |= m_pShader_HDR->Set_Resource("g_OutputAvgLum", pHDR0UAV);
	m_pShader_HDR->Apply(0);
	m_pContext->Dispatch((UINT)ceil((_float)(512 / 4.f * 512 / 4.f) / 1024.f), 1, 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Second
	_uint iGroupSize = (UINT)ceil((_float)(512 / 4.f * 512 / 4.f) / 1024.f);
	hr |= m_pShader_HDR->Set_RawValue("g_iGroupSize", &iGroupSize, sizeof(_uint));
	hr |= m_pShader_HDR->Set_Resource("g_InputAvgLum", pHDR0SAV);
	hr |= m_pShader_HDR->Set_Resource("g_OutputAvgLum", pHDR1UAV);
	m_pShader_HDR->Apply(1);
	m_pContext->Dispatch(1, 1, 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Back1_Enviro(void) {
	CTarget* pHDR1 = m_pTarget_Mgr->Get_Target(TEXT("HDR1_Enviro"));
	ID3D11ShaderResourceView* pHDR1SAV = pHDR1->Get_ShaderResourceView();
	CTarget* pBack = m_pTarget_Mgr->Get_Target(TEXT("Fog1_Enviro"));
	ID3D11ShaderResourceView* pBackSAV = pBack->Get_ShaderResourceView();
	CTarget* pBack1 = m_pTarget_Mgr->Get_Target(TEXT("Back1_Enviro"));
	ID3D11UnorderedAccessView* pBack1UAV = pBack1->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Back->Set_RawValue("MiddleGrey", &m_fMiddleGrey, sizeof(_float));
	hr |= m_pShader_Back->Set_RawValue("LumWhiteSqr", &m_fLumWhiteSqr, sizeof(_float));
	hr |= m_pShader_Back->Set_RawValue("BloomScale", &m_fBloomScale, sizeof(_float));
	hr |= m_pShader_Back->Set_Resource("g_HDR", pHDR1SAV);

	hr |= m_pShader_Back->Set_Resource("g_InputBackTexture", pBackSAV);
	hr |= m_pShader_Back->Set_Resource("g_OutputBackTexture", pBack1UAV);

	hr |= m_pShader_Back->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Back2_Enviro(void) {
	CTarget* pPenetrate = m_pTarget_Mgr->Get_Target(TEXT("Penetrate_Enviro"));
	ID3D11ShaderResourceView* pPenetrateSRV = pPenetrate->Get_ShaderResourceView();
	CTarget* pBloom = m_pTarget_Mgr->Get_Target(TEXT("Bloom1_Enviro"));
	ID3D11ShaderResourceView* pBloomSRV = pBloom->Get_ShaderResourceView();
	CTarget* pBack1 = m_pTarget_Mgr->Get_Target(TEXT("Back1_Enviro"));
	ID3D11ShaderResourceView* pBack1SRV = pBack1->Get_ShaderResourceView();
	CTarget* pBack0 = m_pTarget_Mgr->Get_Target(TEXT("Back0_Enviro"));
	ID3D11UnorderedAccessView* pBack0UAV = pBack0->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	_float2 fWinSize = { 512.f, 512.f };
	hr |= m_pShader_Back->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
	hr |= m_pShader_Back->Set_Resource("g_InputPenetrateTexture", pPenetrateSRV);
	hr |= m_pShader_Back->Set_Resource("g_InputBloomTexture", pBloomSRV);
	hr |= m_pShader_Back->Set_Resource("g_InputBackTexture", pBack1SRV);
	hr |= m_pShader_Back->Set_Resource("g_OutputBackTexture", pBack0UAV);
	m_pShader_Back->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Bloom_Enviro(void) {
	CTarget* pBack = m_pTarget_Mgr->Get_Target(TEXT("Back1_Enviro"));
	ID3D11ShaderResourceView* pBackSRV = pBack->Get_ShaderResourceView();
	CTarget* pHDR0 = m_pTarget_Mgr->Get_Target(TEXT("HDR0_Enviro"));
	ID3D11ShaderResourceView* pHDR0SAV = pHDR0->Get_ShaderResourceView();
	CTarget* pBloom0 = m_pTarget_Mgr->Get_Target(TEXT("Bloom0_Enviro"));
	ID3D11ShaderResourceView* pBloom0SAV = pBloom0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBloom0UAV = pBloom0->Get_UnorderedAccessView();
	CTarget* pBloom1 = m_pTarget_Mgr->Get_Target(TEXT("Bloom1_Enviro"));
	ID3D11ShaderResourceView* pBloom1SAV = pBloom1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBloom1UAV = pBloom1->Get_UnorderedAccessView();
	CTarget* pBloom2 = m_pTarget_Mgr->Get_Target(TEXT("Bloom2_Enviro"));
	ID3D11ShaderResourceView* pBloom2SAV = pBloom2->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pBloom2UAV = pBloom2->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Bloom->Set_RawValue("g_fBloomThreshold", &m_fBloomThreshold, sizeof(_float));
	hr |= m_pShader_Bloom->Set_Resource("g_InputTexture", pBackSRV);
	hr |= m_pShader_Bloom->Set_Resource("AvgLum", pHDR0SAV);
	hr |= m_pShader_Bloom->Set_Resource("g_OutputTexture", pBloom0UAV);
	hr |= m_pShader_Bloom->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//DownScale
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pBloom0SAV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pBloom1UAV);
	hr |= m_pShader_Blur->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / 32.f)), static_cast<_uint>(ceil(512 / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pBloom1SAV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pBloom2UAV);
	hr |= m_pShader_Blur->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(512 / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pBloom2SAV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pBloom1UAV);
	hr |= m_pShader_Blur->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f)), static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::MotionBlur_Enviro(void) {
	return S_OK;
}

const HRESULT CCompute::SSAO_Enviro(void) {
	CTarget* pSSAO = m_pTarget_Mgr->Get_Target(TEXT("SSAO_Enviro"));
	ID3D11ShaderResourceView* pSSAOSRV = pSSAO->Get_ShaderResourceView();

	CTarget* pShadow1 = m_pTarget_Mgr->Get_Target(TEXT("SSAO0_Enviro"));
	ID3D11ShaderResourceView* pShadow1SRV = pShadow1->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pShadow1UAV = pShadow1->Get_UnorderedAccessView();

	CTarget* pShadow2 = m_pTarget_Mgr->Get_Target(TEXT("SSAO1_Enviro"));
	ID3D11ShaderResourceView* pShadow2SRV = pShadow2->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pShadow2UAV = pShadow2->Get_UnorderedAccessView();

	//DownScale
	HRESULT hr = S_OK;
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pSSAOSRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow1UAV);
	hr |= m_pShader_Blur->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / 32.f)), static_cast<_uint>(ceil(512 / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Horizon
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow1SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow2UAV);
	hr |= m_pShader_Blur->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), static_cast<_uint>(ceil(512 / 4.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	//Vertical
	hr |= m_pShader_Blur->Set_Resource("g_InputTexture", pShadow2SRV);
	hr |= m_pShader_Blur->Set_Resource("g_OutputTexture", pShadow1UAV);
	hr |= m_pShader_Blur->Apply(2);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 4.f)), static_cast<_uint>(ceil(512 / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Fog0_Enviro(void) {
	CTarget* pWorld = m_pTarget_Mgr->Get_Target(TEXT("World_Enviro"));
	ID3D11ShaderResourceView* pWorldSRV = pWorld->Get_ShaderResourceView();
	CTarget* pBack0 = m_pTarget_Mgr->Get_Target(TEXT("Back0_Enviro"));
	ID3D11ShaderResourceView* pBack0SRV = pBack0->Get_ShaderResourceView();
	CTarget* pDepth = m_pTarget_Mgr->Get_Target(TEXT("Depth_Enviro"));
	ID3D11ShaderResourceView* pDepthSRV = pDepth->Get_ShaderResourceView();
	CTarget* pGlow = m_pTarget_Mgr->Get_Target(TEXT("Glow2_Enviro"));
	ID3D11ShaderResourceView* pGlowSRV = pGlow->Get_ShaderResourceView();
	CTarget* pGodRay0 = m_pTarget_Mgr->Get_Target(TEXT("GodRay0_Enviro"));
	ID3D11ShaderResourceView* pGodRay0SRV = pGodRay0->Get_ShaderResourceView();
	CTarget* Fog0 = m_pTarget_Mgr->Get_Target(TEXT("Fog0_Enviro"));
	ID3D11UnorderedAccessView* Fog0UAV = Fog0->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Fog->Set_Resource("g_InputWorldTexture", pWorldSRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputBackTexture", pBack0SRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputGlowTexture", pGlowSRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputGodRayTexture", pGodRay0SRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputDepthTexture", pDepthSRV);
	hr |= m_pShader_Fog->Set_Resource("g_OutputTexture", Fog0UAV);

	hr |= m_pShader_Fog->Set_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4));
	hr |= m_pShader_Fog->Set_RawValue("g_fFogStart", &m_fFogStart, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_fFogEnd", &m_fFogEnd, sizeof(_float));

	hr |= m_pShader_Fog->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Fog1_Enviro(void) {
	CTarget* pWorld = m_pTarget_Mgr->Get_Target(TEXT("World_Enviro"));
	ID3D11ShaderResourceView* pWorldSRV = pWorld->Get_ShaderResourceView();
	CTarget* pBack1 = m_pTarget_Mgr->Get_Target(TEXT("Fog0_Enviro"));
	ID3D11ShaderResourceView* pBack1SRV = pBack1->Get_ShaderResourceView();
	CTexture* pFogTexture = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->Get_Component_Prototype(0, TEXT("Texture_Fog")));
	ID3D11ShaderResourceView* pFogTextureSRV = pFogTexture->Get_ResourceView()[0];
	CTarget* pFog = m_pTarget_Mgr->Get_Target(TEXT("Fog1_Enviro"));
	ID3D11UnorderedAccessView* pFogUAV = pFog->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Fog->Set_Resource("g_InputWorldTexture", pWorldSRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputBackTexture", pBack1SRV);
	hr |= m_pShader_Fog->Set_Resource("g_InputFogTexture", pFogTextureSRV);
	hr |= m_pShader_Fog->Set_Resource("g_OutputTexture", pFogUAV);

	hr |= m_pShader_Fog->Set_RawValue("g_vFogColor2", &m_vFogColor2, sizeof(_float4));
	hr |= m_pShader_Fog->Set_RawValue("g_fMaxHeight", &m_fMaxHeight, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_fMinHeight", &m_fMinHeight, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_fTexScale", &m_fTexScale, sizeof(_float));
	hr |= m_pShader_Fog->Set_RawValue("g_vTexel", &m_vTexel, sizeof(_float2));
	hr |= m_pShader_Fog->Set_RawValue("g_vTexel2", &m_vTexel2, sizeof(_float2));
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera) {
		_matrix CameraMatrix = pCamera->Get_ViewMatrix_Inverse();
		hr |= m_pShader_Fog->Set_RawValue("g_vCameraPos", &pCamera->Get_Pos(), sizeof(_float3));
		hr |= m_pShader_Fog->Set_RawValue("g_fCameraDist", &m_fCameraDist, sizeof(_float2));
	}

	hr |= m_pShader_Fog->Apply(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::Light_Enviro(void) {
	CTarget* pDiffuse = m_pTarget_Mgr->Get_Target(TEXT("Diffuse_Enviro"));
	ID3D11ShaderResourceView* pDiffuseSRV = pDiffuse->Get_ShaderResourceView();
	CTarget* pWorld = m_pTarget_Mgr->Get_Target(TEXT("World_Enviro"));
	ID3D11ShaderResourceView* pWorldSRV = pWorld->Get_ShaderResourceView();
	CTarget* pNormal = m_pTarget_Mgr->Get_Target(TEXT("Normal_Enviro"));
	ID3D11ShaderResourceView* pNormalSRV = pNormal->Get_ShaderResourceView();
	CTarget* pMaterial = m_pTarget_Mgr->Get_Target(TEXT("Material_Enviro"));
	ID3D11ShaderResourceView* pMaterialSRV = pMaterial->Get_ShaderResourceView();

	CTarget* pShade = m_pTarget_Mgr->Get_Target(TEXT("Shade_Enviro"));
	ID3D11UnorderedAccessView* pShadeUAV = pShade->Get_UnorderedAccessView();
	CTarget* pSpecular = m_pTarget_Mgr->Get_Target(TEXT("Specular_Enviro"));
	ID3D11UnorderedAccessView* pSpecularUAV = pSpecular->Get_UnorderedAccessView();
	pShade->Clear_RenderTargetView();
	pSpecular->Clear_RenderTargetView();

	HRESULT hr = S_OK;
	hr |= m_pShader_Light->Set_Resource("g_DiffuseTexture", pDiffuseSRV);
	hr |= m_pShader_Light->Set_Resource("g_WorldTexture", pWorldSRV);
	hr |= m_pShader_Light->Set_Resource("g_NormalTexture", pNormalSRV);
	hr |= m_pShader_Light->Set_Resource("g_MaterialTexture", pMaterialSRV);
	hr |= m_pShader_Light->Set_Resource("g_ShadeTexture", pShadeUAV);
	hr |= m_pShader_Light->Set_Resource("g_SpecularTexture", pSpecularUAV);

	hr |= m_pShader_Light->Set_RawValue("g_bOnOff", &m_bOnOff, sizeof(_bool));
	hr |= m_pShader_Light->Set_RawValue("g_bOnOff2", &m_bOnOff2, sizeof(_bool));
	_float m_fPI = static_cast<_float>(M_PI);
	hr |= m_pShader_Light->Set_RawValue("g_fPI", &m_fPI, sizeof(_float));
	hr |= m_pShader_Light->Set_RawValue("g_fAlbedo", &m_fAlbedo, sizeof(_float));
	hr |= m_pShader_Light->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
	hr |= m_pShader_Light->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera) {
		hr |= m_pShader_Light->Set_RawValue("g_vCameraPos", &pCamera->Get_Pos(), sizeof(_float3));
		vector<CLight*> vecLight = CLight::Get_VecLight();
		for (auto& iter : vecLight) {
			if (false == iter->Get_OnOff())
				continue;
			iter->Set_RawValue(m_pShader_Light);
			switch (iter->Get_Type()) {
			case CLight::EType::DIR:
				hr |= m_pShader_Light->Apply(0);
				m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
				break;
			case CLight::EType::POINT:
				hr |= m_pShader_Light->Apply(1);
				m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
				break;
			case CLight::EType::SPOT:
				hr |= m_pShader_Light->Apply(2);
				m_pContext->Dispatch(static_cast<_uint>(ceil(512 / 32.f)), static_cast<_uint>(ceil(512 / 32.f)), 1);
				break;
			}
		}
	}

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);

	return hr;
}

const _float& CCompute::Get_Saturation(void) {
	return m_fSaturation;
}

void CCompute::Set_Saturation(const _float& fSaturation) {
	m_fSaturation = fSaturation;
}

void CCompute::Add_Saturation(const _float& fSaturation) {
	m_fSaturation += fSaturation;
}

const HRESULT CCompute::LumaSharpen(void) {
	ID3D11ShaderResourceView* pBack0SRV = m_pBack0->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pLumaSharpenUAV = m_pLumaSharpen->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_LumaSharpen->Set_Resource("g_InputTexture", pBack0SRV);
	hr |= m_pShader_LumaSharpen->Set_Resource("g_OutputTexture", pLumaSharpenUAV);

	hr |= m_pShader_LumaSharpen->Set_RawValue("sharp_strength", &m_fSharp_Strength, sizeof(_float));
	hr |= m_pShader_LumaSharpen->Set_RawValue("sharp_clamp", &m_fSharp_Clamp, sizeof(_float));
	hr |= m_pShader_LumaSharpen->Set_RawValue("pattern", &m_iPattern, sizeof(_int));
	hr |= m_pShader_LumaSharpen->Set_RawValue("offset_bias", &m_iOffset_bias, sizeof(_float));
	hr |= m_pShader_LumaSharpen->Set_RawValue("g_fOffset", &m_fOffset, sizeof(_float2));
	hr |= m_pShader_LumaSharpen->Set_RawValue("show_sharpen", &m_bShow_Sharpen, sizeof(_bool));

	hr |= m_pShader_LumaSharpen->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}

const HRESULT CCompute::HSBC(void) {
	ID3D11ShaderResourceView* m_pLumaSharpenSRV = m_pLumaSharpen->Get_ShaderResourceView();
	ID3D11UnorderedAccessView* pHSBCUAV = m_pHSBC->Get_UnorderedAccessView();

	HRESULT hr = S_OK;
	hr |= m_pShader_HSBC->Set_Resource("g_InputTexture", m_pLumaSharpenSRV);
	hr |= m_pShader_HSBC->Set_Resource("g_OutputTexture", pHSBCUAV);

	hr |= m_pShader_HSBC->Set_RawValue("g_fHue", &m_fHue, sizeof(_float));
	hr |= m_pShader_HSBC->Set_RawValue("g_fSaturation", &m_fSaturation, sizeof(_float));
	hr |= m_pShader_HSBC->Set_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float));
	hr |= m_pShader_HSBC->Set_RawValue("g_fContrast", &m_fContrast, sizeof(_float));
	hr |= m_pShader_HSBC->Set_RawValue("g_bShow_HSBC", &m_bShow_HSBC, sizeof(_bool));

	hr |= m_pShader_HSBC->Apply(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(m_fWinSize.x / 32.f)), static_cast<_uint>(ceil(m_fWinSize.y / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 7, m_pClearUAV, nullptr);
	return hr;
}
