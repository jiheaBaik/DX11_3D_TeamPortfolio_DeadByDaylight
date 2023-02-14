#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Run_Carry final : public CFS {
public:
	enum ERun { RUN_FT, RUN_BK, RUN_RT, RUN_LT, RUN_FTRT, RUN_FTLT, RUN_BKRT, RUN_BKLT, END };
public:
	explicit CFS_Slasher_Run_Carry(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Run_Carry(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
private:
	ERun m_eCurRun = ERun::END;
	ERun m_ePreRun = ERun::END;
};
END_NAMESPACE