#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_Mori final : public CFS {
public:
	explicit CFS_Camper_Mori(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_Mori(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	class CSlasher_Huntress* m_pSlasher = nullptr;
	_double m_fTime = 0.f;
	//class CTransform* m_pSlasherTransform = nullptr;
	
	_float4x4	m_fRootOffsetMatrix;
	_float3		m_fRootPrePos = { 0.f,0.f,0.f };
	_bool m_flag = false;
private:
	_bool m_bCheck = false;
	_bool m_bMoriEnd = false;
private:
	_double m_dEndingTime = 0.;
	_bool m_bUI_FadeIn = false;
	_bool m_bUI_FadeOut = false;
	_bool m_bUI_First = false;
	_bool m_bUI_Second = false;
};
END_NAMESPACE