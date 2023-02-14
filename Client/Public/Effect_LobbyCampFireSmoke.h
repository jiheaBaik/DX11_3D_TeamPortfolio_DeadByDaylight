#pragma once
#include "Object.h"
BEGIN_NAMESPACE(Client)
class CEffect_LobbyCampFireSmoke final : public CObject{
	struct DirKeyFrame {
		_float	fDirTime = 0.f;
		_float3 fDir = { 0.f,0.f,0.f };

		_bool	bRandTime = false;
		_float2 fRandDirTimeRange = { 0.f,0.f };
		_bool	bRandDir = false;
		_float3 fRandDirRange[2] = { {0.f,0.f,0.f}, {0.f,0.f,0.f} };
	};
	struct ScaleKeyFrame {
		_float	fScaleTime = 0.f;
		_float3 fScale = { 0.f, 0.f,0.f };

		_bool	bRandTime = false;
		_float2 fRandScaleTimeRange = { 0.f,0.f };
		_bool	bRandScale = false;
		_float3 fRandScaleRange[2] = { {0.f,0.f,0.f}, {0.f,0.f,0.f} };
	};
	struct EffectTex {
		CTransform* pTransform = nullptr;
		CTexture* pDiffuseTex = nullptr;
		CTexture* pNormalTex = nullptr;
		CTexture* pNoiseTex = nullptr;
		CTexture* pTempTex = nullptr;
		CTexture* pDestTex = nullptr;

		_char		szDif[MAX_PATH] = "";
		_char		szNor[MAX_PATH] = "";
		_char		szNoise[MAX_PATH] = "";
		_char		szTemp[MAX_PATH] = "";
		_char		szDest[MAX_PATH] = "";

		// 위치크기회전
		_float3		fPos = { 0.f,0.f,0.f };
		_float3		fRot = { 0.f,0.f,0.f };
		_float3		fScale = { 0.01f,0.01f,0.01f };
		_bool		bRandPos = false;
		_bool		bRandRot = false;
		_bool		bRandScale = false;
		_bool		bLimitPos = false;
		_float3		fRandPos[2] = { {0.f,0.f,0.f},{0.f,0.f,0.f} };
		_float3		fLimitPos[2] = { {0.f,0.f,0.f},{0.f,0.f,0.f} };
		_float3		fRandRot[2] = { {0.f,0.f,0.f},{0.f,0.f,0.f} };
		_float3		fRandScale[2] = { {0.01f,0.01f,0.01f},{0.01f,0.01f,0.01f} };

		// 이동
		_float3		fEndPos = { 0.f,0.f,0.f };
		_float3		fDir = { 0.f,0.f,0.f };
		_bool		bDirType = false;
		_float3		fEndRot = { 0.f,0.f,0.f };
		_float3		fEndScale = { 0.f,0.f,0.f };
		_bool		bRot[3] = { false };
		_bool		bRandomEndPos = false;
		_bool		bRandomDir = false;
		_bool		bRandomEndRot = false;
		_bool		bRandomEndScale = false;
		_float3		fRandomPosRange[2] = { { 0.f,0.f,0.f },{ 0.f,0.f,0.f } };
		_float3		fRandomDirRange[2] = { { 0.f,0.f,0.f },{ 0.f,0.f,0.f } };
		_float3		fRandomRotRange[2] = { { 0.f,0.f,0.f },{ 0.f,0.f,0.f } };
		_float3		fRandomScaleRange[2] = { { 0.f,0.f,0.f },{ 0.f,0.f,0.f } };
		_bool		bMove = false;

		// 물리
		_bool		bPhysics = false;
		_bool		bRandAccel = false;
		_bool		bRandResist = false;
		_bool		bRandResistP = false;
		_float3		vSetAccel = { 0.f, 0.f ,0.f };
		_float3		vAccel = { 0.f,0.f,0.f };
		_float3		vRandAccel[2] = { {0.f,0.f,0.f},{0.f,0.f,0.f} };
		_float2		fResist = { 0.f, 0.f };
		_float2		fRandResist[2] = { {0.f, 0.f},{0.f, 0.f} };
		_float		fResistP = 0.f;
		_float		fRandResistP[2] = { 0.f,0.f };
		_float		fGravity = 0.f;
		_float		fGa = 0.f;
		_bool		bGravity = false;

		//시간
		_bool		bRandCreateTime = false;
		_bool		bRandProgressTime = false;
		_float2		fRandCreateTime = { 0.f, 0.f };
		_float2		fRandProgressTime = { 0.f, 0.f };
		_float		fCreateTime = 0.f;
		_float		fProgressTime = 0.f;
		_float		fFrameTime = 0.f;
		_float		fTime = 0.f;
		_float		fAlphaTime = 0.;

