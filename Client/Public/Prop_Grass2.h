#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CProp_Grass2 final : public CObject {
public:
	static CProp_Grass2* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CProp_Grass2(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CProp_Grass2(const CProp_Grass2& rhs);
	virtual ~CProp_Grass2(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
	class CAudio* m_pAudio = nullptr;
private:
	_vector m_vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_matrix m_mMatirx = XMMatrixIdentity();
	_bool m_bSavePos = false;
	_bool m_bRestore = false;
	_bool m_bAudio = false;
	_bool m_bKeep = false;
	_float m_fKeep = 0.f;
};
END_NAMESPACE