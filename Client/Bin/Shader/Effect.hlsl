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

float2 g_flowUV;

float g_fTime;
float3 g_fScrollSpeeds;
float3 g_fScales;
float2 g_fDistortion1;
float2 g_fDistortion2;
float2 g_fDistortion3;
float g_fDiscardAlpha;
int	g_iAlphaColor;
float2 g_fWinSize;
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vWorldPos = Out.vPos.xyz;
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vTexCoord = In.vTexCoord;
	Out.vDepth = float2(Out.vPos.z, Out.vPos.w);
	return Out;
}
//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;
};
struct PS_OUT {
	float4 vDiffuse : SV_TARGET0;
	float4 vWorld : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
};
struct PS_OUT_GLOW {
	float4 vDiffuse : SV_TARGET0;
	float4 vBuffer : SV_TARGET1;
};
struct PS_OUT_ALPHABLEND {
	float4 vDiffuse : SV_TARGET0;
};
//Out.vBuffer = vector(g_vGlow_Buffer.x, g_vGlow_Buffer.y, 0.f, 1.f);

PS_OUT PS_MAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	if (Out.vDiffuse.a < 0.1f)
		discard;
	return Out;
}
PS_OUT PS_COLOR_MAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vDiffuse.xyz *= g_Color.xyz;
	//if (Out.vDiffuse.a < 0.1f)
	//	discard;
	return Out;
}
PS_OUT PS_BLOODMAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	
	Out.vDiffuse.a = Out.vDiffuse.r;
	Out.vDiffuse *= g_Color;

	if (Out.vDiffuse.r < g_fAlpha)
		discard;

	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	
	return Out;
}
PS_OUT PS_FRAME(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	/*Out.vDiffuse.a = Out.vDiffuse.r;*/
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	//if (Out.vDiffuse.a < g_fAlpha)
	//	discard;
	return Out;
}
PS_OUT PS_FRAME_A(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	//if (Out.vDiffuse.a < g_fAlpha)
	//	discard;
	return Out;
}
PS_OUT PS_COLORFRAME(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	if (Out.vDiffuse.r < g_fAlpha)
		discard;
	Out.vDiffuse.rgb *= g_Color.rgb;
	return Out;
}
PS_OUT PS_COLORFRAME_NA(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vDiffuse.a = Out.vDiffuse.r;
	
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	if (Out.vDiffuse.r < g_fAlpha)
		discard;
	Out.vDiffuse.rgb *= g_Color.rgb;
	return Out;
}
PS_OUT_ALPHABLEND PS_COLORFRAME_A(PS_IN In) {
	PS_OUT_ALPHABLEND Out = (PS_OUT_ALPHABLEND)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	if (Out.vDiffuse.r < 0.1f)
		discard;
	if (Out.vDiffuse.r < g_fAlpha)
		discard;
	Out.vDiffuse.rgb *= g_Color.rgb;
	return Out;
}
PS_OUT PS_HUNTRESSWEAPONSMOKE(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vDiffuse.a = Out.vDiffuse.r * g_fAlpha * 0.2f;

	Out.vDiffuse.rgb *= g_Color.rgb;

	return Out;
}
PS_OUT PS_FIXUV(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_FixTexCoord.x / g_TexCoordCut.x, g_FixTexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	if (Out.vDiffuse.a < 0.1f)
		discard;
	return Out;
}
PS_OUT_GLOW PS_COLORFRAME_AB(PS_IN In) {
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vBuffer = vector(g_fGlow_Buffer.x, g_fGlow_Buffer.y, 0.f, 1.f);

	if (Out.vDiffuse.r < 0.1f)
		discard;

	Out.vDiffuse.a -= g_fAlpha;
	Out.vDiffuse.xyz *= g_Color.xyz;
	return Out;
}
PS_OUT_GLOW PS_COLORWFRAME_AB(PS_IN In) {
	PS_OUT_GLOW Out = (PS_OUT_GLOW)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vBuffer = vector(g_fGlow_Buffer.x, g_fGlow_Buffer.y, 0.f, 1.f);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse.a -= g_fAlpha;
	Out.vDiffuse *= g_Color;
	return Out;
}
PS_OUT_ALPHABLEND PS_PLUSCOLORWFRAME_AB(PS_IN In) {
	PS_OUT_ALPHABLEND Out = (PS_OUT_ALPHABLEND)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));

	Out.vDiffuse.a -= g_fAlpha;
	Out.vDiffuse *= g_Color;
	Out.vDiffuse.xyz += g_PlusColor.xyz;
	return Out;
}
PS_OUT_ALPHABLEND PS_COLORFRAME_N(PS_IN In) {
	PS_OUT_ALPHABLEND Out = (PS_OUT_ALPHABLEND)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));

	Out.vDiffuse.a = Out.vDiffuse.r;
	Out.vDiffuse.a -= g_fAlpha;

	Out.vDiffuse *= g_Color;
	return Out;
}
PS_OUT PS_SOFTEFFECT(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	float2 vUV = In.vPos.xy;
	vUV.x /= g_fWinSize.x;
	vUV.y /= g_fWinSize.y;
	float fDepth = g_DepthTexture.Sample(Sampler_Default, vUV).x * 500.f;

	Out.vDiffuse.a = saturate(fDepth - In.vDepth.x);

	return Out;
}
PS_OUT PS_SOFTFRAMEEFFECT_NA(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	float2 vUV = In.vPos.xy;
	vUV.x /= g_fWinSize.x;
	vUV.y /= g_fWinSize.y;
	float fDepth = g_DepthTexture.Sample(Sampler_Default, vUV).x * 500.f;

	Out.vDiffuse.a = saturate(fDepth - In.vDepth.x) * Out.vDiffuse.r;
	Out.vDiffuse.a -= g_fAlpha;

	Out.vDiffuse *= g_Color;
	return Out;
}
PS_OUT PS_SOFTFRAMEEFFECT_A(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	float2 vUV = In.vPos.xy;
	vUV.x /= g_fWinSize.x;
	vUV.y /= g_fWinSize.y;
	float fDepth = g_DepthTexture.Sample(Sampler_Default, vUV).x * 500.f;

	Out.vDiffuse.a *= saturate(fDepth - In.vDepth.x);
	Out.vDiffuse.a -= g_fAlpha;

	Out.vDiffuse *= g_Color;
	Out.vDiffuse.xyz += g_PlusColor.xyz;
	return Out;
}
PS_OUT PS_SOFTFLOWEFFECT_NA(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, float2(In.vTexCoord.x + g_fTime * g_flowUV.x, In.vTexCoord.y + g_fTime * g_flowUV.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	float2 vUV = In.vPos.xy;
	vUV.x /= g_fWinSize.x;
	vUV.y /= g_fWinSize.y;
	float fDepth = g_DepthTexture.Sample(Sampler_Default, vUV).x * 500.f;

	Out.vDiffuse.a = saturate(fDepth - In.vDepth.x) * Out.vDiffuse.r;
	Out.vDiffuse.a -= g_fAlpha;

	Out.vDiffuse *= g_Color;
	Out.vDiffuse.xyz += g_PlusColor.xyz;
	return Out;
}
PS_OUT PS_SOFTFLOWEFFECT_A(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, float2(In.vTexCoord.x + g_fTime * g_flowUV.x, In.vTexCoord.y + g_fTime * g_flowUV.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);

	float2 vUV = In.vPos.xy;
	vUV.x /= g_fWinSize.x;
	vUV.y /= g_fWinSize.y;
	float fDepth = g_DepthTexture.Sample(Sampler_Default, vUV).x * 500.f;

	Out.vDiffuse.a *= saturate(fDepth - In.vDepth.x);
	Out.vDiffuse.a -= g_fAlpha;

	Out.vDiffuse *= g_Color;
	Out.vDiffuse.xyz += g_PlusColor.xyz;
	return Out;
}
//---------------------------------------------------------------------------------
// 연기
//---------------------------------------------------------------------------------
PS_OUT PS_OTHERALPHASMOKE(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vDiffuse.a = g_TempTexture.Sample(Sampler_Default, In.vTexCoord).r;

	Out.vDiffuse.a -= g_fAlpha;
	Out.vDiffuse *= g_Color;
	return Out;
}
PS_OUT PS_OTHERFRAMEALPHA_COLOR_NA(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vDiffuse.a = g_TempTexture.Sample(Sampler_Default, In.vTexCoord).r;

	Out.vDiffuse.a -= g_fAlpha;
	Out.vDiffuse *= g_Color;
	return Out;
}
PS_OUT PS_OTHERFRAMEALPHA_COLOR_A(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	Out.vDiffuse.a = g_TempTexture.Sample(Sampler_Default, In.vTexCoord).a;

	Out.vDiffuse.a -= g_fAlpha;
	Out.vDiffuse *= g_Color;
	return Out;
}
PS_OUT PS_OTHERALPHA_COLORFRAME_A(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	float4 AlphaTex = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	if (Out.vDiffuse.a < 0.1f)
		discard;
	if (g_fAlpha > AlphaTex.r)
		discard;
	Out.vDiffuse.rgb *= g_Color.rgb;
	return Out;
}
PS_OUT PS_OTHERFRAMEALPHA_COLORFRAME_NA(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	float4 AlphaTex = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	if (Out.vDiffuse.a < 0.1f)
		discard;
	if (g_fAlpha > AlphaTex.r)
		discard;
	Out.vDiffuse.rgb *= g_Color.rgb;
	return Out;
}
PS_OUT PS_OTHERALPHA_COLORFRAME(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.y) + float2(g_TexCoord.x / g_TexCoordCut.x, g_TexCoord.y / g_TexCoordCut.y));
	Out.vWorld = vector(In.vWorldPos.xyz, 1.f);
	Out.vDepth = vector(In.vDepth.x / 500.f, In.vDepth.x / In.vDepth.y, 0, 1.f);
	float4 AlphaTex = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);
	if (Out.vDiffuse.a < 0.1f)
		discard;
	if (g_fAlpha > AlphaTex.r)
		discard;
	Out.vDiffuse.rgb *= g_Color.rgb;
	return Out;
}
//-------------------------------------------------------------------------------------------------
///////////불
//-------------------------------------------------------------------------------------------------
struct VS_OUT_SHAKE {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;

