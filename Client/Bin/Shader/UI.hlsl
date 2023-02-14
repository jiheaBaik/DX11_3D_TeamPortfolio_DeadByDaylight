#include "Shader_Define.hlsli"

bool g_bBothRange;

float g_fTime;
float g_fLoading;
float g_fMaxTime;
float g_fAlpha;
float g_fDissolve;
float g_fScrollX, g_fScrollY;
float g_fDelta;
float g_fAngle;

float2 g_vScrollSpd;
float2 g_vSpread;
float2 g_vScales;
float2 g_vAngle;
float2 g_vTrans;
float4 g_vColor;
float2 g_vTexSize;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_DestTexture;
texture2D g_NoiseTexture;
texture2D g_FilterTexture;
texture2D g_DissolveTexture;
texture2D g_FlowTexture;
//-------------------------------------------------------------------------------------------------
struct VS_IN {
	float3 vPos : POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct VS_OUT {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct VS_OUT_DOUBLE {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.vTexCoord = In.vTexCoord;
	return Out;
}

VS_OUT VS_ROTATE(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);
	Out.vTexCoord = In.vTexCoord;

	// ccw [cos -sin / sin cos]
	// cw  [cos sin / -sin cos]
	//float2x2 matRot = float2x2(g_vAngle.x, -g_vAngle.y, g_vAngle.y, g_vAngle.x);
	float2x2 matRot = float2x2(cos(g_fAngle), -sin(g_fAngle), sin(g_fAngle), cos(g_fAngle));
	float2 uvRot = mul(float2(In.vTexCoord.x - 0.5f, In.vTexCoord.y - 0.5f), matRot);
	//uvRot.x += 0.5f;
	//uvRot.y += 0.5f;
	Out.vTexCoord = float2(uvRot.x + 0.5f, uvRot.y + 0.5);
	return Out;
}

VS_OUT_DOUBLE VS_FLOW(VS_IN In) {
	VS_OUT_DOUBLE Out = (VS_OUT_DOUBLE)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.vTexCoord = float2(In.vTexCoord.x - g_vTrans.x, In.vTexCoord.y);
	Out.vTexCoord1 = float2(In.vTexCoord.x - g_vTrans.y, In.vTexCoord.y);
	return Out;
}

VS_OUT_DOUBLE VS_OVERLAP(VS_IN In) {
	VS_OUT_DOUBLE Out = (VS_OUT_DOUBLE)0;
	Out.vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(Out.vPos, g_ViewMatrix);
	Out.vPos = mul(Out.vPos, g_ProjMatrix);

	Out.vTexCoord = In.vTexCoord;
	Out.vTexCoord1 = In.vTexCoord /** g_vScales*/;

	Out.vTexCoord1.x = Out.vTexCoord1.x + (1.0f - g_fTime);
	//Out.vTexCoord1.y = Out.vTexCoord1.y + (1.0f - g_fTime);/* g_vScrollSpd.y*/
	return Out;
}

//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct PS_IN_DOUBLE {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
};
struct PS_OUT {
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vColor *= g_vColor;
	if (Out.vColor.a < 0.1f)
		discard;
	return Out;
}
PS_OUT PS_ALPHABLEND(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vColor *= g_vColor;
	if (Out.vColor.a < g_fAlpha)
		discard;
	return Out;
}

PS_OUT PS_DECREASE(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Clamp, In.vTexCoord);
	Out.vColor *= g_vColor;
	if (In.vTexCoord.x > g_fScrollX && In.vTexCoord.y > g_fScrollY)
		discard;
	return Out;
}

PS_OUT PS_FLOW(PS_IN_DOUBLE In) {
	PS_OUT Out = (PS_OUT)0;	
	float4 vSample = g_DiffuseTexture.Sample(Sampler_Clamp, In.vTexCoord);
	float4 vFlow = g_FlowTexture.Sample(Sampler_Clamp, In.vTexCoord1);
	vFlow = g_vColor;
	Out.vColor = vSample * vFlow;
	Out.vColor *= g_fDelta;
	return Out;
}

// ratio = fA * (1.0f / fB);
PS_OUT PS_SPREAD(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vColor *= g_vColor;
	if ((abs(g_vSpread.x - In.vTexCoord.x) > g_vSpread.y) && (abs(g_vSpread.x - In.vTexCoord.y) > g_vSpread.y))
		discard;
	return Out;
}

PS_OUT PS_OVERLAP(PS_IN_DOUBLE In) {
	PS_OUT Out = (PS_OUT)0;
	float4 vSour = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	vSour *= g_vColor;
	float4 vDest = g_DestTexture.Sample(Sampler_Clamp, In.vTexCoord1);	
	//float4 vMix = vSour * vDest;
	//Out.vColor = vMix;
	vSour.rgb *= vDest.rgb;
	Out.vColor = vSour;
	return Out;
}

PS_OUT PS_LOADING(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vColor *= g_vColor;
	if (In.vTexCoord.y >= g_fLoading)
		Out.vColor.rgb *= 0.3f;
	if (Out.vColor.a < g_fAlpha)
		discard;
	return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vColor *= g_vColor;
	if (0.f < g_fDissolve) {
		float4 vDissolve = g_DissolveTexture.Sample(Sampler_Default, In.vTexCoord);
		float4 vResult = Out.vColor * vDissolve;
		if (vResult.x < g_fDissolve || vResult.y < g_fDissolve || vResult.z < g_fDissolve)
			discard;
	}	
	return Out;
}

//PS_OUT PS_COLOR(PS_IN In) {
//	PS_OUT Out = (PS_OUT)0;
//	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
//	Out.vColor = g_vColor;
//	return Out;
//}

PS_OUT PS_COOLDOWN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Clamp, In.vTexCoord);
	Out.vColor *= g_vColor;
	
	In.vTexCoord = In.vTexCoord * 2.f - 1.f;	// pivot to center(-0.5f, -0.5f)
	float fAngle = atan2(-In.vTexCoord.x, In.vTexCoord.y);// cw, atan(x,y) = pi/2 - atan(y,x)
	fAngle += radians(180.f); // starting line
	//fAngle = fmod(fAngle, radians(180.f) * 2.f); // +-2/pi ~> 0-360
	if (fAngle < g_fAngle)
		discard;

	return Out;
}

//-------------------------------------------------------------------------------------------------
BlendState	BS_Add {
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = one;
	DestBlend = one;
	Blendop = add;
};
technique11 Technique_Default {
	pass Pass_Default { // 0
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_AlpahBlend { // 1
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();
	}
	pass Pass_Spread { // 2
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SPREAD();
	}
	pass Pass_Flow { // 3
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_FLOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_FLOW();
	}
	pass Pass_Decrease { // 4
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DECREASE();
	}
	pass Pass_Loading {	//made in cheol to loading flow
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LOADING();
	}
	pass Pass_Overlap { // 6
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_OVERLAP();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OVERLAP();
	}
	pass Pass_Dissolve { // 7
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}
	pass Pass_Cooldown { // 8
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_COOLDOWN();
	}
	pass Pass_Rotate { // 9
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_ROTATE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();
	}
};