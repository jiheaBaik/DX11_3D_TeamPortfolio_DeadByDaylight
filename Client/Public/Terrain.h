#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CTerrain final : public CObject {
public:
	static CTerrain* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CTerrain(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain(void) override = default;
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
	class CShader* m_pShader = nullptr;
	class CBuffer_Terrain* m_pBuffer = nullptr;
	class CTexture* m_pTexture[5]{ nullptr };
	class CTexture* m_pTexture_Normal[5]{ nullptr };
private:
	_float m_fRoughness = 0.275f;
	_float m_fRefract = 0.275f;
	_float m_fMetalness = 0.f;
	_float m_fMetalness2 = 0.f;
};
END_NAMESPACE