// Bloom Compute Shader
Texture2D<float4> g_InputTexture : register(t0);
StructuredBuffer<float> AvgLum : register(t1);
RWTexture2D<float4> g_OutputTexture : register(u1);

static const float3 LUM_FACTOR = float3(0.299f, 0.587f, 0.114f);
float g_fBloomThreshold = 0.6f;
//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_BLOOM(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	float4 vColor = g_InputTexture.Load(int3(vPixel, 0));
	float fLum = dot(vColor, LUM_FACTOR);
	float fAvgLum = AvgLum[0];

	float fColorScale = saturate(fLum - fAvgLum * g_fBloomThreshold);

	g_OutputTexture[vPixel.xy] = vColor * fColorScale;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Bloom {
	pass Pass_Bloom {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_BLOOM();
	}
}