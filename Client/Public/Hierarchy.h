#pragma once
#include "Edit.h"

BEGIN_NAMESPACE(Client)
class CHierarchy final : public CEdit {
public:
	static CHierarchy* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
private:
	explicit CHierarchy(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CHierarchy(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(void) override;
};
END_NAMESPACE