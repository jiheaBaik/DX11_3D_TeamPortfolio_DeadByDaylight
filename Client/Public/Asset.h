#pragma once
#include "Edit.h"

BEGIN_NAMESPACE(Client)
class CAsset final : public CEdit {
public:
	static CAsset* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
private:
	explicit CAsset(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CAsset(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(void) override;
private:
	map<wstring, class CObject*>* m_pMapPrototype = nullptr;
	_tchar m_szPrototypeKey[MAX_PATH] = TEXT("");
};
END_NAMESPACE
