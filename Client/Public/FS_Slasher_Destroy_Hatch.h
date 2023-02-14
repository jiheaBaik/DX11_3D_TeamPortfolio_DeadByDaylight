#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Destroy_Hatch final : public CFS {
public:
	explicit CFS_Slasher_Destroy_Hatch(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Destroy_Hatch(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CInter_Hatch* m_pHatch = nullptr;
	class CTransform* m_pHatchTransform = nullptr;
	class CTransform* m_pHatchTransform_Slasher = nullptr;
private:
	_float4 m_vPos = {};
	_float4 m_vCenterPos = {};
};
END_NAMESPACE