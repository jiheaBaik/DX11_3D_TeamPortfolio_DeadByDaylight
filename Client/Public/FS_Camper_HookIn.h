#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_HookIn final : public CFS {
public:
	explicit CFS_Camper_HookIn(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_HookIn(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;

	class CInter_Hook* m_pHook = nullptr;
	class CTransform* m_pHookTransform = nullptr;
	class CModel* m_pHookModel = nullptr;

	class CSlasher_Huntress* m_pHuntress = nullptr;
private:
	_float4x4 m_fRootOffsetMatrix;
	_float3 m_fRootPrePos = { 0.f,0.f,0.f };
	_bool m_flag = false;
private:
	class CAudio* m_pAudio = nullptr;
};
END_NAMESPACE