	float2 vTexCoord1 : TEXCOORD3;
	float2 vTexCoord2 : TEXCOORD4;
	float2 vTexCoord3 : TEXCOORD5;
};

VS_OUT_SHAKE VS_MAIN_SHAKE(VS_IN In) {
	VS_OUT_SHAKE Out = (VS_OUT_SHAKE)0;

	vector vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);

	Out.vPos = vPos;
	Out.vTexCoord = In.vTexCoord;

	// 첫번째 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산합니다.
	Out.vTexCoord1 = (In.vTexCoord * g_fScales.x);
	Out.vTexCoord1.y = Out.vTexCoord1.y + (g_fTime * g_fScrollSpeeds.x);

	// 두번째 노이즈 텍스쳐의 좌표를 두번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산합니다.
	Out.vTexCoord2 = (In.vTexCoord * g_fScales.y);
	Out.vTexCoord2.y = Out.vTexCoord2.y + (g_fTime * g_fScrollSpeeds.y);

	return Out;
}

struct PS_IN_SHAKE {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float2 vDepth : TEXCOORD2;

	float2 vTexCoord1 : TEXCOORD3;
	float2 vTexCoord2 : TEXCOORD4;
	float2 vTexCoord3 : TEXCOORD5;
};
PS_OUT PS_SMOKE(PS_IN_SHAKE In) {
	PS_OUT Out = (PS_OUT)0;

	float4 noise1;
	float4 noise2;
	float4 noise3;
	float4 finalNoise;
	float2 noiseCoords;
	float4 DiffuseColor;
	float4 alphaColor;

	noise1 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord1);
	noise2 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord2);
	noise3 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord3);

	noise1 = (noise1 - 1.f) * 2.0f;
	noise2 = (noise2 - 1.f) * 2.0f;
	noise3 = (noise3 - 1.f) * 2.0f;

	float2 distort1 = g_fDistortion1;
	float2 distort2 = g_fDistortion2;
	float2 distort3 = g_fDistortion3;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨립니다.
	noise1.xy = noise1.xy * distort1;
	noise2.xy = noise2.xy * distort2;
	noise3.xy = noise3.xy * distort3;

	finalNoise = noise1 + noise2 + noise3;

	noiseCoords.xy = (finalNoise.xy) + In.vTexCoord.xy;

	DiffuseColor = g_DiffuseTexture.Sample(Sampler_Default, noiseCoords.xy);

	alphaColor = g_TempTexture.Sample(Sampler_Default, noiseCoords.xy);
	DiffuseColor.a = alphaColor.r;
	Out.vDiffuse = DiffuseColor;
	if (1-Out.vDiffuse.r < g_fAlpha)
		discard;
	//Out.vDiffuse.a = Out.vDiffuse.r;
	//Out.vDiffuse.xyz = 1 - Out.vDiffuse.xyz;
	return Out;
}

