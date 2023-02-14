#pragma once
#include "Layer.h"
#include "Camera.h"
#include "Light.h"
#include "Enviro.h"

BEGIN_NAMESPACE(Engine)
class CObject_Mgr final : public CBase {
	DECLARE_SINGLETON(CObject_Mgr)
private:
	explicit CObject_Mgr(void);
	virtual ~CObject_Mgr(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const _ubyte& byScene);
	void Update_Layer(const _double& dTimeDelta);
	void Late_Update_Layer(const _double& dTimeDelta);
public:
	const HRESULT Add_Prototype(const _tchar* const& pPrototypeKey, class CObject* const& pObject);
	map<wstring, class CObject*>& Get_MapPrototype(void);

	const HRESULT Create_Clone(const _tchar* const& pPrototypeKey, const _ubyte& byScene, const _tchar* const& pLayerKey, void* const& pArg = nullptr);
	class CObject* const CreateGet_Clone(const _tchar* const& pPrototypeKey, const _ubyte& byScene, const _tchar* const& pLayerKey, void* const& pArg = nullptr);
	map<wstring, class CLayer*>* const Get_LayerClone(const _ubyte& byScene) const;
	list<class CObject*>* const Get_ListClone(const _ubyte& byScene, const _tchar* const& pLayerKey) const;
	class CObject* const Get_Clone(const _ubyte& byScene, const _tchar* const& pLayerKey, const _uint& iIndex = 0) const;
	class CObject* const Get_CloneID(const _ubyte& byScene, const _tchar* const& pLayerKey, const _uint& iID = 0) const;
	class CObject* const Get_CloneControl(const _ubyte& byScene, const _tchar* const& pLayerKey) const;
	const HRESULT Clear_Clone(const _ubyte& byScene);
	const HRESULT Delete_Clone(class CObject* const& pObject);

private:
	class CObject* const Find_Prototype(const _tchar* const& pPrototypeKey) const;
	class CLayer* const Find_Layer(const _ubyte& byScene, const _tchar* const& pLayerKey) const;
private:
	_ubyte m_byScene = 0;
	map<wstring, class CObject*> m_mapPrototype;
	map<wstring, class CLayer*>* m_pMapLayer = nullptr;
};
END_NAMESPACE