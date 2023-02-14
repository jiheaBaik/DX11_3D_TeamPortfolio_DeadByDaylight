#include "Shader_Define.hlsli"

float3 g_vCameraPos;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_DepthTexture;
float4 g_vPenetrateColor;
float2 g_fPenetrateDist = float2(5.f, 8.f);

texture2D g_TempTexture;
texture2D g_DestTexture;

float g_DissolveLength;
float3 g_DissolveColor;
texture2D g_DissolveTexture;
texture2D g_DissolveAlpha;

float4 g_vUIColor;
float4 g_vUIColor2;
float g_fSuccesTime;

float2 g_fWinSize;

float g_fRoughness;
float g_fRefract;
float g_fMetalness;
float g_fMetalness2;

struct Bone {
	matrix Matrix[256];
};
Bone g_Bone;

float2 g_fGlow_Buffer;

//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT;
	uint4 vBlendIndex : BLENDINDEX;
	float4 vBlendWeight : BLENDWEIGHT;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
};
struct VS_OUT_SHADOW {
	float4 vPos : SV_POSITION;
	float fDepth : PSIZE;
};
struct VS_OUT_DISSOLVEYOUT {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float3 vLocalPos : TEXCOORD2;
	float2 vLocalTexCoord : TEXCOORD3;
	float2 vDepth : TEXCOORD4;
};
struct VS_OUT_PENETRATE {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float2 vDepth : TEXCOORD1;
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
	VS_OUT Out = (VS_OUT)0;

	float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
	matrix BoneMatrix = g_Bone.Matrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bone.Matrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bone.Matrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bone.Matrix[In.vBlendIndex.w] * fWeightW;

