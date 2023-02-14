#pragma once
#include "FS.h"
#include "Inter_ChestBox.h"


BEGIN_NAMESPACE(Client)
class CFS_Camper_HealCamper final : public CFS {
public:
	explicit CFS_Camper_HealCamper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_HealCamper(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	class CCamper_GamePlay* m_pCamper_Target = nullptr;
	class CTransform* m_pCamper_TargetTransform = nullptr;
private:
	_double m_fHealGauge = 0.f;
	_double m_dHealTime = 0.;
	_double	m_fTargetGauge = 0.f;
};
END_NAMESPACE