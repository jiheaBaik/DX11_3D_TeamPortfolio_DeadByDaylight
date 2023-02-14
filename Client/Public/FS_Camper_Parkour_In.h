#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_Parkour_In final : public CFS {
public:
	explicit CFS_Camper_Parkour_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_Cheryl* const& pCamper_Cheryl);
	virtual ~CFS_Camper_Parkour_In(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_Cheryl* m_pCamper_Cheryl = nullptr;
	class CInter_Parkour* m_pParkour = nullptr;
	_uint m_iDir = 0;
	CTransform* pParkourTransform2 = nullptr;
};
END_NAMESPACE