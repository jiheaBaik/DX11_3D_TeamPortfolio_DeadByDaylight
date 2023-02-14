#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CInter_Hook final : public CObject {
	
public:
	enum class ESTATE : _byte { IDLE, IDLE2, HANGING, HANGING_2, HANGING_END, SPREAD, SHAKE, ENCASE, STAB, STAB_2,  STRUGGLE, STRUGGLE_2,
		KILL, RAISE, TRY_ESCAPE, ESCAPE, END };
public:
	static CInter_Hook* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CInter_Hook(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CInter_Hook(const CInter_Hook& rhs);
	virtual ~CInter_Hook(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	const ESTATE& Get_State(void) const {
		return m_eState;
	}
	void Set_State(ESTATE eState) {
		m_eState = eState;
		Update_Network("Set_State");
	}
public:
	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
public:
	_bool Get_SlasherPerk() { return m_bSlasherPerk; }
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CShader* m_pShader = nullptr;
	class CShader* m_pShader2 = nullptr;
	class CModel* m_pModel_Hook = nullptr;
	class CModel* m_pModel_HookSpider = nullptr;
private:
	_uint		m_iAnimFinish = 0;
	_double		m_dTime = 0.0;
	ESTATE		m_eState = ESTATE::IDLE;
	ESTATE		m_ePreState = m_eState;


	_bool		m_bLoop = false;
	_bool m_bSpiderRender = false;

	// effect _yeon
	_bool m_bCreate = false;
	_bool m_bDelete = false;
	_bool m_bSlasherPerk = false;
	_float m_fDissolveLength = 0.f;
	_float m_fGoalDissolveLength = 0.f;
private:
	char m_szModel[MAX_PATH] = "";
};
END_NAMESPACE