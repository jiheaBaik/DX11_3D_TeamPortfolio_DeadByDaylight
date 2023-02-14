#pragma once

namespace Engine {
	//버텍스
	typedef struct SVertex {
		XMFLOAT3 vPos;
	}VTX;
	typedef struct DECLSPEC_DLL SVertex_Declaration {
		static const unsigned int iNumElement = 1;
		static D3D11_INPUT_ELEMENT_DESC Input_Element_Desc[iNumElement];
	}VTX_DECLARATION;

	//텍스쳐
	typedef struct SVertexTexture {
		XMFLOAT3 vPos;
		XMFLOAT2 vTexCoord;
	}VTXTEX;
	typedef struct DECLSPEC_DLL SVertexTexture_Declaration {
		static const unsigned int iNumElement = 2;
		static D3D11_INPUT_ELEMENT_DESC Input_Element_Desc[iNumElement];
	}VTXTEX_DECLARATION;

	//노말 텍스쳐 
	typedef struct SVertexNormalTexture {
		XMFLOAT3 vPos;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexCoord;
		XMFLOAT2 vTexCoord1;
	}VTXNORTEX;
	typedef struct DECLSPEC_DLL SVertexNormalTexture_Declaration {
		static const unsigned int iNumElement = 4;
		static D3D11_INPUT_ELEMENT_DESC Input_Element_Desc[iNumElement];
	}VTXNORTEX_DECLARATION;

	//큐브 텍스쳐
	typedef struct SVertexCubeTexture {
		XMFLOAT3 vPos;
		XMFLOAT3 vTexCoord;
	}VTXCUBETEX;
	typedef struct DECLSPEC_DLL SVertexCubeTexture_Declaration {
		static const unsigned int iNumElement = 2;
		static D3D11_INPUT_ELEMENT_DESC Input_Element_Desc[iNumElement];
	}VTXCUBETEX_DECLARATION;

	//모델
	typedef struct SVertexModel {
		XMFLOAT3 vPos;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexCoord;
		XMFLOAT3 vTangent;
	}VTXMODEL;
	typedef struct DECLSPEC_DLL SVertexModel_Declaration {
		static const unsigned int iNumElement = 4;
		static D3D11_INPUT_ELEMENT_DESC Input_Element_Desc[iNumElement];
	}VTXMODEL_DECLARATION;
	typedef struct SVertexModelInst {
		XMFLOAT4 vRight;
		XMFLOAT4 vUp;
		XMFLOAT4 vLook;
		XMFLOAT4 vTrans;
	}VTXMODELINST;
	typedef struct DECLSPEC_DLL SVertexModelInstance_Declaration {
		static const unsigned int iNumElement = 8;
		static D3D11_INPUT_ELEMENT_DESC Input_Element_Desc[iNumElement];
	}VTXMODELINST_DECLARATION;

	//애니 모델
	typedef struct SVertexAnimModel {
		XMFLOAT3 vPos;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexCoord;
		XMFLOAT3 vTangent;

		XMUINT4 vBlendIndex;
		XMFLOAT4 vBlendWeight;
	}VTXANIMMODEL;
	typedef struct DECLSPEC_DLL SVertexAnimModel_Declaration {
		static const unsigned int iNumElement = 6;
		static D3D11_INPUT_ELEMENT_DESC Input_Element_Desc[iNumElement];
	}VTXANIMMODEL_DECLARATION;

	typedef struct SIndex_Face16 {
		unsigned short _1, _2, _3;
	}IDXFACE16;
	typedef struct SIndex_Face32 {
		unsigned long _1, _2, _3;
	}IDXFACE32;
	typedef struct SIndex_Line16 {
		unsigned short _1, _2;
	}IDXLINE16;
	typedef struct SIndex_Line32 {
		unsigned long _1, _2;
	}IDXLINE32;
}

