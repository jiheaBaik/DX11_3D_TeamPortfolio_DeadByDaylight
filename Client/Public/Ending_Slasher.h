#pragma once
BEGIN_NAMESPACE(Client)
class CEnding_Slasher final : public CObject {
public:
	static CEnding_Slasher* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CEnding_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEnding_Slasher(const CEnding_Slasher& rhs);
	virtual ~CEnding_Slasher(void) override = default;
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
	class CShader* m_pShader_Weapon = nullptr;
	class CModel* m_pModel = nullptr;

	class CTransform* m_pTransform_Weapon = nullptr;
	class CModel* m_pModel_Weapon = nullptr;
private:
	_float m_fRoughness = 0.275f;
	_float m_fRefract = 0.275f;
	_float m_fMetalness = 0.f;
};
END_NAMESPACE