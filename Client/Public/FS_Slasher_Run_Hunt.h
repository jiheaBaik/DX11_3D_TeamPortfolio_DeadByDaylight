#pragma once
#include "FS.h"

BEGIN_NAMESPACE(Client)
class CFS_Slasher_Run_Hunt final : public CFS {
public:
	enum ERun { RUN_FT, RUN_BK, RUN_RT, RUN_LT, RUN_FTRT, RUN_FTLT, RUN_BKRT, RUN_BKLT, END };
public:
	explicit CFS_Slasher_Run_Hunt(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFS_Slasher_Run_Hunt(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) override;
	virtual const HRESULT Init(_uint iAnimIndex) override;
	virtual const _ubyte  Update(const _double& dTimeDelta) override;
public:
	const _float3& Get_Dir(void);
private:
	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
private:
	ERun m_eCurRun = ERun::END;
	ERun m_ePreRun = ERun::END;
private:
	_float3 m_vDir = _float3{ 0.f,0.f,0.f };
};
END_NAMESPACE