#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CInter_ChestBox final : public CObject {
public:
	enum class ESTATE : _byte { IDLE, OPEN_STEP1, OPEN_STEP2, OPEN_STEP3, OPEN_END, END };
	enum class EItemKind : _int { 
		MEDIKIT, FLASHLIGHT, END 
	};
public:
	static CInter_ChestBox* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CInter_ChestBox(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CInter_ChestBox(const CInter_ChestBox& rhs);
	virtual ~CInter_ChestBox(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	const EItemKind& Get_ItemKind(void) const {
		return m_eItemKind;
	}
	const ESTATE& Get_State(void) const {
		return m_eState;
	}
	void Set_State(ESTATE eState) {
		m_eState = eState;
		Update_Network("Set_State");
	}
	void Plus_PlayerNum(void){
		++m_iPlayerNum;
	}
public:
	_bool Get_IsUse() { return m_bIsUse; }
	void Set_IsUse(_bool isUse) { 
		m_bIsUse = isUse; 
		Update_Network("Set_ItemUse");
	}

public:
	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;
public:
	virtual const HRESULT Save_Data(Json::Value& root) override;
	virtual void Load_Data(Json::Value& root) override;
public:
	const _float& Get_Gauge(void);
	const void Add_Gauge(const _float& fGauge);
private:
	class CTransform* m_pTransform = nullptr;

	class CRenderer* m_pRenderer = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
	class CAudio* m_pAudio = nullptr;
	//item
	class CTransform* m_pItemTransform = nullptr;
	class CModel* m_pMediKit_Model = nullptr;
	class CModel* m_pFlashLight_Model = nullptr;
	class CModel* m_pItem_Model = nullptr;
	class CModel* m_pItem_Model2 = nullptr;

	class CShader* m_pShader2 = nullptr;

	class CTransform* m_pColTransform = nullptr;
	class CCollider* m_pCollider2 = nullptr;

private:
	_uint		m_iAnimFinish = 0;
	_uint		m_iPlayerNum = 0;
	_double		m_dTime = 0.0;
	ESTATE		m_eState = ESTATE::END;
	ESTATE		m_ePreState = m_eState;
	EItemKind m_eItemKind = EItemKind::MEDIKIT;

private:
	char m_szModel[MAX_PATH] = "";
private:
	_bool m_bIsUse = false;
	_float m_fAddGauge = 0.f;
	_float m_fGauge = 0.f;
private:
	class CCamper_GamePlay* m_pCamper = nullptr;
private:
	_float m_fSound_OpenLoop = 1.f;
};
END_NAMESPACE