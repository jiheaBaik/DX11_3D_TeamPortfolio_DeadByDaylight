#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CComponent abstract : public CBase {
public:
	virtual CComponent* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CComponent(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent(void) override = default;
	virtual void Delete(void) override;
public:
	virtual void Update_Edit(const _int & iPushID) {};
public:
	virtual void Save_Data(Json::Value& root) {};
	virtual void Load_Data(Json::Value& root) {};
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_bool m_bClone = false;
};
END_NAMESPACE