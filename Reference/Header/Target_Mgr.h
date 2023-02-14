#pragma once
#include "Base.h"
#include "Target.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CTarget_Mgr final : public CBase {
	DECLARE_SINGLETON(CTarget_Mgr)
private:
	explicit CTarget_Mgr(void);
	virtual ~CTarget_Mgr(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext);
public:
	class CTarget* const Get_Target(const _tchar* const& pTargetKey);
	map<wstring, class CTarget*>& Get_MapTarget(void);
public:
	const HRESULT Add_Target(const _tchar* const& pTargetKey, class CTarget* const& pTarget);
	const HRESULT Add_DepthStencilView(const _tchar* const& pDepthStencilViewKey, const _uint& iWidth, const _uint& iHeight);
	const HRESULT Add_Viewport(const _tchar* const& pViewportKey, const _uint& iWidth, const _uint& iHeight);
public:
	const HRESULT Add_MultiTarget(const _tchar* const& pMultiKey, const _tchar* const& pTargetKey);
	const HRESULT Begin_MultiTarget(const _tchar* const& pMultiKey, const _tchar* const& pDepthStencilViewKey, const _tchar* const& pViewportKey, const _bool& bClear_RenderTargetView = true);
	const HRESULT End_MultiTarget(void);

	const HRESULT Clear_DepthStencilView(const _tchar * pDepthStencilViewKey);

private:
	class CTarget* const Find_Target(const _tchar* const& pTargetKey);
	list<class CTarget*>* Find_MultiTarget(const _tchar* const& pMultiKey);

	ID3D11DepthStencilView* const Find_DepthStencilView(const _tchar* const& pDepthStencilViewKey);
	D3D11_VIEWPORT* const Find_Viewport(const _tchar* const& pTargetKey);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	ID3D11RenderTargetView* m_pBackBufferView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	D3D11_VIEWPORT m_Viewport;

	map<wstring, class CTarget*> m_mapTarget;
	map<const _tchar* const, list<class CTarget*>> m_mapMultiTarget;
	map<const _tchar* const, ID3D11DepthStencilView*> m_mapDepthStencilView;
	map<const _tchar* const, D3D11_VIEWPORT> m_mapViewport;
};
END_NAMESPACE
