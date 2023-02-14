#include "Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_NoiseTexture;
texture2D g_TempTexture;

float g_fRoughness;
float g_fRefract;
float g_fMetalness;
float g_fMetalness2;
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT;

	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vLook : TEXCOORD3;
	float4 vTrans : TEXCOORD4;
};

struct VS_OUT {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
};
struct VS_OUT_SHADOW {
	float4 vPos : SV_POSITION;
	float2 vDepth : TEXCOORD0;
};
struct VS_OUT_NORMAL {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In) {
	matrix InstMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTrans);

	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), InstMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), InstMatrix).xyz);
	Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
VS_OUT_SHADOW VS_SHADOW(VS_IN In) {
	matrix InstMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTrans);

	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;
	Out.vPos = mul(float4(In.vPos, 1.f), InstMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
VS_OUT_NORMAL VS_NORMAL(VS_IN In) {
	matrix InstMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTrans);

	VS_OUT_NORMAL Out = (VS_OUT_NORMAL)0;
	Out.vPos = mul(float4(In.vPos, 1.f), InstMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), InstMatrix).xyz);
	Out.vNormal = normalize(mul(float4(Out.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	Out.vTangent = mul(float4(In.vTangent, 0.f), InstMatrix);
	Out.vTangent = normalize(mul(vector(Out.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
	return Out;
}

//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
};
struct PS_IN_NORMAL {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;
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

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	if (0.1f > Out.vDiffuse.a)
		discard;
	return Out;
}
float4 PS_SHADOW(float4 vPos : SV_POSITION, float2 vDepth : TEXCOORD0) : SV_TARGET0{
	float fDepth = vDepth.x / 1000.f;
	return vector(fDepth, fDepth, fDepth, 1.f);
}
PS_OUT PS_NORMAL(PS_IN_NORMAL In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	float3 vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	Out.vNormal = vector(mul(-vNormal, WorldMatrix) * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	if (0.1f > Out.vDiffuse.a)
		discard;
	return Out;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_ModelInst {
	pass Pass_Default {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_Shadow {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
	pass Pass_Normal {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NORMAL();
	}
}