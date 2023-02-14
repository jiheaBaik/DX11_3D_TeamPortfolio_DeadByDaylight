#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CAnimator final : public CObject {
public:
	typedef struct NewModel {
		CTransform* m_pTransform = nullptr;
		CRenderer* m_pRenderer = nullptr;
		CShader* m_pShader = nullptr;
		CModel* m_pModel = nullptr;

		// 애니메이션 목록, 뼈 목록
		_int m_iAnimCuridx = 0;
		_int m_iBoneCuridx = 0;
		_uint m_iAnimCount = 0;
		_uint m_iBoneCount = 0;
		vector<const _char*>	m_vecAnimName;
		vector<const _char*>	m_vecBoneName;

		_bool m_bAnimPlay = true;
		_bool m_bAnimStop = false;

		_double m_dAnimSpeed = 1.;
		_float	m_fAnimeSpeed = 1.f;
		_float	m_fAnimKeyFrame = 0.f;
		_float	m_fAnimDuration = 0.f;
	}NEWMODEL;
public:
	typedef struct SaveInfo {
		const _tchar* m_szSoundKey;
		const _tchar* m_szEffectKey;
	}SAVEINFO;
public:
	static CAnimator* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CAnimator(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CAnimator(const CAnimator& rhs);
	virtual ~CAnimator(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	HRESULT DeleteModel();
	HRESULT DirectDeleteModel();
	void AnimNameIdx();
	void BoneNameIdx();
private:
	NEWMODEL* m_sModel1 = nullptr;
	CTexture* m_pDissolveShapeTexture = nullptr;
	CTexture* m_pDissolveAlphaTexture = nullptr;
	CTexture* m_pDissolveColorTexture = nullptr;
	CTransform* m_pTransformm = nullptr;
	_float		m_fDissolveLength = 0.f;
	_uint		m_iModelIdx = 0;
	_tchar* m_ComponentKey[4] = { nullptr };
	_bool		m_bRoot = false;
	_bool		m_bRepeat = true;
	_bool		m_bRootPos = false;
	_double		m_dRootTime = 0.;
	_uint		m_iAnimEnd = 0;
	_uint		m_iKeyFrame = 0;
	_double		m_dTickperSec = 0.;
	_float3		m_fRootPrePos = { 0.f,0.f,0.f };
	_float4x4	m_fRootOffsetMatrix;
	vector<const _char*> m_vecBoneName;
	vector<_float4> m_vecAnimRootPos;
	vector<_double> m_vecdTime;
	map<const _char*, vector<_float4>> m_mSaveRootPos;
	_char		m_szAnimName[MAX_PATH];
};
END_NAMESPACE


