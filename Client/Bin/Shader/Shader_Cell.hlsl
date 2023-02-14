

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vColor = (vector)0.f;

float3 g_vCameraPos = (float3)0.f;

//-------------------------------------------------------------------------------------------------

struct VS_IN {
	float3 vPos : POSITION;
	float4 vColor : COLOR0;
};

struct VS_OUT {
	float4 vPos : SV_POSITION;
	float4 vColor : COLOR0;
};


VS_OUT VS_MAIN(VS_IN In) {
	VS_OUT Out = (VS_OUT)0;

	vector vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);

	Out.vPos = vPos;
	Out.vColor = In.vColor;

	return Out;
}
//-------------------------------------------------------------------------------------------------

struct PS_IN {
	float4 vPos : SV_POSITION;
	float4 vColor: COLOR0;
};

struct PS_OUT {
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In) {
	PS_OUT Out = (PS_OUT)0;
	Out.vColor =  g_vColor;
	return Out;
}
//-------------------------------------------------------------------------------------------------

DepthStencilState DDS_Cell {
	DepthEnable = false;
	DepthWriteMask = zero;
};

technique11	DefaultTechnique {
	pass Pass_Default {
		SetDepthStencilState(DDS_Cell, 0);
		//SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
