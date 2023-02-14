#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_Generator final : public CFS {
public:
	explicit CFS_Camper_Generator(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_Generator(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	class CInter_Generator* m_pGenerator = nullptr;
private:
	CTransform* pGeneratorTransform2 = nullptr;
	class CAudio* m_pAudio = nullptr;
	_uint m_iDir = 0;
	_double m_dGauge = 0.;
	_double m_dEffectTime = 0.;
	_bool m_bFailed = false;
};
END_NAMESPACE