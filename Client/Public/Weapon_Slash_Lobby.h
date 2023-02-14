#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CWeapon_Slash_Lobby final : public CObject {
public:
	static CWeapon_Slash_Lobby* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CWeapon_Slash_Lobby(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CWeapon_Slash_Lobby(const CWeapon_Slash_Lobby& rhs);
	virtual ~CWeapon_Slash_Lobby(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_Dissolve(const _float& m_fGoal);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;

	class CTexture* m_Texture_pDissolve = nullptr;
	class CTexture* m_Texture_pAlpha = nullptr;
private:
	_float m_fDissolveLength = 0.f;
	_float3 m_fDissolveColor = { 8.f, 0.f, 0.f };
	_float3 m_fDissolveColor2 = { 8.f, 1.5f, 0.f };
	_float m_fGoalDissolveLength = 0.f;
private:
	_float m_fRoughness = 0.1f;
	_float m_fRefract = 0.375f;
	_float m_fMetalness = 0.15f;
	_float m_fMetalness2 = 0.15f;
};
END_NAMESPACE