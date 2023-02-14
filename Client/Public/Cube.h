#pragma once
BEGIN_NAMESPACE(Client)
class CCube final : public CObject {
public:
	static CCube* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCube(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCube(const CCube& rhs);
	virtual ~CCube(void) override = default;
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
	class CTexture* m_pDiffuseTex = nullptr;
	//class CTexture* m_pNormalTex = nullptr;
	class CBuffer_Cube* m_pBuffer = nullptr;
private:
	_char m_szDiffuse[MAX_PATH] = "";
	_char m_szSearchKey[MAX_PATH] = "";

	_float4 m_fColor = { 0.f,0.f,0.f,0.f };
	_uint m_iShaderPass = 0;
};
END_NAMESPACE