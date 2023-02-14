#pragma once
BEGIN_NAMESPACE(Client)
class CEnding_Grass final : public CObject {
public:
	static CEnding_Grass* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CEnding_Grass(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEnding_Grass(const CEnding_Grass& rhs);
	virtual ~CEnding_Grass(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_Active(const _bool& bActive);
	const _bool& Get_Active(void);
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
private:
	_float m_fRoughness = 0.275f;
	_float m_fRefract = 0.275f;
	_float m_fMetalness = 0.f;
private:
	_bool m_bActive = true;
	_float m_fSpeed = 3.f;
	_float m_fGoal = 240.f;
};
END_NAMESPACE