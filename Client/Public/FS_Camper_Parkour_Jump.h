#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_Parkour_Jump final : public CFS {
public:
	explicit CFS_Camper_Parkour_Jump(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_Cheryl* const& pCamper_Cheryl);
	virtual ~CFS_Camper_Parkour_Jump(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_Cheryl* m_pCamper_Cheryl = nullptr;
	class CInter_Parkour* m_pParkour = nullptr;
	_uint m_iDir = 0;
	CTransform* pParkourTransform2 = nullptr;

	_float4x4	m_fRootOffsetMatrix;
	_float3		m_fRootPrePos = { 0.f,0.f,0.f };
	_float m_fDist = 0.f;
};
END_NAMESPACE