matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float3 g_vCameraPos = (float3)0.f;

float3 g_vLightPos = (float3)0.f;
float3 g_vLightDir = (float3)0.f;
float g_fLightRange = 0.f;
float4 g_vLightDiffuse = (float4)0.f;
float4 g_vLightAmbient = (float4)0.f;
float4 g_vLightSpecular = (float4)0.f;
float2 g_fLightAngle = (float2)0.f;

texture2D g_NormalTexture;
texture2D g_WorldTexture;

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
	float4 vShade : SV_TARGET0;
	float4 vSpecular : SV_TARGET1;
};

PS_OUT PS_DIR(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	vector vNormal = normalize(vector(g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f, 0.f));
	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vector(g_vLightDir, 0.f)) * -1.f, vNormal)) + g_vLightAmbient);
	Out.vShade.a = 1.f;

	vector vWorldPos = g_WorldTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vReflect = normalize(reflect(normalize(vector(g_vLightDir, 0.f)), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));
	Out.vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 30.f);
	Out.vSpecular.a = 1.f;
	return Out;
}
PS_OUT PS_POINT(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	vector vNormal = normalize(vector(g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f, 0.f));
	vector vWorldPos = g_WorldTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vLightDir = vWorldPos - vector(g_vLightPos, 1.f);
	float fLightDist = length(vLightDir);
	float fAtt = saturate((g_fLightRange - fLightDist) / g_fLightRange);
	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + g_vLightAmbient) * fAtt;
	Out.vShade.a = 1.f;
	vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));
	Out.vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 10.f) * fAtt;
	Out.vSpecular.a = 1.f;
	return Out;
}
PS_OUT PS_SPOT(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;

	vector vNormal = normalize(vector(g_NormalTexture.Sample(Sampler_Default, In.vTexCoord).xyz * 2.f - 1.f, 0.f));
	vector vWorldPos = g_WorldTexture.Sample(Sampler_Default, In.vTexCoord);
	vector vLightDir = vWorldPos - vector(g_vLightPos, 1.f);
	float fLightDist = length(vLightDir);
	float fAtt = saturate((g_fLightRange - fLightDist) / g_fLightRange);
	//걸러준다
	float fAngle = dot(normalize(vector(g_vLightDir, 0.f)), normalize(vLightDir));
	if (fAngle < g_fLightAngle.x)
		discard;
	float fAtt2 = (fAngle - g_fLightAngle.x) / (g_fLightAngle.y - g_fLightAngle.x);
	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + g_vLightAmbient) * fAtt * fAtt2;
	Out.vShade.a = 1.f;

	vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
	vector vLook = normalize(vWorldPos - vector(g_vCameraPos, 1.f));
	Out.vSpecular = g_vLightSpecular * pow(saturate(dot(vLook * -1.f, vReflect)), 10.f) * fAtt * fAtt2;
	Out.vSpecular.a = 1.f;
	return Out;
}
//-------------------------------------------------------------------------------------------------
RasterizerState RS_Default {
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
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

technique11	Technique_Light {
	pass Pass_Dir {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DIR();
	}
	pass Pass_Point {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_POINT();
	}
	pass Pass_Spot {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DEF_DWZ, 0);
		SetBlendState(BS_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SPOT();
	}
}