VS_OUT_SHAKE VS_MAIN_FIRESMOKESHAKE(VS_IN In) {
	VS_OUT_SHAKE Out = (VS_OUT_SHAKE)0;

	vector vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);

	Out.vPos = vPos;
	Out.vTexCoord = In.vTexCoord;

	// 첫번째 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산합니다.
	Out.vTexCoord1 = (In.vTexCoord * g_fScales.x);
	Out.vTexCoord1.y = Out.vTexCoord1.y + (g_fTime * g_fScrollSpeeds.x);

	// 두번째 노이즈 텍스쳐의 좌표를 두번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산합니다.
	Out.vTexCoord2 = (In.vTexCoord * g_fScales.y);
	Out.vTexCoord2.y = Out.vTexCoord2.y + (g_fTime * g_fScrollSpeeds.y);

	Out.vTexCoord3 = (In.vTexCoord * g_fScales.z);
	Out.vTexCoord3.y = Out.vTexCoord3.y + (g_fTime * g_fScrollSpeeds.z);
	return Out;
}
PS_OUT PS_FIRESMOKE(PS_IN_SHAKE In) {
	PS_OUT Out = (PS_OUT)0;

	float4 noise1;
	float4 noise2;
	float4 noise3;
	float4 finalNoise;
	float2 noiseCoords;
	float4 DiffuseColor;
	float4 alphaColor;

	noise1 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord1);
	noise2 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord2);
	noise3 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord3);

	noise1 = (noise1 - 1.f) * 2.0f;
	noise2 = (noise2 - 1.f) * 2.0f;
	noise3 = (noise3 - 1.f) * 2.0f;

	float2 distort1 = g_fDistortion1;
	float2 distort2 = g_fDistortion2;
	float2 distort3 = g_fDistortion3;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨립니다.
	noise1.xy = noise1.xy * distort1;
	noise2.xy = noise2.xy * distort2;
	noise3.xy = noise3.xy * distort3;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성합니다.
	finalNoise = noise1 + noise2 + noise3;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만듭니다.
	noiseCoords.xy = (finalNoise.xy) + In.vTexCoord.xy;

	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링합니다.
	// wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지합니다.
	DiffuseColor = g_DiffuseTexture.Sample(Sampler_Default, noiseCoords.xy);

	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링합니다.
	// 불꽃의 투명도를 지정하는 데 사용될 것입니다.
	// wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지합니다.
	alphaColor = g_TempTexture.Sample(Sampler_Default, noiseCoords.xy);
	DiffuseColor.a = DiffuseColor.r;
	Out.vDiffuse = DiffuseColor;
	if (Out.vDiffuse.r < g_fDiscardAlpha)
		discard;
	//Out.vDiffuse.xyz = float3(0.f,0.f,0.f);
	//Out.vDiffuse.a = Out.vDiffuse.r;
	//Out.vDiffuse.xyz = 1 - Out.vDiffuse.xyz;
	Out.vDiffuse.rgb *= g_Color.rgb;
	return Out;
}
//-------------------------------------------------------------------------------------------------
///////////가시걸릴때 테두리
//-------------------------------------------------------------------------------------------------
VS_OUT_SHAKE VS_SPIDERCRACK(VS_IN In) {
	VS_OUT_SHAKE Out = (VS_OUT_SHAKE)0;

	vector vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);

	// 기본 텍스처 두개(중앙 알파값 텍스처)
	Out.vPos = vPos;
	Out.vTexCoord = In.vTexCoord;

	// 움직는 빛용 텍스처
	Out.vTexCoord1 = (In.vTexCoord * g_fScales.x);
	Out.vTexCoord1.y = Out.vTexCoord1.y + (g_fTime * g_fScrollSpeeds.x);

	Out.vTexCoord2 = (In.vTexCoord * g_fScales.y);
	Out.vTexCoord2.y = Out.vTexCoord2.y + (g_fTime * g_fScrollSpeeds.y);

	return Out;
}
PS_OUT PS_SPIDERCRACK(PS_IN_SHAKE In) {
	PS_OUT Out = (PS_OUT)0;

	float4 DiffuseTex = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord * float2(1 / g_TexCoordCut.x, 1 / g_TexCoordCut.x) + float2(g_FixTexCoord.x / g_TexCoordCut.x, g_FixTexCoord.y / g_TexCoordCut.y));
	float4 NoiseTex1 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord1);
	float4 NoiseTex2 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord2);
	float AlphaTex = g_TempTexture.Sample(Sampler_Default, In.vTexCoord).r;

	Out.vDiffuse.a = 1 - AlphaTex.r;
	if (Out.vDiffuse.a < g_fAlpha)
		discard;
	if (DiffuseTex.r < 0.1f)
		discard;
	Out.vDiffuse.rgb = NoiseTex1.rgb;
	if (Out.vDiffuse.g < 0.1f)
		discard;
	return Out;
}
// 불똥
PS_OUT PS_FIREPARTICLE(PS_IN_SHAKE In) {
	PS_OUT Out = (PS_OUT)0;
	float4 noise1;
	float4 noise2;
	float4 noise3;
	float4 finalNoise;
	float2 noiseCoords;
	float4 DiffuseColor;
	float4 alphaColor;

	noise1 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord1 * 10);
	noise2 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord2 * 10);
	noise3 = g_NoiseTexture.Sample(Sampler_Default, In.vTexCoord3 * 10);

	noise1 = (noise1 - 1.f) * 2.0f;
	noise2 = (noise2 - 1.f) * 2.0f;
	noise3 = (noise3 - 1.f) * 2.0f;

	float2 distort1 = g_fDistortion1;
	float2 distort2 = g_fDistortion2;
	float2 distort3 = g_fDistortion3;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨립니다.
	noise1.xy = noise1.xy * distort1;
	noise2.xy = noise2.xy * distort2;
	noise3.xy = noise3.xy * distort3;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성합니다.
	finalNoise = noise1 + noise2 + noise3;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만듭니다.
	noiseCoords.xy = (finalNoise.xy) + In.vTexCoord.xy;

	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링합니다.
	// wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지합니다.
	DiffuseColor = g_DiffuseTexture.Sample(Sampler_Default, noiseCoords.xy * 0.1f);

	Out.vDiffuse.xyz += g_Color.xyz;
	return Out;
}
//---------------------------------------------------------------------------------
// 혈흔
//---------------------------------------------------------------------------------
PS_OUT PS_BLOODSTAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	float4 DiffuseTex = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	float4 ColorTex = g_DestTexture.Sample(Sampler_Default, In.vTexCoord);
	float4 AlphaTex = g_TempTexture.Sample(Sampler_Default, In.vTexCoord);

	Out.vDiffuse = DiffuseTex;

	if (g_iAlphaColor == 0) {
		if (g_fDiscardAlpha > AlphaTex.r)
			discard;
	}
	if (g_iAlphaColor == 1) {
		if (g_fDiscardAlpha > AlphaTex.g)
			discard;
	}
	if (g_iAlphaColor == 2) {
		if (g_fDiscardAlpha > AlphaTex.b)
			discard;
	}

	Out.vDiffuse.xyz *= g_Color.xyz;

	return Out;
}

