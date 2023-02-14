matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vColor = (vector)0.f;

texture2D g_DepthTexture;

sampler Sampler_Default = sampler_state {
	filter = min_mag_mip_linear;
};
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float2 vDepth : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
//-------------------------------------------------------------------------------------------------
float4 PS_MAIN(float4 vPos : SV_POSITION, float2 vDepth : TEXCOORD0) : SV_TARGET0{
	float2 vUV = vPos.xy;
	vUV.x /= 1600.f;
	vUV.y /= 900.f;
	float vDepthTex = g_DepthTexture.Sample(Sampler_Default, vUV).x;
	if (vDepth.x - 0.1f > vDepthTex.x * 500.f)
		discard;
	return g_vColor;
}
//-------------------------------------------------------------------------------------------------
DepthStencilState DSS_DEF_DWZ {
	DepthEnable = false;
	DepthWriteMask = zero;
};

technique11	Technique_Cell {
	pass Pass_Cell {
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		//SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

