#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CScene abstract : public CBase{
protected:
	explicit CScene(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual ~CScene(void) override = default;
	virtual void Delete(void) override;
public:
	virtual void Update(const _double & dTimeDelta) PURE;
	virtual void Late_Update(const _double & dTimeDelta) PURE;

	virtual void Update_Network(const string& pKey) {};
	virtual void Update_Network(void) {};
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	class CNetwork* m_pNetwork = nullptr;
};
END_NAMESPACE