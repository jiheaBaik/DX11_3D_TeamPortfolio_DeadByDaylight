#pragma once
BEGIN_NAMESPACE(Client)
class CGTest final : public CObject {
public:
	static CGTest* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CGTest* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CGTest(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CGTest(const CGTest& rhs);
	virtual ~CGTest(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CTexture* m_pTexture = nullptr;
	class CBuffer_Rect* m_pBuffer = nullptr;
private:
	_float2 m_vGlow_Buffer = { 0.f, 0.f };
};
END_NAMESPACE