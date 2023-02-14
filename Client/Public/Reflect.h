#pragma once
BEGIN_NAMESPACE(Client)
class CReflect final : public CEnviro {
public:
	static CReflect* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CReflect(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CReflect(const CReflect& rhs);
	virtual ~CReflect(void) override = default;
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
	class CBuffer_Cube* m_pBuffer = nullptr;

	CCamera* m_pCamera[6]{ nullptr };
	_bool m_bRender = false;

	ID3D11Texture2D* m_pTexture[6]{ nullptr };
	ID3D11ShaderResourceView* m_pSRV[6]{ nullptr };

	ID3D11Texture2D* m_pTextureCube = nullptr;
};
END_NAMESPACE