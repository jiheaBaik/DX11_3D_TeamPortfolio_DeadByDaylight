matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_BackTexture;

sampler Sampler_Default = sampler_state {
	filter = min_mag_mip_linear;
};
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vTexCoord = In.vTexCoord;
	return Out;
}
//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

float4 LightShaftValue;
float2 ScreenLightPos = { 0.5f, 0.5f };
// x = Density, y = Decay, z = Weight, w = Exposure

#define NUM_SAMPLES 64
float4 PS_GODRAY(PS_IN In) : SV_TARGET0{
	float2 vTexCoord = In.vTexCoord;
	float2 vDeltaTexCoord = (In.vTexCoord.xy - ScreenLightPos.xy);
	vDeltaTexCoord *= 1.0 / NUM_SAMPLES * LightShaftValue.x;

	float3 vColor = g_BackTexture.Sample(Sampler_Default, In.vTexCoord);
	float IlluminationDecay = 1.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		vTexCoord -= vDeltaTexCoord;
		float3 vSample = g_BackTexture.Sample(Sampler_Default, vTexCoord);
		vSample *= IlluminationDecay * LightShaftValue.z;
		vColor += vSample;
		IlluminationDecay *= LightShaftValue.y;
	}
	return saturate(float4(vColor * LightShaftValue.w, 1.0));
}
//-------------------------------------------------------------------------------------------------
RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};
DepthStencilState DSS_DEF_DWZ {
	DepthEnable = false;
	DepthWriteMask = zero;
};
BlendState	BS_Default {
	BlendEnable[0] = false;
};

technique11	Technique_GodRay {
	pass Pass_GodRay {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GODRAY();
	}
}