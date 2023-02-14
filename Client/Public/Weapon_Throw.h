#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CWeapon_Throw final : public CObject {
public:
	static CWeapon_Throw* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CWeapon_Throw(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CWeapon_Throw(const CWeapon_Throw& rhs);
	virtual ~CWeapon_Throw(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	const _bool& Get_Throw(void);
	void Set_Throw(const _bool& bThrow);

	const _float3& Get_Dir(void);
	void Set_Dir(const _float3& vDir);

	const _float3& Get_RotDir(void);
	void Set_RotDir(const _float3& vRotDir);
public:
	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CAudio* m_pAudio = nullptr;

	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CTransform* m_pSlasher_Transform = nullptr;
	class CThrow_Trail* m_pThrow_Trail = nullptr;

	class CNavi* m_pNavi = nullptr;

	_int m_iColID = -1;
	_bool m_bThrow = false;
	_bool m_bLook = false;

	_double m_dTimeDelta = 0.;
	_float3 m_vThrowDir;
	_float3 m_vRotDir;
private:
	_float3 m_vAccel{ 0.f, 0.f, 0.f };
private:
	_uint m_iCamperHitDir;
private:
	_float m_fRoughness = 0.1f;
	_float m_fRefract = 0.222f;
	_float m_fMetalness = 0.613f;
	_float m_fMetalness2 = 0.222f;
};
END_NAMESPACE