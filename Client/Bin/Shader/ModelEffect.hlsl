#include "Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_NoiseTexture;
texture2D g_TempTexture;
texture2D g_DestTexture;
texture2D g_DepthTexture;
float2 g_FixTexCoord;
float2 g_TexCoord;
float2 g_TexCoordCut;
float4 g_Color;
float4 g_PlusColor;
float2 g_fGlow_Buffer;
float g_fAlpha;
float g_fSmokeAlpha;
float2 g_fWinSize;
float2 g_flowUV;
float g_fTime;
float3 g_fScrollSpeeds;
float3 g_fScales;
float2 g_fDistortion1;
float2 g_fDistortion2;
float2 g_fDistortion3;
float g_fDiscardAlpha;
float g_DissolveLength;
bool g_bReverse;
bool g_bSetColor;
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
struct VS_OUT_NORMAL {
	float4 vPos : SV_POSITION;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
	float4 vTangent : TEXCOORD3;
	float4 vBinormal : TEXCOORD4;
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
VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In) {
	VS_OUT_NORMAL Out = (VS_OUT_NORMAL)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
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
	float4 vTangent : TEXCOORD3;
	float4 vBinormal : TEXCOORD4;
};
struct PS_OUT {
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vWorld : SV_TARGET2;
	float4 vDepth : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);
	if (g_bReverse) {
		if (g_fAlpha > 1 - Out.vDiffuse.a)
			discard;
	}
	else {
		if (g_fAlpha > Out.vDiffuse.a)
			discard;
	}

	if (g_bSetColor)
		Out.vDiffuse *= g_Color;

	return Out;
}
PS_OUT PS_FRAME(PS_IN_NORMAL In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	float3 vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y)).xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	Out.vNormal = vector(mul(-vNormal, WorldMatrix) * 0.5f + 0.5f, 1.f);
	//Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	if (g_bReverse) {
		if (g_fAlpha > 1 - Out.vDiffuse.a)
			discard;
	}
	else {
		if (g_fAlpha > Out.vDiffuse.a)
			discard;
	}

	if (g_bSetColor)
		Out.vDiffuse.xyz *= g_Color.xyz;

	return Out;
}
PS_OUT PS_UNNORMALFRAME(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 0.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);

	if (g_bReverse) {
		if (g_fAlpha > 1 - Out.vDiffuse.a)
			discard;
	}
	else {
		if (g_fAlpha > Out.vDiffuse.a)
			discard;
	}

	if (g_bSetColor)
		Out.vDiffuse.xyz *= g_Color.xyz;

	return Out;
}
PS_OUT PS_OTHERTEXFRAME(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);

	if (g_bReverse) {
		if (g_fAlpha > 1 - Out.vDiffuse.r)
			discard;
	}
	else {
		if (g_fAlpha > Out.vDiffuse.r)
			discard;
	}

	if (g_bSetColor)
		Out.vDiffuse.xyz *= g_Color.xyz;

	return Out;
}
PS_OUT PS_OTHERTEXSETCOLORFRAME(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	float4 vTemp = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);

	if (g_bReverse) {
		if (g_fAlpha > 1 - Out.vDiffuse.r)
			discard;
	}
	else {
		if (g_fAlpha > Out.vDiffuse.r)
			discard;
	}
	Out.vDiffuse.xyz *= vTemp.xyz;

	return Out;
}
PS_OUT PS_BLOODSTAIN_RG(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord);
	float4 Alpha = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);

	if (Alpha.r < g_fAlpha && Alpha.g < g_fAlpha)
		discard;

	return Out;
}
PS_OUT PS_BLOODSTAIN_R(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord);
	float4 Alpha = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);

	if (Alpha.r < g_fAlpha)
		discard;

	return Out;
}
PS_OUT PS_ALPHATESTCOLORTBLOOD(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);

	if (Out.vDiffuse.r < 0.1f)
		discard;

	if (Out.vDiffuse.r < g_fAlpha)
		discard;

	Out.vDiffuse.xyz *= g_Color.xyz;

	return Out;
}
PS_OUT PS_BLOODPARTICLE(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);

	if (g_bReverse) {
		if (g_fAlpha > 1 - Out.vDiffuse.a)
			discard;
	}
	else {
		if (g_fAlpha > Out.vDiffuse.a)
			discard;
	}

	if (g_bSetColor)
		Out.vDiffuse.xyz *= g_Color.xyz;

	//if (Out.vDiffuse.r < g_fAlpha)
	//	discard;

	return Out;
}

PS_OUT PS_FOOTBLOOD0(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	//Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	float4 Alpha = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);

	if (Alpha.r < g_fAlpha)
		discard; 

	Out.vDiffuse.xyz *= g_Color.xyz;
	

	return Out;
}
PS_OUT PS_FOOTBLOOD_NORMAL(PS_IN_NORMAL In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	float3 vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	Out.vNormal = vector(mul(-vNormal, WorldMatrix) * 0.5f + 0.5f, 1.f);
	//Out.vNormal = vector(In.vNormal * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	float4 Alpha = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);

	if (Alpha.r < g_fAlpha)
		discard;

	Out.vDiffuse.xyz *= g_Color.xyz;

	return Out;
}
PS_OUT PS_COLORA(PS_IN_NORMAL In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	float3 vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f;
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
	Out.vNormal = vector(mul(-vNormal, WorldMatrix) * 0.5f + 0.5f, 1.f);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vDiffuse.a = Out.vDiffuse.r;

	Out.vDiffuse.a -= g_fAlpha;

	Out.vDiffuse *= g_Color;

	return Out;
}
PS_OUT PS_RAINDROP(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vNormal = g_NormalTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0.f, 1.f);
	Out.vDiffuse.a = Out.vDiffuse.r;
	if (Out.vDiffuse.a < 0.1f)
		discard;
	if (Out.vDiffuse.a < g_fAlpha)
		discard;

	Out.vDiffuse *= g_Color;
	return Out;
}
//-------------------------------------------------------------------------------------------------
technique11	Technique_Default {
	//0
	pass Pass_Default {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	//1
	pass Pass_Frame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FRAME();
	}
	//2
	pass Pass_UnNormalFrame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_UNNORMALFRAME();
	}
	//3
	pass Pass_OtherTexFrame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OTHERTEXFRAME();
	}
	//4
	pass Pass_OtherTexSetColorFrame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OTHERTEXSETCOLORFRAME();
	}
	//5
	pass Pass_BloodSplainRG {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOODSTAIN_RG();
	}
	//6
	pass Pass_BloodSplainR {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOODSTAIN_R();
	}
	//7
	pass Pass_AlphatestColorBlood {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHATESTCOLORTBLOOD();
	}
	//8
	pass Pass_BloodParticle {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOODPARTICLE();
	}
	//9
	pass Pass_FootBlood {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FOOTBLOOD0();
	}
	//10
	pass Pass_FootBlood_N {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FOOTBLOOD_NORMAL();
	}
	//11
	pass Pass_ColorAMesh {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLORA();
	}
	//12
	pass Pass_RainDrop {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_RAINDROP();
	}
}