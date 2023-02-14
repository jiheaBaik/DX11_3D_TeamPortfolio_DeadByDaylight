#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_Closet_UnHide final : public CFS {
public:
	explicit CFS_Camper_Closet_UnHide(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_Closet_UnHide(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	_float4x4	m_fRootOffsetMatrix;
	_float3		m_fRootPrePos = { 0.f,0.f,0.f };
	class CInter_Locker* m_pLocker = nullptr;
	class CTransform* m_pLockerTarnsform = nullptr;
	class CTransform* m_pLockerTransform2 = nullptr;

};
END_NAMESPACE