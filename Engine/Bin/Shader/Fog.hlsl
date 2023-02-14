
Texture2D g_InputBackTexture;
Texture2D g_InputGlowTexture;
Texture2D g_InputWorldTexture;
Texture2D g_InputDepthTexture;
Texture2D g_InputFogTexture;
Texture2D g_InputGodRayTexture;

RWTexture2D<float4> g_OutputTexture;

float4 g_vFogColor = float4(0.5f, 0.5f, 0.5f, 1.f);
float g_fFogStart = 0.f;
float g_fFogEnd = 1.f;

float4 g_vFogColor2 = float4(1.f, 1.f, 1.f, 0.5f);
float g_fMaxHeight = 10.f;
float g_fMinHeight = 2.f;
float g_fTexScale = 1.f;
float2 g_vTexel;
float2 g_vTexel2;
float3 g_vCameraPos;
float2 g_fCameraDist;

//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_FOG(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0.f));

	//vBack += vGlow;
	float fDepth = g_InputDepthTexture.Load(int3(vPixel, 0.f)).r;

	float fogFactor = saturate((g_fFogEnd - fDepth) / (g_fFogEnd - g_fFogStart));

	g_OutputTexture[vPixel] = vBack * fogFactor + g_vFogColor * (1.f - fogFactor);
}
[numthreads(32, 32, 1)]
void CS_FOGHEIGHT(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	float4 vWorld = g_InputWorldTexture.Load(int3(vPixel, 0.f));
	float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0.f));
	float4 vGlow = g_InputGlowTexture.Load(int3(vPixel, 0.f));
	float4 vGodRay = g_InputGodRayTexture.Load(int3(vPixel, 0.f));

	float2 vWrap = (vWorld.xz + g_vTexel) * g_fTexScale % 256.f;
	float2 vWrap2 = (vWorld.xz + g_vTexel2) * g_fTexScale % 256.f;

	float FogColor = (g_InputFogTexture.Load(int3(vWrap, 0.f)).r +
		g_InputFogTexture.Load(int3(vWrap2, 0.f)).r) * 0.5f;

	float fCameraDist = distance(g_vCameraPos, vWorld.xyz);
	fCameraDist = (fCameraDist - g_fCameraDist.x) / (g_fCameraDist.y - g_fCameraDist.x);
	fCameraDist = saturate(fCameraDist);

	float Alpha = clamp((vWorld.y - g_fMinHeight) / (g_fMaxHeight - g_fMinHeight), 0.f, 1.f);
	Alpha = 1.f - (1.f - Alpha) * FogColor * fCameraDist;

	if (vWorld.w == 1.f)
		g_OutputTexture[vPixel] = vBack * Alpha + g_vFogColor2 * (1.f - Alpha) + vGodRay + vGlow;
	else
		g_OutputTexture[vPixel] = vBack + vGodRay + vGlow;

}
//-------------------------------------------------------------------------------------------------
technique11 Technique_Fog {
	pass Pass_Fog {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_FOG();
	}
	pass Pass_FogHeight {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_FOGHEIGHT();
	}
}