#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CInter_PlankAnim final : public CObject {
public:
	enum class ESTATE : _byte { IDLE, FALLDOWN, DOWNLOOP, PULLUP, BREAK, BROKEN, END };
public:
	static CInter_PlankAnim* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CInter_PlankAnim(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CInter_PlankAnim(const CInter_PlankAnim& rhs);
	virtual ~CInter_PlankAnim(void) override = default;
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
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
	class CAudio* m_pAudio = nullptr;

	class CTransform* m_pTransform_Slasher = nullptr;
	class CCollider* m_pCollider_Slasher = nullptr;

	class CTransform* m_pTransform_F = nullptr;
	class CCollider* m_pCollider_F = nullptr;
	class CTransform* m_pTransform_B = nullptr;
	class CCollider* m_pCollider_B = nullptr;

	//camper
	class CTransform* m_pTransform_CamperL = nullptr;
	class CTransform* m_pTransform_CamperR = nullptr;
	class CCollider* m_pCollider_CamperL = nullptr;
	class CCollider* m_pCollider_CamperR = nullptr;
private:
	_uint		m_iAnimFinish = 0;
	ESTATE		m_eState = ESTATE::END;
	ESTATE		m_ePreState = m_eState;
	_bool		m_bLoop = false;
	_bool		m_bCamper = false;
private:
	char m_szModel[MAX_PATH] = "";
private:
	class CCamper_GamePlay* m_pCamper = nullptr;
	class CSlasher_Huntress* m_pSlasher = nullptr;
private:
	_bool m_bSound_FallDown = false;
	_bool m_bSound_FallDown2 = false;

	_bool m_bSound_Break = false;
	_bool m_bSound_Break2 = false;
private:
	_bool m_bDestroyDissolve = false;
	_float m_fDissolveLength = 3.f;
	_float m_fGoalDissolveLength = 0.f;
};
END_NAMESPACE