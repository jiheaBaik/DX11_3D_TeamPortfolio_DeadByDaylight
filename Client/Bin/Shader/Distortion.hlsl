Texture2D g_InputDistortionTexture;
Texture2D g_InputBackTexture;
RWTexture2D<float4> g_OutputTexture;

float g_fDistortion = 1.f;
//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_DISTORTION(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

    float2 vDistortion = g_InputDistortionTexture.Load(int3(vPixel, 0.f)).xy;

    uint2 vPixel2 = vPixel + vDistortion * g_fDistortion;
	g_OutputTexture[vPixel.xy] = g_InputBackTexture.Load(int3(vPixel2, 0.f));
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Distortion {
	pass Pass_Distortion {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DISTORTION();
	}
}