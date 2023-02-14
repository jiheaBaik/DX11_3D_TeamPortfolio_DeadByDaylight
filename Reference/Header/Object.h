#pragma once
#include "Base.h"
#include "Renderer.h"

BEGIN_NAMESPACE(Engine)
class DECLSPEC_DLL CObject abstract : public CBase{
public:
	virtual CObject* const Clone(void* const& pArg = nullptr) PURE;
protected:
	explicit CObject(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CObject(const CObject& rhs);
	virtual ~CObject(void) override = default;
	virtual void Delete(void) override;
public:
	virtual const _ubyte Update(const _double & dTimeDelta) PURE;
	virtual void Update_Edit(void) {};
	virtual void Late_Update(const _double & dTimeDelta) PURE;
	virtual const HRESULT Render(const CRenderer::EOrder & eOrder) PURE;
public:
	class CComponent* const Get_Component(const _tchar* const& pComponentKey) const;
	virtual const HRESULT Save_Data(Json::Value & root) { return E_FAIL; };
	virtual void Load_Data(Json::Value & root) {};

	const _bool& Get_Control(void);
	void Set_Control(const _bool& bControl);
	const _int& Get_Id(void);
	void Set_Id(const _int & iId);
	virtual void Update_Network(const string& pKey) {};
	virtual void Update_Network(void) {};
protected:
	const HRESULT Add_Component(const _ubyte & byScene, const _tchar* const& pPrototypeKey, const _tchar* const& pComponentKey, class CComponent*& ppOut, void* const& pArg = nullptr);
private:
	class CComponent* const Find_Component(const _tchar* const& pComponentKey) const;
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	class CNetwork* m_pNetwork = nullptr;
protected:
	map<wstring, class CComponent*> m_mapComponent;

	_bool m_bControl = false;
	_int m_iId = -1;
};
END_NAMESPACE