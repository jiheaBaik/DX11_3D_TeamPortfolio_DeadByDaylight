#include "Shader_Define.hlsli"

matrix g_InvWorldMatrix;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

textureCUBE g_DiffuseTexture;
texture2D g_WorldTexture;
texture2D g_NormalTexture;

float2 g_fWinSize;
float2 g_fGlow_Buffer;

float4 g_fColor;
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float3 vTexCoord : TEXCOORD0;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float3 vTexCoord : TEXCOORD0;
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
	float3 vTexCoord : TEXCOORD0;
};
struct PS_OUT {
	float4 vColor : SV_TARGET0;
};
struct PS_OUT_GLOW {
	float4 vDiffuse : SV_TARGET0;
	float4 vBuffer : SV_TARGET1;
};
float4 PS_MAIN(PS_IN In) : SV_TARGET0{
	return  g_DiffuseTexture.Sample(Sampler_Default,In.vTexCoord);
}

PS_OUT_GLOW PS_SSD(PS_IN In) {
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;

	float2 vUV = In.vPos.xy;
	vUV.x /= g_fWinSize.x;
	vUV.y /= g_fWinSize.y;

	float4 vWorld = vector(g_WorldTexture.Sample(Sampler_Default, vUV).xyz, 1.f);
	float4 vLocal = mul(vWorld, g_InvWorldMatrix);
	float4 vCheck = abs(vLocal);
	if (vCheck.x > 0.5f || vCheck.y > 0.5f || vCheck.z > 0.5f) {
		discard;
	}

	float3 vNormal = g_NormalTexture.Sample(Sampler_Default, vUV).xyz * 2.f - 1.f;
	float absX = abs(vNormal.x);
	float absY = abs(vNormal.y);
	float absZ = abs(vNormal.z);

	if (absX > absY && absX > absZ) {
		if (vNormal.x > 0.f)
			vLocal.x = -0.5f;
		else
			vLocal.x = 0.5f;

	}
	if (absY > absX && absY > absZ) {
		if (vNormal.y > 0.f)
			vLocal.y = -0.5f;

		else
			vLocal.y = 0.5f;
	}
	if (absZ > absY && absZ > absX) {
		if (vNormal.z > 0.f)
			vLocal.z = -0.5f;

		else
			vLocal.z = 0.5f;
	}

	//vLocal += 0.5f;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, (vLocal));
	Out.vDiffuse *= g_fColor; // 원하는 색상으로 바꿈
	Out.vBuffer = vector(g_fGlow_Buffer.x, g_fGlow_Buffer.y, 0.f, 1.f);
	return Out;
}
//-------------------------------------------------------------------------------------------------
technique11 Technique_Cube {
	pass Pass_Cube {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_SSD {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_SSD();
	}
}