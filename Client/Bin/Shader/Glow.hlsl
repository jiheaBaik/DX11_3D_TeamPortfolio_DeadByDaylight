Texture2D g_InputTexture;
Texture2D g_InputTexture2;
Texture2D g_InputBufferTexture;
Texture2D g_InputBufferTexture2;
RWTexture2D<float4> g_OutputTexture;

float2 g_fWinSize = { 0.f, 0.f };
float2 g_fGlowPower = { 0.f, 0.f };

sampler Sampler_Default = sampler_state {
	Filter = min_mag_mip_linear;
};
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
void CS_HORIZON2(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex) {
	int2 coord = int2(groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.x, groupID.y);
	//coord = clamp(coord, int2(0, 0), int2(Res.x - 1, Res.y - 1));
	SharedInput[groupIndex] = g_InputTexture.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (groupIndex >= kernelhalf && groupIndex < (groupthreads - kernelhalf) /*&&
		((groupID.x * (groupthreads - 2 * kernelhalf) + groupIndex - kernelhalf) < (uint)Res.x)*/) {
		float4 vOut = 0;

		[unroll]
		for (int i = -kernelhalf; i <= kernelhalf; ++i) {
			if (length(SharedInput[groupIndex + i]) > length(vOut)) {
				vOut = SharedInput[groupIndex + i];
			}
		}

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
[numthreads(groupthreads, 1, 1)]
void CS_VERTICAL2(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex) {
	int2 coord = int2(groupID.x, groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.y);
	//coord = clamp(coord, int2(0, 0), int2(Res.x - 1, Res.y - 1));
	SharedInput[groupIndex] = g_InputTexture.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (groupIndex >= kernelhalf && groupIndex < (groupthreads - kernelhalf) /*&&
		((groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.y) < (uint)Res.y)*/) {
		float4 vOut = 0;

		[unroll]
		for (int i = -kernelhalf; i <= kernelhalf; ++i) {
			//vOut += SharedInput[groupIndex + i];
			if (length(SharedInput[groupIndex + i]) > length(vOut)) {
				vOut = SharedInput[groupIndex + i];
			}
		}

		g_OutputTexture[coord] = vOut;
	}
}
[numthreads(32, 32, 1)]
void CS_GLOW(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
	float2 vUV = float2(vPixel.x / g_fWinSize.x, vPixel.y / g_fWinSize.y);

	float4 vGlow = g_InputTexture.Load(int3(vPixel, 0));
	float4 vGlow0 = g_InputTexture2.SampleLevel(Sampler_Default, vUV, 0);
	float4 vBuffer = g_InputBufferTexture.Load(int3(vPixel, 0));
	float4 vBuffer0 = g_InputBufferTexture2.SampleLevel(Sampler_Default, vUV, 0);

	float4 vColor = vGlow * vBuffer.x + vGlow0 * vBuffer0.y;

	g_OutputTexture[vPixel] = vColor;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Glow {
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
	pass Pass_Glow {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_GLOW();
	}
	pass Pass_Horizon2 {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_HORIZON2();
	}
	pass Pass_Vertical2 {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_VERTICAL2();
	}
}