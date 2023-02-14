Texture2D g_InputDiffuseTexture;
Texture2D g_InputShadeTexture;
Texture2D g_InputSpecularTexture;
texture2D g_InputWorldTexture;
texture2D g_InputShadowTexture;
texture2D g_InputShadowBakeTexture;
RWTexture2D<float4> g_OutputBlendTexture;
RWTexture2D<float4> g_OutputShadowTexture;

matrix g_ShadowViewMatrix;
matrix g_ShadowProjMatrix;

float4 g_vShadowColor;
//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_BLEND(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	vector vDiffuse = g_InputDiffuseTexture.Load(int3(vPixel, 0));
	vector vShade = g_InputShadeTexture.Load(int3(vPixel, 0));
	vector vSpecular = g_InputSpecularTexture.Load(int3(vPixel, 0));

	//±×¸²ÀÚ
	vector vWorld = vector(g_InputWorldTexture.Load(int3(vPixel, 0)).xyz, 1.f);
	vWorld = mul(vWorld, g_ShadowViewMatrix);
	vWorld = mul(vWorld, g_ShadowProjMatrix);
	float fDepth = vWorld.z / 1000.f;
	vWorld /= vWorld.w;
	float2 vTexCoord;
	vTexCoord.x = vWorld.x * 0.5f + 0.5f;
	vTexCoord.y = vWorld.y * -0.5f + 0.5f;
	vector vShadow = g_InputShadowTexture.Load(int3(vTexCoord.x * 8000.f, vTexCoord.y * 6000.f, 0));
	vector vShadowBake = g_InputShadowBakeTexture.Load(int3(vTexCoord.x * 8000.f, vTexCoord.y * 6000.f, 0));

	vector vOut = vDiffuse * vShade + vector(vSpecular.xyz, 0.f);
	if (vTexCoord.x >= 0 && vTexCoord.x <= 1.f && vTexCoord.y >= 0 && vTexCoord.y <= 1.f)
		if (fDepth -0.0001f > vShadow.r || fDepth - 0.0001f > vShadowBake.r)
			g_OutputShadowTexture[vPixel] = g_vShadowColor;
	g_OutputBlendTexture[vPixel] = vOut;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Blend {
	pass Pass_Blend {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_BLEND();
	}
}