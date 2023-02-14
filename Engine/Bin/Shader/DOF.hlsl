
Texture2D g_InputTexture;
RWTexture2D<float4> g_OutputTexture;

Texture2D g_InputBackTexture;
Texture2D g_InputBlurTexture;
Texture2D g_InputWorldTexture;

float g_fDepth;
//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_DOWNSCALE(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	int3 vFullPos = int3(vPixel * 4, 0);
	float4 vDownScale = float4(0.f, 0.f, 0.f, 0.f);
	[unroll]
	for (int i = 0; i < 4; i++) {
		[unroll]
		for (int j = 0; j < 4; j++)
			vDownScale += g_InputTexture.Load(vFullPos, int2(j, i));
	}
	vDownScale /= 16.f;
	g_OutputTexture[vPixel.xy] = vDownScale;
}
#define kernelhalf 6
#define groupthreads 128
groupshared float4 SharedInput[groupthreads];
static const float SampleWeights[13] = {
	0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216,
};
[numthreads(groupthreads, 1, 1)]
void CS_HORIZON(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex) {
	int2 coord = int2(groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.x, groupID.y);
	//coord = clamp(coord, int2(0, 0), int2(Res.x - 1, Res.y - 1));
	SharedInput[groupIndex] = g_InputTexture.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (groupIndex >= kernelhalf && groupIndex < (groupthreads - kernelhalf) /*&&
		((groupID.x * (groupthreads - 2 * kernelhalf) + groupIndex - kernelhalf) < (uint)Res.x)*/) {
		float4 vOut = 0;

		[unroll]
		for (int i = -kernelhalf; i <= kernelhalf; ++i)
			vOut += SharedInput[groupIndex + i] * SampleWeights[i + kernelhalf];

		g_OutputTexture[coord] = vOut;
	}
}
[numthreads(groupthreads, 1, 1)]
void CS_VERTICAL(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex) {
	int2 coord = int2(groupID.x, groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.y);
	//coord = clamp(coord, int2(0, 0), int2(Res.x - 1, Res.y - 1));
	SharedInput[groupIndex] = g_InputTexture.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (groupIndex >= kernelhalf && groupIndex < (groupthreads - kernelhalf) /*&&
		((groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.y) < (uint)Res.y)*/) {
		float4 vOut = 0;

		[unroll]
		for (int i = -kernelhalf; i <= kernelhalf; ++i)
			vOut += SharedInput[groupIndex + i] * SampleWeights[i + kernelhalf];

		g_OutputTexture[coord] = vOut;
	}
}
[numthreads(32, 32, 1)]
void CS_DOF(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
	float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
	float4 vBlur = g_InputBlurTexture.Load(int3(vPixel / 4.f, 0));
	float fDepth = g_InputWorldTexture.Load(int3(vPixel, 0)).w;

	float fDist = abs(g_fDepth - fDepth);
	fDist *= 30.f;
	fDist = saturate(fDist);
	//fDist = exp(fDist * 1.2f);
	float fBlur = fDist;
	float fBack = 1.f - fBlur;
	float4 vColor = vBack * fBack + vBlur * fBlur;
	//vColor.a = 1.f;
	g_OutputTexture[vPixel] = vColor;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_DOF {
	pass Pass_DownScale {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALE();
	}
	pass Pass_Horizon {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_HORIZON();
	}
	pass Pass_Vertical {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_VERTICAL();
	}
	pass Pass_DOF {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOF();
	}
}