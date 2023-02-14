#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CEdit_Mgr  final : public CBase {
	DECLARE_SINGLETON(CEdit_Mgr)
public:
	enum class EMode : _ubyte { PLAY, EDIT, DEBUG, END };
private:
	explicit CEdit_Mgr(void);
	virtual ~CEdit_Mgr(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const HWND & hWnd, ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EMode& eMode);
	void Update(void);
	const HRESULT Render(void);
public:
	const EMode& Get_Mode(void) const;
	void Set_Mode(const EMode& eMode);
	const HRESULT Add_Edit(const _tchar* const& pEditKey, class CEdit* const& pEdit);
	class CEdit* const Get_Edit(const _tchar* const& pEditKey);
private:
	class CEdit* const Find_Edit(const _tchar* pEditKey) const;
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	EMode m_eMode = EMode::END;
	vector<class CEdit*> m_vecEdit;
	map<const _tchar*, class CEdit*> m_mapEdit;
};
END_NAMESPACE