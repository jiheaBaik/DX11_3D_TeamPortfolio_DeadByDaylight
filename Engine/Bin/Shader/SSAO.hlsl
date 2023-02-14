matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_WorldTexture;
texture2D g_NormalTexture;
Texture2D g_RandomTexture;

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
float g_sample_rad;
float g_intensity;
float g_scale;
float g_bias;

struct PS_INPUT { 
	float4 vPos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
struct PS_OUTPUT { 
	float4 color : SV_TARGET0; 
};

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm) {
	float4 vp = g_WorldTexture.Sample(Sampler_Default, tcoord + uv);
	float3 diff = vp - p;
	const float3 v = normalize(diff);
	const float d = length(diff) * g_scale;
	return max(0.0, dot(cnorm, v) - g_bias) * (1.0 / (1.0 + d)) * g_intensity;
}

PS_OUTPUT PS_MAIN(PS_INPUT i) {
	PS_OUTPUT o = (PS_OUTPUT)0;
	o.color.rgb = 1.0f;
	const float2 vec[4] = { float2(1,0),float2(-1,0), float2(0,1),float2(0,-1) };

	float4 p = g_WorldTexture.Sample(Sampler_Default, i.uv);
	float3 n = g_NormalTexture.Sample(Sampler_Default, i.uv).xyz * 2.0f - 1.0f;
	float2 rand = g_RandomTexture.Sample(Sampler_Default, i.uv).xy * 2.0f - 1.0f;
	float ao = 0.0f;
	float rad = g_sample_rad / p.z;

	int iterations = 4; 
	for (int j = 0; j < iterations; ++j){
		float2 coord1 = reflect(vec[j], rand) * rad;
		float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);

		ao += doAmbientOcclusion(i.uv, coord1 * 0.25, p, n);
		ao += doAmbientOcclusion(i.uv, coord2 * 0.5, p, n);
		ao += doAmbientOcclusion(i.uv, coord1 * 0.75, p, n);
		ao += doAmbientOcclusion(i.uv, coord2, p, n);
	}

	ao /= (float)iterations * 4.0;
	o.color = 1.f - ao;
	o.color.a = 1.f;
	return o;
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

technique11	Technique_Deferred {
	pass Pass_Deferred {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}