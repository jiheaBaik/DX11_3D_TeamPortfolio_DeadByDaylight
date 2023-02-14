#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CEdit abstract : public CBase{
protected:
	explicit CEdit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
	virtual ~CEdit(void) override = default;
	virtual void Delete(void) override;
public:
	virtual void Update(void) PURE;
	virtual const HRESULT Render(void) { return S_OK; }
public:
	const _bool& Get_Open(void) const;
	void Set_Open(const _bool & bOpen);
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	bool m_bOpen = true;
	bool m_bFocus = false;
	ImGuiWindowFlags m_iWindow_Flag = ImGuiWindowFlags_None;
};
END_NAMESPACE