	Out.vPos = mul(float4(In.vPos, 1.f), BoneMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vNormal = normalize(mul(mul(float4(In.vNormal, 0.f), BoneMatrix), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
VS_OUT_SHADOW VS_SHADOW(VS_IN In) {
	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;

	float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
	matrix BoneMatrix = g_Bone.Matrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bone.Matrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bone.Matrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bone.Matrix[In.vBlendIndex.w] * fWeightW;

	Out.vPos = mul(float4(In.vPos, 1.f), BoneMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.fDepth = Out.vPos.z / 1000.f;
	return Out;
}
VS_OUT_DISSOLVEYOUT VS_DISSOLVESUMMON(VS_IN In) {
	VS_OUT_DISSOLVEYOUT Out = (VS_OUT_DISSOLVEYOUT)0;

	float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
	matrix BoneMatrix = g_Bone.Matrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bone.Matrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bone.Matrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bone.Matrix[In.vBlendIndex.w] * fWeightW;

	Out.vPos = mul(float4(In.vPos, 1.f), BoneMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vLocalPos = Out.vPos.xyz;
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.vNormal = normalize(mul(mul(float4(In.vNormal, 0.f), BoneMatrix), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vLocalTexCoord = Out.vLocalPos.xy;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);

	return Out;
}
VS_OUT_PENETRATE VS_PENETRATE(VS_IN In) {
	VS_OUT_PENETRATE Out = (VS_OUT_PENETRATE)0;

	float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
	matrix BoneMatrix = g_Bone.Matrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bone.Matrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bone.Matrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bone.Matrix[In.vBlendIndex.w] * fWeightW;

	Out.vPos = mul(float4(In.vPos, 1.f), BoneMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
VS_OUT_NORMAL VS_NORMAL(VS_IN In) {
	VS_OUT_NORMAL Out = (VS_OUT_NORMAL)0;

	float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
	matrix BoneMatrix = g_Bone.Matrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_Bone.Matrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_Bone.Matrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_Bone.Matrix[In.vBlendIndex.w] * fWeightW;

	Out.vPos = mul(float4(In.vPos, 1.f), BoneMatrix);
	Out.vPos = mul(Out.vPos, g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vNormal = normalize(mul(mul(float4(In.vNormal, 0.f), BoneMatrix), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	Out.vTangent = normalize(mul(mul(float4(In.vTangent, 0.f), BoneMatrix), g_WorldMatrix).xyz);
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
	return Out;
}
//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
};
struct PS_IN_NORMAL {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
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
struct PS_DISSOLVEYIN {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float3 vLocalPos : TEXCOORD2;
	float2 vLocalTexCoord : TEXCOORD3;
	float2 vDepth : TEXCOORD4;
};
struct PS_OUT_NOLIGHT {
	float4 vDiffuse : SV_TARGET0;
	float4 vWorld : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
};
struct PS_OUT_GLOW {
	float4 vDiffuse : SV_TARGET0;
	float4 vBuffer : SV_TARGET1;
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
float4 PS_SHADOW(float4 vPos : SV_POSITION, float fDepth : PSIZE) : SV_TARGET0{
	return vector(fDepth, fDepth, fDepth, 1.f);
}
PS_OUT PS_DISSOLVESUMMON(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	vector vDissolveShape = g_DissolveTexture.Sample(Sampler_Default, In.vLocalTexCoord * 2.f);
	vector vDissolveAlpha = g_DissolveAlpha.Sample(Sampler_Clamp, float2(In.vLocalTexCoord.x, (g_DissolveLength - In.vLocalTexCoord.y) * 2.f));

	if (g_DissolveLength < In.vLocalPos.y)
		discard;

	float fDissolveRValue = vDissolveShape.r * vDissolveAlpha.r;
	float fDissolveGValue = vDissolveShape.g * vDissolveAlpha.r;
	float fDissolveBValue = vDissolveShape.b * vDissolveAlpha.r;

	if (g_DissolveLength >= In.vLocalPos.y) {
		if (0.1f < fDissolveRValue || 0.04f < fDissolveGValue || 0.04f < fDissolveBValue)
			discard;
		if (0.1f >= fDissolveRValue && 0.04f >= fDissolveGValue && 0.04f >= fDissolveBValue)
			Out.vDiffuse.xyz = Out.vDiffuse.xyz * (1.f - vDissolveAlpha.r) + g_DissolveColor * vDissolveAlpha.r;
	}

	if (Out.vDiffuse.a < 0.1f)
		discard;
	return Out;
}
PS_OUT_GLOW PS_DISSOLVESUMMON2(PS_DISSOLVEYIN In) {
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vBuffer = vector(3.f, 3.f, 0.f, 1.f);

	vector vDissolveShape = g_DissolveTexture.Sample(Sampler_Default, In.vLocalTexCoord * 2.f);
	vector vDissolveAlpha = g_DissolveAlpha.Sample(Sampler_Clamp, float2(In.vLocalTexCoord.x, (g_DissolveLength - In.vLocalTexCoord.y) * 2.f));

	if (g_DissolveLength < In.vLocalPos.y)
		discard;

	float fDissolveRValue = vDissolveShape.r * vDissolveAlpha.r;
	float fDissolveGValue = vDissolveShape.g * vDissolveAlpha.r;
	float fDissolveBValue = vDissolveShape.b * vDissolveAlpha.r;

	if (g_DissolveLength >= In.vLocalPos.y) {
		if (0.1f < fDissolveRValue || 0.04f < fDissolveGValue || 0.04f < fDissolveBValue)
			discard;
		if (0.1f >= fDissolveRValue && 0.04f >= fDissolveGValue && 0.04f >= fDissolveBValue)
			Out.vDiffuse.xyz = g_DissolveColor * vDissolveAlpha.r;
	}

	if (Out.vDiffuse.a < 0.1f)
		discard;
	return Out;
}
PS_OUT PS_DISSOLVEHOOKSUMMON(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	vector vDissolveTexture = g_DissolveTexture.Sample(Sampler_Default, In.vTexCoord * 4.f);

	float DissolveLength = length(vDissolveTexture);
	if (DissolveLength - 0.2f > g_DissolveLength)
		discard;
	if (DissolveLength - 0.12f > g_DissolveLength)
		Out.vDiffuse.xyz = g_DissolveColor;
	
	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}
PS_OUT PS_DISSOLVEHOOKSUMMON2(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);

	vector vDissolveTexture = g_DissolveTexture.Sample(Sampler_Default, In.vTexCoord * 4.f);

	float DissolveLength = length(vDissolveTexture);
	if (DissolveLength - 0.09f > g_DissolveLength)
		discard;

	if (DissolveLength > g_DissolveLength)
		Out.vDiffuse.xyz = g_DissolveColor;

	if (Out.vDiffuse.a < 0.1f)
		discard;
	
	return Out;
}
PS_OUT PS_DISSOLVEPLANT(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	vector vDissolveTexture = g_DissolveTexture.Sample(Sampler_Default, In.vTexCoord * 4.f);

	float DissolveLength = length(vDissolveTexture);
	if (DissolveLength - 0.1f > g_DissolveLength)
		discard;
	if (DissolveLength - 0.07f > g_DissolveLength)
		Out.vDiffuse.xyz = g_DissolveColor;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}
PS_OUT PS_DISSOLVEPLANT2(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);

	vector vDissolveTexture = g_DissolveTexture.Sample(Sampler_Default, In.vTexCoord * 4.f);

	float DissolveLength = length(vDissolveTexture);
	if (DissolveLength - 0.04f > g_DissolveLength)
		discard;
	if (DissolveLength > g_DissolveLength)
		Out.vDiffuse.xyz = g_DissolveColor;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}
PS_OUT PS_DISSOLVECAMPERHOOKDEATH(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	vector vDissolveTexture = g_DissolveTexture.Sample(Sampler_Default, In.vTexCoord * 4.f);

	float DissolveLength = length(vDissolveTexture);
	if (DissolveLength - 0.1f > g_DissolveLength)
		Out.vDiffuse = float4(0.01f, 0.01f, 0.01f, 0.5f);

	if (DissolveLength - 0.07f > g_DissolveLength)
		Out.vDiffuse.xyz = g_DissolveColor;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}
PS_OUT PS_DISSOLVECAMPERHOOKDEATH2(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);

	vector vDissolveTexture = g_DissolveTexture.Sample(Sampler_Default, In.vTexCoord * 4.f);

	float DissolveLength = length(vDissolveTexture);
	if (DissolveLength - 0.04f > g_DissolveLength)
		Out.vDiffuse = float4(0.01f, 0.01f, 0.01f, 0.5f);
	if (DissolveLength > g_DissolveLength)
		Out.vDiffuse.xyz = g_DissolveColor;

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}
float4 PS_PENETRATE(float4 vPos : SV_POSITION, float2 vTexCoord : TEXCOORD0, float2 vDepth : TEXCOORD1) : SV_TARGET0{
	vector vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, vTexCoord);
	float2 vUV = vPos.xy;
	vUV.x /= g_fWinSize.x;
	vUV.y /= g_fWinSize.y;
	float vDepthTex = g_DepthTexture.Sample(Sampler_Default, vUV).x;
	if (vDepth.x - 1.f < vDepthTex.x * 500.f)
		discard;

	float fDist = (vDepth.x - g_fPenetrateDist.x) / (g_fPenetrateDist.y - g_fPenetrateDist.x);
	fDist = saturate(fDist);
	return (g_vPenetrateColor * vDiffuse + g_vPenetrateColor * 0.5f) * fDist;
}
PS_OUT_NOLIGHT PS_NOLIGHT(PS_IN In) {
	PS_OUT_NOLIGHT Out = (PS_OUT_NOLIGHT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	if (0.1f > Out.vDiffuse.a)
		discard;
	return Out;
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

float4 PS_UI(PS_IN In) : SV_TARGET0{
	float4 vUIColor = g_vUIColor;
	float4 vUIColor2 = g_vUIColor2;

	float a = 1.f;
	if (g_fSuccesTime >= 4.5f && g_fSuccesTime <= 5.f)
		a = 1.f - (g_fSuccesTime - 4.5f) / 0.5f;
	if (g_fSuccesTime <= 0.5f && g_fSuccesTime >= 0.f)
		a = g_fSuccesTime / 0.5f;

	float vDot = dot(normalize(g_vCameraPos - In.vWorldPos), In.vNormal);
	return ((vUIColor * 0.1f) + (vUIColor * vDot) + ((1.f - vDot) * vUIColor2)) * a;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_ModelAnim {
	pass Pass_Default {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_Shadow {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
	pass Pass_Dissolve {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVESUMMON();
	}
	pass Pass_Penetrate {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_PENETRATE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PENETRATE();
	}
	pass Pass_Dissolve2 {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVESUMMON2();
	}
	pass Pass_NoLight {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NOLIGHT();
	}
	pass Pass_Locker {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_Normal {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NORMAL();
	}
	pass Pass_UI {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UI();
	}
	pass Pass_HookDissolve {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVEHOOKSUMMON();
	}
	pass Pass_HookDissolve2 {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVEHOOKSUMMON2();
	}
	pass Pass_PlantDissolve {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVEPLANT();
	}
	pass Pass_PlantDissolve2 {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVEPLANT2();
	}
	pass Pass_CamperHookDeathDissolve {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVECAMPERHOOKDEATH();
	}
	pass Pass_CamperHookDeathDissolve2 {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVECAMPERHOOKDEATH2();
	}//14
}