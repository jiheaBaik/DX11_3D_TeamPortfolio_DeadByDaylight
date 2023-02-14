#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CEffect_Edit final : public CObject {
public:
	struct SEffect {
		class CTransform* pTransform = nullptr;
		char szModel[MAX_PATH] = "";
		class CModel* pModel = nullptr;
		char szTexture[MAX_PATH] = "";
		class CTexture* pTexture = nullptr;
		//�ð�
		_float2 fTime = { 0.f, 0.f };
		_float2 arrTime[2] = { _float2{ 0.f, 9999.f }, _float2{ 9999.f, 9999.f } };

		//�ʱ���ġ
		_float3 arrPos[2] = { _float3{ 0.f, 0.f, 0.f }, _float3{ 9999.f, 9999.f, 9999.f } };
		_float3 arrRot[2] = { _float3{ 0.f, 0.f, 0.f }, _float3{ 9999.f, 9999.f, 9999.f } };
		_float3 arrScale[2] = { _float3{ 1.f, 1.f, 1.f }, _float3{ 9999.f, 9999.f, 9999.f } };

		//����
		_float3 vAccel = _float3{ 0.f, 0.f, 0.f };
		_float3 arrAccel[2] = { _float3{ 0.f, 0.f, 0.f }, _float3{ 9999.f, 9999.f, 9999.f } };
		_float fGravity = 0.f;
		_float fResist = 0.f;

		//�̵�
		_float3 vPos = _float3{ 0.f, 0.f, 0.f };
		_float3 vRot = _float3{ 0.f, 0.f, 0.f };
		_float3 vScale = _float3{ 0.f, 0.f, 0.f };
		_float3 arrMovePos[2] = { _float3{ 0.f, 0.f, 0.f }, _float3{ 9999.f, 9999.f, 9999.f } };
		_float3 arrMoveRot[2] = { _float3{ 0.f, 0.f, 0.f }, _float3{ 9999.f, 9999.f, 9999.f } };
		_float3 arrMoveScale[2] = { _float3{ 0.f, 0.f, 0.f }, _float3{ 9999.f, 9999.f, 9999.f } };

		//�ִ�
		vector<pair<_float, _float3>> vecPos;
		vector<pair<_float, _float>> vecRot[3];
		vector<pair<_float, _float3>> vecScale;
		_int iFrame[5]{ 0, 0, 0, 0, 0 };
	};
public:
	static CEffect_Edit* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CEffect_Edit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEffect_Edit(const CEffect_Edit& rhs);
	virtual ~CEffect_Edit(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eRenderer) override;
public:

	void Create_Effect(SEffect& sEffect);
	void Create_Effect(void);
	void Init_Effect(void);

	void Save_Effect(const _tchar* const& pFilePath);
	void Load_Effect(const _tchar* const& pFilePath);
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader[2]{ nullptr, nullptr };
	class CBuffer_Rect* m_pRect = nullptr;
private:
	_float m_fTime[2]{ 0.f, 0.f };
	_float m_fEndTime[2] = { 9999.f, 9999.f };
	_bool m_bLoop = true;

private:
	SEffect m_sEffect;
	vector<SEffect> m_vecSEffect;
};
END_NAMESPACE