//-------------------------------------------------------------------------------------------------
technique11 Technique_Default {
	//0
	pass Pass_Default {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	//1
	pass Pass_BloodMain {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOODMAIN();
	}
	//2
	pass Pass_Smoke {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_FIRESMOKESHAKE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FIRESMOKE();
	}
	//3
	pass Pass_Frame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FRAME();
	}
	//4
	pass Pass_ColorFrame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLORFRAME();
	}
	//5
	pass Pass_ColorFrame_A {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLORFRAME_A();
	}
	//6
	pass Pass_ProjSmoke {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SHAKE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SMOKE();
	}
	//7
	pass Pass_SpiderCrack {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_SPIDERCRACK();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SPIDERCRACK();
	}
	//8
	pass Pass_BloodStain {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOODSTAIN();
	}
	//9
	pass Pass_Frame_A {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FRAME_A();
	}
	//10
	pass Pass_OtherAlphaSmoke {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OTHERALPHASMOKE();
	}
	//11
	pass Pass_HuntressSmoke {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HUNTRESSWEAPONSMOKE();
	}
	//12
	pass Pass_HuntressParticle {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FIXUV();
	}
	//13
	pass Pass_FireParticle {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_SHAKE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FIREPARTICLE();
	}
	//14
	pass Pass_SetColorTex {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLOR_MAIN();
	}
	//15
	pass Pass_SmokeColorFrame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLORFRAME_NA();
	}
	//16
	pass Pass_OtherAlphaColorFrame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OTHERALPHA_COLORFRAME();
	}
	//17
	pass Pass_OtherAlphaColorFrame_a {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OTHERALPHA_COLORFRAME_A();
	}
	//18
	pass Pass_ColorFrame_AB_NoDepth {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PLUSCOLORWFRAME_AB();
	}
	//19
	pass Pass_ParticleColorFrame_AB {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLORFRAME_AB();
	}
	//20
	pass Pass_ParticleColorwFrame_AB {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLORWFRAME_AB();
	}
	//21
	pass Pass_SmokeColorAlphaFrame {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COLORFRAME_N();
	}
	//22
	pass Pass_SoftTest {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SOFTEFFECT();
	}
	//23
	pass Pass_SoftFrame_NA {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SOFTFRAMEEFFECT_NA();
	}
	//24
	pass Pass_SoftFrame_A {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SOFTFRAMEEFFECT_A();
	}
	//25
	pass Pass_Soft_OtherFrameAlpha_NA {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OTHERFRAMEALPHA_COLOR_NA();
	}
	//25
	pass Pass_Soft_OtherFrameAlpha_A {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OTHERFRAMEALPHA_COLOR_A();
	}
	//26
	pass Pass_PlusColorwFrame_AB {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_PLUSCOLORWFRAME_AB();
	}
	//27
	pass Pass_SoftFlow_NA {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SOFTFLOWEFFECT_NA();
	}
	//28
	pass Pass_SoftFlow_A {
		SetRasterizerState(RS_Effect);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SOFTFLOWEFFECT_A();
	}
}; 