#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Client)
class CFS : public CBase {
public:
	explicit CFS(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual ~CFS(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const HRESULT Init(_char* pModelName) PURE;
	virtual const HRESULT Init(_uint iAnimIndex);
	virtual const HRESULT Init(_char* pModelName, _double dLerpTime);

	virtual const _ubyte Update(const _double& dTimeDelta) PURE;
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	class CNetwork* m_pNetwork = nullptr;
};
END_NAMESPACE