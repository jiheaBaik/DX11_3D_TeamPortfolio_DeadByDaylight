matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float3 g_vCameraPos = (float3)0.f;

float3 g_vLightPos = (float3)0.f;
float3 g_vLightDir = (float3)0.f;
float g_fLightRange = 0.f;
float4 g_vLightDiffuse = (float4)0.f;
float4 g_vLightAmbient = (float4)0.f;
float4 g_vLightSpecular = (float4)0.f;
float g_fLightAngle = 0.f;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_WorldTexture;
texture2D g_ShadeTexture;
texture2D g_SpecularTexture;

sampler Sampler_Default = sampler_state {
	filter = min_mag_mip_linear;
};
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
//-------------------------------------------------------------------------------------------------
struct PS_IN {
	float4 vPos : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};
struct PS_OUT {
	float4 vColor : SV_TARGET0;
};
struct PS_OUT_LIGHT {
	float4 vShade : SV_TARGET0;
	float4 vSpecular : SV_TARGET1;
};
struct PS_IN_SHADOW_BLUR {
	float4 vPos : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	return Out;
}
PS_OUT_LIGHT PS_LIGHTDIR(PS_IN In) {
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormal = normalize(vector(g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f, 0.f));
	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vector(g_vLightDir, 0.f)) * -1.f, vNormal)) + g_vLightAmbient);

	vector vWorldPos = g_WorldTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vReflect = normalize(reflect(normalize(vector(g_vLightDir, 0.f)), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));
	Out.vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 30.f);
	Out.vSpecular.a = 0.f;
	return Out;
}
PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In) {
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormal = normalize(vector(g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f, 0.f));
	vector vWorldPos = g_WorldTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vLightDir = vWorldPos - vector(g_vLightPos, 1.f);
	float fLightDist = length(vLightDir);
	float fAtt = saturate((g_fLightRange - fLightDist) / g_fLightRange);
	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + g_vLightAmbient) * fAtt;

	vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));
	Out.vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 10.f) * fAtt;
	Out.vSpecular.a = 0.f;
	return Out;
}
PS_OUT_LIGHT PS_MAIN_LIGHT_SPOT(PS_IN In) {
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormal = normalize(vector(g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f, 0.f));
	vector vWorldPos = g_WorldTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vLightDir = vWorldPos - vector(g_vLightPos, 1.f);
	float fLightDist = length(vLightDir);
	float fAtt = saturate((g_fLightRange - fLightDist) / g_fLightRange);
	//걸러준다
	float fAngle = dot(normalize(vector(g_vLightDir, 0.f)), normalize(vLightDir));
	if (fAngle < g_fLightAngle)
		discard;
	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + g_vLightAmbient) * fAtt;

	vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));
	Out.vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 10.f) * fAtt;
	Out.vSpecular.a = 0.f;
	return Out;
}
PS_OUT PS_MAIN_BLEND(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	vector vDiffuse = g_DiffuseTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vShade = g_ShadeTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vSpecular = g_SpecularTexture.Sample(Sampler_Default, In.vTexCoord);
	Out.vColor = vDiffuse * vShade + vSpecular;
	if (Out.vColor.a == 0.f)
		discard;
	return Out;
}
//-------------------------------------------------------------------------------------------------
RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};
DepthStencilState DSS_Default {
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};
DepthStencilState DSS_DEF_DWZ {
	DepthEnable = false;
	DepthWriteMask = zero;
};
BlendState	BS_Default {
	BlendEnable[0] = false;
};
BlendState	BS_Add {
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = one;
	DestBlend = one;
	Blendop = add;
};
BlendState BS_AlphaBlend {
	BlendEnable[0] = true;
	SrcBlend[0] = src_alpha;
	DestBlend[0] = inv_src_alpha;
	BlendOp[0] = Add;
};

technique11	Technique_Default {
	pass Pass_Debug {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}
	pass Pass_LightDir {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_LIGHTDIR();
	}
	pass Pass_LightPoint {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}
	pass Pass_LightSpot {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_SPOT();
	}
	pass Pass_Blend {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}
}