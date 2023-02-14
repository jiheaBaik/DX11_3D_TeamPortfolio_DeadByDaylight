#pragma once
BEGIN_NAMESPACE(Client)
class CCube_Free final : public CObject {
public:
	static CCube_Free* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCube_Free(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCube_Free(const CCube_Free& rhs);
	virtual ~CCube_Free(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_Pos(_fvector pos);
	const _float Get_Dist();
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CTexture* m_pDiffuseTex = nullptr;
	class CBuffer_Cube* m_pBuffer = nullptr;

	class CTarget* m_pTarget_World = nullptr;
	class CTarget* m_pTarget_Normal = nullptr;

private:
	_char m_szDiffuse[MAX_PATH] = "";
	_char m_szSearchKey[MAX_PATH] = "";

	_float4 m_fColor = { 0.f,0.f,0.f,0.f };
	_float2 m_fGlow = { 0.f,0.f };
	_uint m_iShaderPass = 0;

private:
	_double m_dDurationTime = 0.;
};
END_NAMESPACE