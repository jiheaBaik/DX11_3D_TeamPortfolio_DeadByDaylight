#pragma once
#include "Edit.h"

BEGIN_NAMESPACE(Client)
class CDebug final : public CEdit {
public:
	static CDebug* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
private:
	explicit CDebug(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CDebug(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	virtual void Update(void) override;
private:
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
};
END_NAMESPACE