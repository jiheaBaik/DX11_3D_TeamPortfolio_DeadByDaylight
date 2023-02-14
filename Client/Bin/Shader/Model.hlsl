#include "Shader_Define.hlsli"

matrix g_OldWorldMatrix, g_OldViewMatrix;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;

float g_DissolveLength;
float3 g_DissolveColor;
texture2D g_DissolveTexture;
texture2D g_DissolveAlpha;
texture2D g_TempTexture;

texture2D g_DepthTexture;
float4 g_vPenetrateColor;
float2 g_fPenetrateDist = float2(5.f, 8.f);

float4 g_vGlowColor;
float2 g_fGlow_Buffer;

float g_fRoughness;
float g_fRefract;
float g_fMetalness;
float g_fMetalness2;

bool g_bBlink;
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
};
struct VS_OUT_SKY {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct VS_OUT_GLOW {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct VS_OUT_SHADOW {
	float4 vPos : SV_POSITION;
	float fDepth : PSIZE;
};
struct VS_OUT_PENETRATE {
	float4 vPos : SV_POSITION;
	float2 vDepth : TEXCOORD0;
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
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
VS_OUT_SKY VS_SKY(VS_IN In) {
	VS_OUT_SKY Out = (VS_OUT_SKY)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vTexCoord = In.vTexCoord;
	return Out;
}
VS_OUT_GLOW VS_GLOW(VS_IN In) {
	VS_OUT_GLOW Out = (VS_OUT_GLOW)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vTexCoord = In.vTexCoord;
	return Out;
}
VS_OUT_SHADOW VS_SHADOW(VS_IN In) {
	VS_OUT_SHADOW Out = (VS_OUT_SHADOW)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.fDepth = Out.vPos.z / 1000.f;
	return Out;
}
VS_OUT_PENETRATE VS_PENETRATE(VS_IN In) {
	VS_OUT_PENETRATE Out = (VS_OUT_PENETRATE)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
VS_OUT_DISSOLVEYOUT VS_DISSOLVESUMMON(VS_IN In) {
	VS_OUT_DISSOLVEYOUT Out = (VS_OUT_DISSOLVEYOUT)0;

	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vLocalPos = Out.vPos.xyz;
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vLocalTexCoord = Out.vLocalPos.xy;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);

	return Out;
}
VS_OUT_NORMAL VS_NORMAL(VS_IN In) {
	VS_OUT_NORMAL Out = (VS_OUT_NORMAL)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
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
struct PS_IN_NORMAL {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;
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
PS_OUT PS_UV(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * 10.f);
	Out.vNormal = vector(float3(0.f, 1.f, 0.f) * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	if (0.1f > Out.vDiffuse.a)
		discard;
	return Out;
}
PS_OUT PS_SKY(float4 vPos : SV_POSITION, float2 vTexCoord : TEXCOORD0) : SV_TARGET0{
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, vTexCoord);
	Out.vDepth = vector(1.f, 1.f, 0, 1.f);
	return Out;
}
float4 PS_GLOW(float4 vPos : SV_POSITION, float2 vTexCoord : TEXCOORD0) : SV_TARGET0{
	return g_DiffuseTexture.Sample(Sampler_Default, vTexCoord);
}
float4 PS_SHADOW(float4 vPos : SV_POSITION, float fDepth : PSIZE) : SV_TARGET0{
	return vector(fDepth, fDepth, fDepth, 1.f);
}
float4 PS_PENETRATE(float4 vPos : SV_POSITION, float2 vDepth : TEXCOORD0) : SV_TARGET0{
	float2 vUV = vPos.xy;
	vUV.x /= 1280.f;
	vUV.y /= 720.f;
	float vDepthTex = g_DepthTexture.Sample(Sampler_Default, vUV).x;
	if (vDepth.x - 1.f < vDepthTex.x * 500.f)
		discard;
	float fDist = (vDepth.x - g_fPenetrateDist.x) / (g_fPenetrateDist.y - g_fPenetrateDist.x);
	fDist = saturate(fDist);
	return g_vPenetrateColor * fDist;
}
PS_OUT_GLOW PS_POLICEBLINK(float4 vPos : SV_POSITION, float2 vTexCoord : TEXCOORD0){
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;

	Out.vDiffuse = g_EmissiveTexture.Sample(Sampler_Default, vTexCoord);
	Out.vBuffer = vector(g_fGlow_Buffer.x, g_fGlow_Buffer.y, 0.f, 1.f);
	if (g_bBlink) {
		Out.vDiffuse.r = 0.f;
		if (Out.vDiffuse.g > 0.1f) {
			Out.vDiffuse.g = 0.f;
			Out.vDiffuse.b = 1.f;
		}
	}
	else
		Out.vDiffuse.g = 0.f;

	if (length(Out.vDiffuse) == 0.f)
		discard;

	return Out;
}
PS_OUT PS_DISSOLVESUMMON(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	vector vDissolveShape = g_DissolveTexture.Sample(Sampler_Default, In.vLocalTexCoord * 2.f);
	vector vDissolveAlpha = g_DissolveAlpha.Sample(Sampler_Clamp, float2(In.vLocalTexCoord.x, (g_DissolveLength - In.vLocalTexCoord.y) * 2.f));
	vector vDissolveColor = g_TempTexture.Sample(Sampler_Clamp, float2(In.vLocalTexCoord.y, (g_DissolveLength - In.vLocalTexCoord.x) * 2.f));

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
PS_OUT PS_DISSOLVESUMMON2(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	vector vDissolveShape = g_DissolveTexture.Sample(Sampler_Default, In.vLocalTexCoord * 2.f);
	vector vDissolveAlpha = g_DissolveAlpha.Sample(Sampler_Clamp, float2(In.vLocalTexCoord.x, (g_DissolveLength - In.vLocalTexCoord.y) * 2.f));
	vector vDissolveColor = g_TempTexture.Sample(Sampler_Clamp, float2(In.vLocalTexCoord.y, (g_DissolveLength - In.vLocalTexCoord.x) * 2.f));

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
PS_OUT PS_DISSOLVESPIRITWEAPON(PS_DISSOLVEYIN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vMaterial = vector(g_fRoughness, g_fRefract, g_fMetalness, g_fMetalness2);

	float2 vTexCoord = In.vTexCoord * 6.f;
	vector vDissolveShape = g_DissolveTexture.Sample(Sampler_Default, vTexCoord);
	//vector vDissolveAlpha = g_DissolveAlpha.Sample(Sampler_Clamp, float2(In.vLocalTexCoord.x, (g_DissolveLength - In.vLocalTexCoord.y) * 2.f));

	if (g_DissolveLength < vDissolveShape.r)
		discard;

	if (Out.vDiffuse.a < 0.1f)
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
//-------------------------------------------------------------------------------------------------
technique11	Technique_Default {
	pass Pass_Default {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_Sky {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_SKY();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SKY();
	}
	pass Pass_Glow {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_GLOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GLOW();
	}
	pass Pass_Shadow {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
	pass Pass_Penetrate {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_PENETRATE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PENETRATE();
	}
	pass Pass_PoliceGlow {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_GLOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POLICEBLINK();
	}
	pass Pass_Dissolve {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVESUMMON();
	}
	pass Pass_Dissolve2 {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVESUMMON2();
	}
	pass Pass_Normal {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NORMAL();
	}
	pass Pass_UV {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UV();
	}
	pass Pass_NoCull {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_NormalNoCull {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_NORMAL();
	}
	pass Pass_SpritWeaponDissolve {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DISSOLVESUMMON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVESPIRITWEAPON();
	}
}
