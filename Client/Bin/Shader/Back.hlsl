texture2D g_InputBlendTexture;
texture2D g_InputShadowTexture;
texture2D g_InputSSAOTexture;
texture2D g_InputPenetrateTexture;
texture2D g_InputBloomTexture;
texture2D g_InputBackTexture;
RWTexture2D<float4> g_OutputBackTexture;
StructuredBuffer<float> g_HDR : register(t1);
cbuffer FinalPassConstants : register(b0) {
	float MiddleGrey : packoffset(c0);
	float LumWhiteSqr : packoffset(c0.y);
	float BloomScale : packoffset(c0.z);
}
static const float3 LUM_FACTOR = float3(0.2126, 0.7152, 0.0722);

float2 g_fWinSize;

sampler Sampler_Default = sampler_state {
	Filter = min_mag_mip_linear;
};
//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_BACK0(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
	float2 vUV = float2(vPixel.x / g_fWinSize.x, vPixel.y / g_fWinSize.y);

	float4 vBlend = g_InputBlendTexture.Load(int3(vPixel, 0));
	float4 vShadow = g_InputShadowTexture.SampleLevel(Sampler_Default, vUV, 0);
	float4 vSSAO = g_InputSSAOTexture.SampleLevel(Sampler_Default, vUV, 0);

	if (vBlend.a != 0.f) {
		vBlend *= vShadow;
		vBlend *= vSSAO;
		g_OutputBackTexture[vPixel] = vBlend;
	}
}
float3 ToneMapping(float3 HDRColor) {
	float LScale = dot(HDRColor, LUM_FACTOR);
	LScale *= MiddleGrey / g_HDR[0];
	LScale = (LScale + LScale * LScale / LumWhiteSqr) / (1.f + LScale);
	return HDRColor * LScale;
}
[numthreads(32, 32, 1)]
void CS_BACK1(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
	vBack = vector(ToneMapping(vBack), vBack.w);
	g_OutputBackTexture[vPixel] = vBack;
}
[numthreads(32, 32, 1)]
void CS_BACK2(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
	float2 vUV = float2(vPixel.x / g_fWinSize.x, vPixel.y / g_fWinSize.y);

	float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
	float4 vBloom = g_InputBloomTexture.SampleLevel(Sampler_Default, vUV, 0);
	float4 vPenetrate = g_InputPenetrateTexture.Load(int3(vPixel, 0));
	g_OutputBackTexture[vPixel] = vBack + vBloom + vPenetrate * 0.5f;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Back {
	pass Pass_Back0 {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_BACK0();
	}
	pass Pass_Back1 {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_BACK1();
	}
	pass Pass_Back2 {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_BACK2();
	}
}

