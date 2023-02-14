#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Camper_ExitDoor_In final : public CFS {
public:
	explicit CFS_Camper_ExitDoor_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFS_Camper_ExitDoor_In(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* ModelName) override;
	virtual const _ubyte Update(const _double& dTimeDelta) override;
private:
	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	class CInter_ExitDoor* m_pExitDoor = nullptr;
	class CTransform* m_pExitDoorTransform = nullptr;
	class CModel* m_pExitDoorModel = nullptr;
};
END_NAMESPACE