		// 셰이더
		_float4		fColor = { 0.f,0.f,0.f,0.f };
		_float4		fPlusColor = { 0.f,0.f,0.f,0.f };
		_float2		fGlow_Buffer = { 0.f,0.f };
		_float		fDissolveTime = 0.f;
		_float		fAlpha = 0.1f;
		_float		fAlphaSet = 0.1f;
		_int		iAlphaFrame = 0;
		_int		iShader = 0;
		_bool		bAlphaReverse = false;
		_bool		bAlphaGradation = false;
		_bool		bAllColor = false;
		_bool		bAllGlow = false;
		_bool		bAllPlusColor = false;
		_bool		bAllEndScale = false;
		_bool		bAllShader = false;

		// smoke
		_float3		fScrollSpeeds = { 0.f,0.f,0.f };
		_float3		fShadeScale = { 0.f,0.f,0.f };
		_float		fSmokeAlpha = 0.f;
		_float2		fDistortion1 = { 0.1f, 0.1f };
		_float2		fDistortion2 = { 0.1f, 0.1f };
		_float2		fDistortion3 = { 0.1f, 0.1f };
		_float2		fDistortion4 = { 0.1f, 0.1f };
		_float		fDiscardAlpha = 0.f;

		// uv
		_float2		fDivide = { 0.f, 0.f };
		_float2		fUV = { 0.f,0.f };
		_float2		fFixUV = { 0.f,0.f };
		_float		fsetUVY = 0.f;
		_float		fOtherSpeed = 0.f;
		_float		fFrameSpeed = 0.f;
		_float		fDefaultAlpha = 0.f;
		_int		iOtherFrame = 0;
		_float2		fFlowUV = { 0.f,0.f };
		_bool		bFrameLoop = false;
		_bool		bFix = false;

		//렌더
		_uint		iShaderPass = 0;
		_uint		iFrame = 0;
		_uint		iSetFrame = 0;
		_bool		bProj = false;
		_bool		bRender = true;
		_bool		bLookCam = false;

		// key frame
		vector<pair<_float, _float>> vecFrameAlpha;
		vector<pair<_float, _float4>> vecFrameColor;
		vector<pair<_float, _float3>> vecFramePos;
		vector<pair<_float, _float3>> vecFrameRot;
		vector<ScaleKeyFrame> vecFrameScale;
		vector<DirKeyFrame> vecFrameDir;
		_int iKeyFrame[6]{ 0, 0, 0, 0, 0 };
	};
public:
	static CEffect_LobbyCampFireSmoke* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, const _tchar* const& pFilePath = nullptr);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CEffect_LobbyCampFireSmoke(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEffect_LobbyCampFireSmoke(const CEffect_LobbyCampFireSmoke& rhs);
	virtual ~CEffect_LobbyCampFireSmoke(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const _tchar* const& pFilePath = nullptr);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;

public:
	void LoadEffect(const _tchar* const& pFilePath);
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;

public:
	virtual void Update_Network(const string& pkey) override;
	virtual void Update_Network(void) override;
private:
	CShader* m_pShader = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CBuffer_Rect* m_pRect = nullptr;
private:
	EffectTex m_sEffectTex;
	vector<EffectTex> m_vecEffect;
private:
	_float4x4	m_ProjMatrix;
	_float4x4	m_ViewMatrix;
private:
	_tchar	m_TransformKey[50][MAX_PATH] = { TEXT("") };
	_tchar	m_DiffuseKey[50][MAX_PATH] = { TEXT("") };
	_tchar	m_NormalKey[50][MAX_PATH] = { TEXT("") };
	_tchar	m_NoiseKey[50][MAX_PATH] = { TEXT("") };
	_tchar	m_TempKey[50][MAX_PATH] = { TEXT("") };
	_tchar	m_DestKey[50][MAX_PATH] = { TEXT("") };
	_char	m_szSave[MAX_PATH] = "";
	_char	m_szLoad[MAX_PATH] = "";
	// 시간
	_float	m_fMainEndTime = 0.f;
	_double m_dMainTime = 0.;
	_bool	m_bProgressLoop = false;

	_float	m_fX = 0;
	_float	m_fY = 0;
	_float	m_fSizeX = 0;
	_float	m_fSizeY = 0;

	_bool	m_bStart = false;
	_bool	m_bCreate = false;
	_bool	m_bRepeat = false;
	_int	m_iEffectIdx = 0;
	_int	m_iRenderIdx = 0;
	_bool	m_bLookCam = false;
	_bool	m_bSort = false;
	_bool	m_bZSort = false;
	class CTarget* m_pTarget_Depth = nullptr;
};
END_NAMESPACE
