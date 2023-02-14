#pragma once
BEGIN_NAMESPACE(Client)
class CEnding_Spon final : public CObject {
public:
	static CEnding_Spon* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CEnding_Spon(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CEnding_Spon(const CEnding_Spon& rhs);
	virtual ~CEnding_Spon(void) override = default;
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
private:
	list<class CEnding_Ground*> m_listGround;
	list<class CEnding_Grass*> m_listGrass;
	class CEnding_Ground* m_pGround = nullptr;
private:
	_float m_fGround_Dist = 31.8f;
	_float m_fGrass_Time = 0.f;
	_float m_fGrass_TimeMax = 3.f;
};
END_NAMESPACE