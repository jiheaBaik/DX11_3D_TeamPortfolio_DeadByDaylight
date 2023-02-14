sampler Sampler_Default = sampler_state{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
	//MaxAnisotropy = 16;
	//MinLOD = 0.0f;
	//MaxLOD = 3.402823466e+38f;	
};
sampler Sampler_Clamp = sampler_state{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler Sampler_Point = sampler_state {
	filter = min_mag_mip_point;
};
//-------------------------------------------------------------------------------------------------
/*
D3D11_FILL_MODE FillMode;
D3D11_CULL_MODE CullMode;
BOOL FrontCounterClockwise;
INT DepthBias;
FLOAT DepthBiasClamp;
FLOAT SlopeScaledDepthBias;
BOOL DepthClipEnable;
BOOL ScissorEnable;
BOOL MultisampleEnable;
BOOL AntialiasedLineEnable;
*/

RasterizerState RS_Default{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = false;
};
RasterizerState RS_FS_CF{
	FillMode = SOLID;
	CullMode = FRONT;
	FrontCounterClockwise = false;
};
RasterizerState RS_WireFrame{
	FillMode = WIREFRAME;
	CullMode = BACK;
	FrontCounterClockwise = false;
};
RasterizerState RS_Effect{
	FillMode = SOLID;
	CullMode = NONE;
	FrontCounterClockwise = false;
};

//-------------------------------------------------------------------------------------------------

/*
BOOL DepthEnable;
D3D11_DEPTH_WRITE_MASK DepthWriteMask;
DepthFunc;
BOOL StencilEnable;
UINT8 StencilReadMask;
UINT8 StencilWriteMask;
D3D11_DEPTH_STENCILOP_DESC FrontFace;
D3D11_DEPTH_STENCILOP_DESC BackFace;
*/

DepthStencilState DSS_Default {
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_DEF_DWZ{
	DepthEnable = false;
	DepthWriteMask = zero;
};
//-------------------------------------------------------------------------------------------------

/*
BOOL BlendEnable;
D3D11_BLEND SrcBlend;
D3D11_BLEND DestBlend;
D3D11_BLEND_OP BlendOp;
D3D11_BLEND SrcBlendAlpha;
D3D11_BLEND DestBlendAlpha;
D3D11_BLEND_OP BlendOpAlpha;
UINT8 RenderTargetWriteMask;
*/

BlendState BS_Default {
	BlendEnable[0] = false;
};


BlendState BS_AlphaBlend {
	//AlphaToCoverageEnable = true;

	BlendEnable[0] = true;
	SrcBlend[0] = src_alpha;
	DestBlend[0] = inv_src_alpha;
	BlendOp[0] = Add;
	SrcBlendAlpha = src_alpha;
	DestBlendAlpha = inv_src_alpha;
	BlendOpAlpha = Add;
};