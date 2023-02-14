#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_HatchOut final : public CFS {
public:
	explicit CFS_Camper_HatchOut(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_HatchOut(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
private:
	class CInter_Hatch* m_pHatch = nullptr;
	class CTransform* m_pHatchTransform = nullptr;
private:
	_bool m_bCheck = false;
private:
	_double m_dEndingTime = 0.;
	_bool m_bUI_FadeIn = false;
	_bool m_bUI_FadeOut = false;
	_bool m_bUI_First = false;
	_bool m_bUI_Second = false;
	_bool m_bAnimEnd = false;


};
END_NAMESPACE