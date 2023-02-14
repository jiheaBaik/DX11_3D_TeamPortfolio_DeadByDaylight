#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CCamper_Lobby_Sit final : public CObject {
public:
	enum class ECamper : _ubyte { CHERYL, BILL, DWIGHT, MEG, END };
	enum class ESTATE { SIT, IDEL, IDEL2, IDEL3, IDEL4, SELECT, END };
public:
	static CCamper_Lobby_Sit* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCamper_Lobby_Sit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCamper_Lobby_Sit(const CCamper_Lobby_Sit& rhs);
	virtual ~CCamper_Lobby_Sit(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	void Set_State(ESTATE eState) { m_eState = eState; }
	void Set_Dissolve(const _float& m_fGoal);
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CTransform* m_pOrbitTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel_Cheryl = nullptr;
	class CModel* m_pModel_Bill = nullptr;
	class CModel* m_pModel_Dwight = nullptr;
	class CModel* m_pModel_Meg = nullptr;
	class CCollider* m_pCollider = nullptr;
private:
	//_uint m_iAnimEnd = 0;
	_uint m_iAnimIndex = 2;
	ESTATE m_eState = ESTATE::SIT;
	ECamper m_eCamper = ECamper::CHERYL;

	_float m_fDissolveLength = 0.f;
	_float3 m_fDissolveColor = { 8.f, 0.f,0.f };
	_float3 m_fDissolveColor2 = { 8.f, 1.5f,0.f };
	_float m_fGoalDissolveLength = -0.1f;
	_float2 m_fGlow = { 0.5f, 1.f };
private:
	_float m_fRoughness = 0.275f;
	_float m_fRefract = 0.275f;
	_float m_fMetalness = 0.f;
	_float m_fMetalness2 = 0.f;
};
END_NAMESPACE