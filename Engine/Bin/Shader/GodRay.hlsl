matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_BackTexture;

sampler Sampler_Default = sampler_state {
	filter = min_mag_mip_point;
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

float4 g_vLightShaftValue;
float2 g_vScreenLightPos = { 0.5f, 0.5f };
// x = Density, y = Decay, z = Weight, w = Exposure

#define NUM_SAMPLES 64
float4 PS_GODRAY(PS_IN In) : SV_TARGET0{
	float2 vTexCoord = In.vTexCoord;
	float2 vDeltaTexCoord = (In.vTexCoord.xy - g_vScreenLightPos.xy);
	vDeltaTexCoord *= 1.0 / NUM_SAMPLES * g_vLightShaftValue.x;

	float3 vColor = g_BackTexture.Sample(Sampler_Default, In.vTexCoord);
	float IlluminationDecay = 1.0;
	for (int i = 0; i < NUM_SAMPLES; ++i) {
		vTexCoord -= vDeltaTexCoord;
		float3 vSample = g_BackTexture.Sample(Sampler_Default, vTexCoord);
		vSample *= IlluminationDecay * g_vLightShaftValue.z;
		vColor += vSample;
		IlluminationDecay *= g_vLightShaftValue.y;
	}
	return saturate(float4(vColor * g_vLightShaftValue.w, 1.0));
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
BlendState	BS_Add {
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = one;
	DestBlend = one;
	Blendop = add;
};

technique11	Technique_GodRay {
	pass Pass_GodRay {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GODRAY();
	}
}