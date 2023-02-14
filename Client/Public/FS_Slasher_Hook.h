#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Hook final : public CFS {
public:
	explicit CFS_Slasher_Hook(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Hook(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
//hwang
private:
	void Check_Perk(_double dTimeDelta);
	
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CInter_Hook* m_pHook = nullptr;
	class CTransform* m_pHookTransform = nullptr;
	class CTransform* m_pHookTransform_Slasher = nullptr;
private:
	_float4 m_vPos = {};
	_float4 m_vCenterPos = {};
};
END_NAMESPACE