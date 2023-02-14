Texture2D g_InputTexture : register(t0);
StructuredBuffer<float> g_InputAvgLum : register(t0);
RWStructuredBuffer<float> g_OutputAvgLum : register(u0);
groupshared float SharedPositions[1024];

uint2 g_vRes;
uint g_iDomain;
uint g_iGroupSize;
static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);
//-------------------------------------------------------------------------------------------------
float DownScale4x4(uint2 vPixel, uint groupThreadId) {
	float avgLum = 0.f;

	//if (vPixel.y < g_vRes.y) {
		int3 iFullResPos = int3(vPixel * 4, 0);
		float4 vDownScaled = float4(0.f, 0.f, 0.f, 0.f);
		[unroll]
		for (int i = 0; i < 4; ++i) {
			[unroll]
			for (int j = 0; j < 4; ++j)
				vDownScaled += g_InputTexture.Load(iFullResPos, int2(j, i));
		}
		vDownScaled /= 16;

		avgLum = dot(vDownScaled, LUM_FACTOR);
		SharedPositions[groupThreadId] = avgLum;
	//}

	GroupMemoryBarrierWithGroupSync();
	return avgLum;
}
float DownScale1024to4(uint groupThreadId, uint dispachThreadId, float avgLum) {
	for (uint iGroupSize = 4, iStep1 = 1, iStep2 = 2, iStep3 = 3; iGroupSize < 1024; iGroupSize *= 4, iStep1 *= 4, iStep2 *= 4, iStep3 *= 4) {
		if (groupThreadId % iGroupSize == 0) {
			float fStepAvgLum = avgLum;

			fStepAvgLum += dispachThreadId + iStep1 < g_iDomain ? SharedPositions[groupThreadId + iStep1] : avgLum;
			fStepAvgLum += dispachThreadId + iStep2 < g_iDomain ? SharedPositions[groupThreadId + iStep2] : avgLum;
			fStepAvgLum += dispachThreadId + iStep3 < g_iDomain ? SharedPositions[groupThreadId + iStep3] : avgLum;

			avgLum = fStepAvgLum;
			SharedPositions[groupThreadId] = fStepAvgLum;
		}
		GroupMemoryBarrierWithGroupSync();
	}
	return avgLum;
}
void DownScale4to1(uint groupId, uint groupThreadId, uint dispatchThreadId, float avgLum) {
	if (groupThreadId == 0) {
		float fFinalAvgLum = avgLum;

		fFinalAvgLum += dispatchThreadId + 256 < g_iDomain ? SharedPositions[groupThreadId + 256] : avgLum;
		fFinalAvgLum += dispatchThreadId + 512 < g_iDomain ? SharedPositions[groupThreadId + 512] : avgLum;
		fFinalAvgLum += dispatchThreadId + 768 < g_iDomain ? SharedPositions[groupThreadId + 768] : avgLum;

		fFinalAvgLum /= 1024.f;
		g_OutputAvgLum[groupId] = fFinalAvgLum;
	}
}
[numthreads(1024, 1, 1)]
void CS_FIRST(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadId.x % g_vRes.x, dispatchThreadId.x / g_vRes.x);

	float favgLum = DownScale4x4(vPixel, groupThreadId.x);
	favgLum = DownScale1024to4(groupThreadId.x, dispatchThreadId.x, favgLum);
	DownScale4to1(groupId.x, groupThreadId.x, dispatchThreadId.x, favgLum);
}


#define MAX_GROUPS 64
// ���� �޸� �׷쿡 �߰� �� ����
groupshared float SharedAvgFinal[MAX_GROUPS];

[numthreads(MAX_GROUPS, 1, 1)]
void CS_SECOND(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID) {
    float favgLum = 0.f;

    if (dispatchThreadId.x < g_iGroupSize) {
        favgLum = g_InputAvgLum[dispatchThreadId.x];
    }
    SharedAvgFinal[dispatchThreadId.x] = favgLum;
    GroupMemoryBarrierWithGroupSync(); 

    if (dispatchThreadId.x % 4 == 0) {
        float fstepAvgLum = favgLum;
        fstepAvgLum += dispatchThreadId.x + 1 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 1] : favgLum;
        fstepAvgLum += dispatchThreadId.x + 2 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 2] : favgLum;
        fstepAvgLum += dispatchThreadId.x + 3 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 3] : favgLum;
        favgLum = fstepAvgLum;
        SharedAvgFinal[dispatchThreadId.x] = fstepAvgLum;
    }
    GroupMemoryBarrierWithGroupSync();

    if (dispatchThreadId.x % 16 == 0) {
        float fstepAvgLum = favgLum;
        fstepAvgLum += dispatchThreadId.x + 4 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 4] : favgLum;
        fstepAvgLum += dispatchThreadId.x + 8 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 8] : favgLum;
        fstepAvgLum += dispatchThreadId.x + 12 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 12] : favgLum;
        favgLum = fstepAvgLum;
        SharedAvgFinal[dispatchThreadId.x] = fstepAvgLum;
    }
    GroupMemoryBarrierWithGroupSync();

    if (dispatchThreadId.x == 0) {
        float fFinalLumValue = favgLum;
        fFinalLumValue += dispatchThreadId.x + 16 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 16] : favgLum;
        fFinalLumValue += dispatchThreadId.x + 32 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 32] : favgLum;
        fFinalLumValue += dispatchThreadId.x + 48 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 48] : favgLum;
        fFinalLumValue /= 64.f;
		g_OutputAvgLum[0] = max(fFinalLumValue, 0.0001);
    }
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_HDR {
	pass Pass_First {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_FIRST();
	}
    pass Pass_Second {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_SECOND();
    }
}