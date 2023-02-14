#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_HookBeeingRescuedIn final : public CFS {
public:
	explicit CFS_Camper_HookBeeingRescuedIn(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_HookBeeingRescuedIn(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	class CCamper_GamePlay* m_pCamper_Target = nullptr;

};
END_NAMESPACE