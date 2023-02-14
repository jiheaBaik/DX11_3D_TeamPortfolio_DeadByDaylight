#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_CrawIdle final : public CFS {
public:
	explicit CFS_Camper_CrawIdle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_CrawIdle(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
private:
	_float m_fDist = 3.f;
	_float3 m_fPrePos = { 0.f,0.f,0.f };
};
END_NAMESPACE