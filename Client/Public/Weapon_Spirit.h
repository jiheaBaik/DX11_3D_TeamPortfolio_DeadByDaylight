#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CWeapon_Spirit final : public CObject {
public:
	static CWeapon_Spirit* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CWeapon_Spirit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CWeapon_Spirit(const CWeapon_Spirit& rhs);
	virtual ~CWeapon_Spirit(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	const _bool& Get_Attack(void);
	void Set_Attack(const _bool& bAttack);

	void Render_On(void);
	void Render_Off(void);
public:
	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CTransform* m_pTransform_FPV = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
	class CModel* m_pModel_FPV = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CCollider* m_pCollider_FPV = nullptr;

	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CTransform* m_pSlasher_Transform = nullptr;
	class CSlasher_Spirit_FPV_Trail* m_pSlasher_FPV_Trail = nullptr;
	class CSlasher_Spirit_Trail* m_pSlasher_Trail = nullptr;
	class CAudio* m_pSlasher_Audio = nullptr;

	_int m_iColID = -1;
	_bool m_bAttack = false;
	_bool m_bRender = false;
private:
	_uint m_iCamperHitDir;
private:
	_float m_fDissolveLength = 0.f;
	_bool m_bDissolveUpDown = false;
};
END_NAMESPACE