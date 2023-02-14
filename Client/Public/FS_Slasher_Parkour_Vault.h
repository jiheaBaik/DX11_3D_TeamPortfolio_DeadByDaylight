#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Pakour_Vault final : public CFS {
public:
	explicit CFS_Slasher_Pakour_Vault(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Pakour_Vault(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CInter_Parkour_Vault* m_pParkour_Vault = nullptr;
	class CTransform* m_pParkourTransform = nullptr;
	class CTransform* m_pParkourTransform2 = nullptr;
private:
	_uint m_iParkourDir = 0;

	_float4x4	m_fRootOffsetMatrix;
	_float3		m_fRootPrePos = { 0.f,0.f,0.f };
};
END_NAMESPACE