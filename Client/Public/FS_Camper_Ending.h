#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_Ending final : public CFS {
public:
	explicit CFS_Camper_Ending(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_Ending(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	_double m_EndingTime = 0.f;
	_bool m_CreatUI = false;
private:
	_bool m_bSoundInit = false;

	_bool m_bUI_First = false;
	_bool m_bUI_FadeIn = false;
	_bool m_bUI_FadeOut = false;
	_bool m_bUI_Second = false;
};
END_NAMESPACE