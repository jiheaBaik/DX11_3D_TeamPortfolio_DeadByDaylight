#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Locker_Reload final : public CFS {
public:
	explicit CFS_Slasher_Locker_Reload(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Locker_Reload(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CInter_Locker* m_pLocker = nullptr;
	class CTransform* m_pLockerTransform = nullptr;
	class CTransform* m_pLockerTransform_Slasher = nullptr;
private:
	_float4x4	m_fRootOffsetMatrix;
	_float3		m_fRootPrePos = { 0.f,0.f,0.f };
};
END_NAMESPACE