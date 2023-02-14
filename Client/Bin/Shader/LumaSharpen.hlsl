float sharp_strength = 1.25f;   //[0.10 to 3.00] Strength of the sharpening
float sharp_clamp = 0.035f;  //[0.000 to 1.000] Limits maximum amount of sharpening a pixel recieves - Default is 0.035
int pattern = 1;        //[1|2|3|4] Choose a sample pattern. 1 = Fast, 2 = Normal, 3 = Wider, 4 = Pyramid shaped.
float offset_bias = 1.f;  //[0.0 to 6.0] 오프셋 바이어스는 샘플링 패턴의 반지름을 조정합니다.
//I designed the pattern for offset_bias 1.0, but feel free to experiment.
float2 g_fOffset = float2(1.f, 1.f);

bool show_sharpen = 0;   //[0 or 1] Visualize the strength of the sharpen (multiplied by 4 to see it better)

#define CoefLuma float3(0.2126, 0.7152, 0.0722)      // BT.709 & sRBG luma coefficient (Monitors and HD Television)
//#define CoefLuma float3(0.299, 0.587, 0.114)       // BT.601 luma coefficient (SD Television)
//#define CoefLuma float3(1.0/3.0, 1.0/3.0, 1.0/3.0) // Equal weight coefficient

Texture2D g_InputTexture;
RWTexture2D<float4> g_OutputTexture;
//-------------------------------------------------------------------------------------------------
[numthreads(32, 32, 1)]
void CS_LUMASHARPEN(uint3 dispatchThreadID : SV_DispatchThreadID) {
	uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
	if (false == show_sharpen) {
		g_OutputTexture[vPixel] = g_InputTexture.Load(int3(vPixel, 0.f));
		return;
	}

	float3 ori = g_InputTexture.Load(int3(vPixel, 0.f)).rgb;
	float3 sharp_strength_luma = (CoefLuma * sharp_strength);

	float3 blur_ori = float3(0, 0, 0);

	switch (pattern) {
	case 1: {
		blur_ori = g_InputTexture.Load(int3(vPixel + (float2(g_fOffset.x, g_fOffset.y) / 3.0) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + (float2(-g_fOffset.x, -g_fOffset.y) / 3.0) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + (float2(-g_fOffset.x, -g_fOffset.y) / 3.0) * offset_bias, 0.f)).rgb;

		blur_ori /= 2;
		sharp_strength_luma *= 1.5;
		break;
	}
	case 2: {
		blur_ori = g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, -g_fOffset.y) * 0.5f * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, -g_fOffset.y) * 0.5f * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, g_fOffset.y) * 0.5f * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, g_fOffset.y) * 0.5f * offset_bias, 0.f)).rgb;

		blur_ori *= 0.25;
		break;
	}
	case 3: {
		blur_ori = g_InputTexture.Load(int3(vPixel + float2(0.4 * g_fOffset.x, -1.2 * g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(-1.2 * g_fOffset.x, -0.4 * g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(1.2 * g_fOffset.x, 0.4 * g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(-0.4 * g_fOffset.x, 1.2 * g_fOffset.y) * offset_bias, 0.f)).rgb;

		blur_ori *= 0.25;
		sharp_strength_luma *= 0.51;
		break;
	}
	case 4: {
		blur_ori = g_InputTexture.Load(int3(vPixel + float2(0.5 * g_fOffset.x, -g_fOffset.y * offset_bias), 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(offset_bias * -g_fOffset.x, 0.5 * g_fOffset.y), 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(offset_bias * g_fOffset.x, 0.5 * g_fOffset.y), 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(0.5 * -g_fOffset.x, g_fOffset.y * offset_bias), 0.f)).rgb;

		blur_ori /= 4.0;
		sharp_strength_luma *= 0.666;
		break;
	}
	case 8: {
		blur_ori = g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, -g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, -g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, g_fOffset.y) * offset_bias, 0.f)).rgb;

		float3 blur_ori2 = g_InputTexture.Load(int3(vPixel + float2(0, g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori2 += g_InputTexture.Load(int3(vPixel + float2(0, -g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori2 += g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, 0) * offset_bias, 0.f)).rgb;
		blur_ori2 += g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, 0) * offset_bias, 0.f)).rgb;

		blur_ori2 *= 2.0;

		blur_ori += blur_ori2;
		blur_ori += (ori * 4);
		blur_ori /= 16.0;
		break;
	}
	case 9: {
		blur_ori = g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, -g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, -g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, g_fOffset.y) * offset_bias, 0.f)).rgb;

		blur_ori += ori.rgb;

		blur_ori += g_InputTexture.Load(int3(vPixel + float2(0, g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(0, -g_fOffset.y) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(-g_fOffset.x, 0) * offset_bias, 0.f)).rgb;
		blur_ori += g_InputTexture.Load(int3(vPixel + float2(g_fOffset.x, 0) * offset_bias, 0.f)).rgb;

		blur_ori /= 9;
		break;
	}
	}

	float3 sharp = ori - blur_ori;
	float4 sharp_strength_luma_clamp = float4(sharp_strength_luma * (0.5 / sharp_clamp), 0.5);
	float sharp_luma = saturate(dot(float4(sharp, 1.0), sharp_strength_luma_clamp));
	sharp_luma = (sharp_clamp * 2.0) * sharp_luma - sharp_clamp;

	float4 done = vector(ori + sharp_luma, 1.f);

#if show_sharpen == 1
	//inputcolor.rgb = abs(sharp * 4.0);
	 //inputcolor.rgb = saturate(0.5 + (sharp_luma * 4)).rrr;
#endif
	g_OutputTexture[vPixel] = saturate(done);
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_LumaSharpen {
	pass Pass_LumaSharpen {
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_LUMASHARPEN();
	}
}