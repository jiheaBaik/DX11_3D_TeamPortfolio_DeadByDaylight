#include "Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_BrushTexture;
texture2D g_DiffuseTexture;
texture2D g_DiffuseTexture0;
texture2D g_DiffuseTexture1;
texture2D g_DiffuseTexture2;
texture2D g_DiffuseTexture3;
texture2D g_NormalTexture;
texture2D g_NormalTexture0;
texture2D g_NormalTexture1;
texture2D g_NormalTexture2;
texture2D g_NormalTexture3;

float g_fRoughness;
float g_fRefract;
float g_fMetalness;
float g_fMetalness2;
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float3 vWorldPos : TEXCOORD2;
	float2 vDepth : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vTexCoord1 = In.vTexCoord1;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float3 vWorldPos : TEXCOORD2;
	float2 vDepth : TEXCOORD3;
};
struct PS_OUT {
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vWorld : SV_TARGET2;
	float4 vDepth : SV_TARGET3;
	float4 vMaterial : SV_TARGET4;
};

PS_OUT PS_MAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	vector vBrush = g_BrushTexture.Sample(Sampler_Default, In.vTexCoord1);

	float fBack = 1.f - (vBrush.r + vBrush.g + vBrush.b + vBrush.a);
	vector vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord) * fBack;
	vector vDiffuse0 = g_DiffuseTexture0.Sample(Sampler_Default, In.vTexCoord) * vBrush.b;
	vector vDiffuse1 = g_DiffuseTexture1.Sample(Sampler_Default, In.vTexCoord) * vBrush.g;
	vector vDiffuse2 = g_DiffuseTexture2.Sample(Sampler_Default, In.vTexCoord) * vBrush.r;
	vector vDiffuse3 = g_DiffuseTexture3.Sample(Sampler_Default, In.vTexCoord) * vBrush.a;

	vector vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord) * fBack;
	vector vNormal0 = g_NormalTexture0.Sample(Sampler_Default, In.vTexCoord) * vBrush.b;
	vector vNormal1 = g_NormalTexture1.Sample(Sampler_Default, In.vTexCoord) * vBrush.g;
	vector vNormal2 = g_NormalTexture2.Sample(Sampler_Default, In.vTexCoord) * vBrush.r;
	vector vNormal3 = g_NormalTexture3.Sample(Sampler_Default, In.vTexCoord) * vBrush.a;

	Out.vDiffuse = (vDiffuse + vDiffuse0 + vDiffuse1 + vDiffuse2 + vDiffuse3);
	float3 Normal = (vNormal + vNormal0 + vNormal1 + vNormal2 + vNormal3) * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(float3(0.f, 0.f, 1.f), float3(1.f, 0.f, 0.f), In.vNormal.xyz);
	Out.vNormal = vector(mul(-Normal, WorldMatrix) * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);
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
};