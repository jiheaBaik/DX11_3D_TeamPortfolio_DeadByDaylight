Texture2D g_InputTexture0;
Texture2D g_InputTexture1;
RWTexture2D<float4> g_OutputTexture;

matrix g_OldViewMatrix, g_ViewMatrix, g_ProjMatrix;
//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_MOTIONBLUR(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

	float4 vWorld = g_InputTexture1.Load(int3(vPixel, 0));
	if (vWorld.x == 0.f && vWorld.y == 0.f) {
		g_OutputTexture[vPixel] = g_InputTexture0.Load(int3(vPixel, 0.f));
		return;
	}
	vWorld = mul(vector(vWorld.xyz, 1.f), g_OldViewMatrix);
	vWorld = mul(vWorld, g_ProjMatrix);
	vWorld /= vWorld.w;
	vWorld = vWorld * 0.5f + 0.5f;
	vWorld.y = 1.f - vWorld.y;
	vWorld.x *= 1280.f;
	vWorld.y *= 720.f;
	float2 vVelocity = vPixel.xy - vWorld.xy;
	vVelocity /= 10.f;

	int iCnt = 1;
	vector vColor = vector(0.f, 0.f, 0.f, 0.f);
	for (int i = iCnt; i < 5; ++i) {
		uint2 vDir = vPixel + vVelocity.xy * i;
		float4 vBlur = g_InputTexture0.Load(int3(vDir, 0.f));
		iCnt++;
		vColor += vBlur;
	}
	vColor /= iCnt - 1;

	g_OutputTexture[vPixel] = vColor;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_MotionBlur {
	pass Pass_Motion_Blur {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_MOTIONBLUR();
	}
}