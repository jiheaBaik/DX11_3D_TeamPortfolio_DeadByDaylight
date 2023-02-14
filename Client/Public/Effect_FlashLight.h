#pragma once
#include "Object.h"
BEGIN_NAMESPACE(Client)
class CEffect_FlashLight final : public CObject {
	struct EffectModel {
		CTransform* pTransform = nullptr;
		CModel* pModel1 = nullptr;
		CModel* pModel2 = nullptr;
		CModel* pModel3 = nullptr;
		CModel* pModel4 = nullptr;
		CTexture* pTempTex = nullptr;
		CTexture* pDestTex = nullptr;

		_char		szModel1[MAX_PATH] = "";
		_char		szModel2[MAX_PATH] = "";
		_char		szModel3[MAX_PATH] = "";
		_char		szModel4[MAX_PATH] = "";
		_char		szTemp[MAX_PATH] = "";
		_char		szDest[MAX_PATH] = "";

		// 위치크기회전
		_float3		fPos = { 0.f,0.f,0.f };
		_float3		fRot = { 0.f,0.f,0.f };
		_float3		fScale = { 0.01f,0.01f,0.01f };

		// 월드전용
		_bool		bMove = false;
		_float3		fEndPos = { 0.f,0.f,0.f };
		_float3		fDir = { 0.f,0.f,0.f };
		_bool		bDirType = false;
		_float3		fEndRot = { 0.f,0.f,0.f };
		_float3		fEndScale = { 0.f,0.f,0.f };
		_bool		bRot[3] = { false };

		// 물리
		_bool		bPhysics = false;
		_float3		vSetAccel = { 0.f, 0.f ,0.f };
		_float3		vAccel = { 0.f,0.f,0.f };
		_float2		fResist = { 0.f, 0.f };
		_float		fResistP = 0.f;
		_float		fGravity = 0.f;
		_float		fGa = 0.f;
		_bool		bGravity = false;

		//시간
		_float		fCreateTime = 0.f;
		_float		fDeleteTime = 1000.f;
		_float		fFrameTime = 0.f;
		_float		fTime = 0.f;
		_float		fAlphaTime = 0.;

		// 셰이더
		_float4		fColor = { 0.f,0.f,0.f,0.f };
		_float		fDissolveTime = 0.f;
		_float		fAlpha = 0.f;
		_float		fAlphaSet = 0.1f;
		_int		iAlphaFrame = 0;
		_int		iShader = 0;
		_bool		bAlphaReverse = false;
		_bool		bSetColor = false;
		_float		fDissolveLength = 0.f;

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
		_bool		bFix = false;
		_float		fsetUVY = 0.f;
		_float		fOtherSpeed = 0.f;
		_float		fFrameSpeed = 0.f;
		_float		fDefaultAlpha = 0.1f;
		_int		iOtherFrame = 0;
		
		//렌더
		_uint		iShaderPass = 0;
		_uint		iFrame = 0;
		_uint		iSetFrame = 0;
		_bool		bRender = true;
		
		_float3		fRight = { 0.f,0.f,0.f };
		_float3		fUp = { 0.f,0.f,0.f };
		_float3		fLook = { 0.f,0.f,0.f };
	
		//키프레임
		vector<pair<_float, _float>> vecFrameAlpha;
		vector<pair<_float, _float3>> vecFrameScale;
		_int iKeyFrame[5]{ 0, 0, 0, 0, 0 };

	};
public:
	static CEffect_FlashLight* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, const _tchar* const& pFilePath);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CEffect_FlashLight(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEffect_FlashLight(const CEffect_FlashLight& rhs);
	virtual ~CEffect_FlashLight(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(const _tchar* const& pFilePath);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
	void LoadEffect(const _tchar* const& pFilePath);
public:
	void LightOff();
	void LightOn();
	const _bool& Get_OnOff() { return m_bOnOff; }
public:
	virtual void Update_Network(const string& pkey) override;
	virtual void Update_Network(void) override;
private:
	CShader* m_pShader = nullptr;
	CShader* m_pAnimShader = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollider* m_pCollider = nullptr;
private:
	EffectModel m_sEffectModel;
	vector<EffectModel> m_vecEffect;
private:
	_float4x4	m_ProjMatrix;
	_float4x4	m_ViewMatrix;
private:
	_tchar	m_TransformKey[50][MAX_PATH] = { TEXT("") };
	_tchar	m_Model1Key[50][MAX_PATH] = { TEXT("") };
	_tchar	m_Model2Key[50][MAX_PATH] = { TEXT("") };
	_tchar	m_Model3Key[50][MAX_PATH] = { TEXT("") };
	_tchar	m_Model4Key[50][MAX_PATH] = { TEXT("") };
	_tchar	m_TempKey[50][MAX_PATH] = { TEXT("") };
	_tchar	m_DestKey[50][MAX_PATH] = { TEXT("") };
	_char	m_szSave[MAX_PATH] = "";
	_char	m_szLoad[MAX_PATH] = "";

	// 시간
	_float	m_fTime = 0.;
	_float	m_fMainEndTime = 0.f;
	_double m_dMainTime = 0.;
	_bool	m_bProgressLoop = false;
	_bool	m_bLookCam = false;
	_float	m_fX = 0;
	_float	m_fY = 0;
	_float	m_fSizeX = 0;
	_float	m_fSizeY = 0;

	_bool	m_bStart = false;
	_bool	m_bCreate = false;
	_bool	m_bRepeat = false;
	_int	m_iEffectIdx = 0;
	_int	m_iRenderIdx = 0;

	
	_bool	m_bOnOff = false;
};
END_NAMESPACE
