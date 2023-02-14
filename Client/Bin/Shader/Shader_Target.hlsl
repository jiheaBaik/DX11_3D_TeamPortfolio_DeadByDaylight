matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;

sampler Sampler_Default = sampler_state {
	filter = min_mag_mip_linear;
};

struct VS_IN {
	float3 vPos : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN_DEBUG(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;

	vector vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);

	Out.vPos = vPos;
	Out.vTexCoord = In.vTexCoord;

	return Out;
}

struct PS_IN {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT {
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(Sampler_Default, In.vTexCoord);

	return Out;
}

RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

DepthStencilState DSS_Default {
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Debug {
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState	BS_Default {
	BlendEnable[0] = false;
	//SrcBlend = src_alpha;
	//DestBlend = inv_src_alpha;
};

technique11	Technique_Default {
	pass Pass_Debug {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Debug, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DEBUG();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}
}
