#include "Shader_Define.hlsli"

float2 g_vGlow_Buffer;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
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
VS_OUT VS_TRAIL(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.vTexCoord = In.vTexCoord;
	return Out;
}
//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct PS_OUT {
	float4 vColor : SV_TARGET0;
};
struct PS_OUT_GLOW {
	float4 vColor : SV_TARGET0;
	float4 vBuffer : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	if (Out.vColor.a < 0.1f)
		discard;
	return Out;
}
PS_OUT PS_ALPHABLEND(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	return Out;
}
PS_OUT_GLOW PS_TRAIL(PS_IN In) {
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vBuffer = vector(g_vGlow_Buffer.x, g_vGlow_Buffer.y, 0.f, 1.f);
	Out.vColor.a = Out.vColor.r;
	return Out;
}
PS_OUT_GLOW PS_GLOW(PS_IN In) {
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vBuffer = vector(g_vGlow_Buffer.x, g_vGlow_Buffer.y, 0.f, 1.f);
	if (Out.vColor.a < 0.1f)
		discard;
	return Out;
}
//-------------------------------------------------------------------------------------------------
technique11 Technique_Default {
	pass Pass_Default {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_AlpahBlend {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();
	}
	pass Pass_Trail {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_TRAIL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TRAIL();
	}
	pass Pass_Glow {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GLOW();
	}
};