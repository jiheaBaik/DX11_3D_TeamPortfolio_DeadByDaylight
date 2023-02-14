#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_Walk final : public CFS {
public:
	explicit CFS_Camper_Walk(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_Walk(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	//void KeyInput(const _double& dTimeDelta);
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
private:
	_float m_fSpeed = 1.7f;
	_bool m_bisSit = false;
	_bool m_bisCraw = false;
private:
	_double m_dBloodStainTime = 0.;
	_float3 m_fPrePos = { 0.f,0.f,0.f };
};
END_NAMESPACE
