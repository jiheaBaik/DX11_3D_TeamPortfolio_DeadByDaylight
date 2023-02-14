#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Attack_Slash_Swing final : public CFS {
public:
	explicit CFS_Slasher_Attack_Slash_Swing(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Attack_Slash_Swing(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
public:
	const _float3& Get_Dir(void);
	void Set_Dir(const _float3& vDir);

	const _bool& Get_Attack(void);
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	_float3 m_vDir = _float3{ 0.f,0.f,0.f };
	_float3 m_vPreDir = _float3{ 0.f,0.f,0.f };
	
	_bool m_bAttack = false;
};
END_NAMESPACE