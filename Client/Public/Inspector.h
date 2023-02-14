#pragma once
#include "Edit.h"

BEGIN_NAMESPACE(Client)
class CInspector final : public CEdit {
public:
	static CInspector* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
private:
	explicit CInspector(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CInspector(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(void) override;
public:
	class CObject* const Get_Object(void);
	void Set_Object(class CObject* const& pObject);
private:
	class CObject* m_pObject = nullptr;
};
END_NAMESPACE