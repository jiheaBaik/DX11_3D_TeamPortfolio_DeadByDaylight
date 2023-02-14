#include "Object_Mgr.h"
#include "Object.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Mgr)

CObject_Mgr::CObject_Mgr(void)
	: CBase() {
}

void CObject_Mgr::Delete(void) {
	for (_ubyte i = 0; i < m_byScene; ++i) {
		for (auto& pair : m_pMapLayer[i])
			Safe_Release(pair.second);
		m_pMapLayer[i].clear();
	}
	Safe_Delete_Array(m_pMapLayer);

	for (auto& pair : m_mapPrototype)
		Safe_Release(pair.second);
	m_mapPrototype.clear();
}

const HRESULT CObject_Mgr::Init(const _ubyte& byScene) {
	if (nullptr != m_pMapLayer)
		return E_FAIL;
	m_pMapLayer = new map<wstring, class CLayer*>[byScene];
	if (nullptr == m_pMapLayer)
		return E_FAIL;
	m_byScene = byScene;
	return S_OK;
}

void CObject_Mgr::Update_Layer(const _double& dTimeDelta) {
	if (nullptr == m_pMapLayer)
		return;
	for (_ubyte i = 0; i < m_byScene; ++i)
		for (auto& pair : m_pMapLayer[i])
			if (nullptr != pair.second)
				if (OBJ_SCENE == pair.second->Update_Object(dTimeDelta))
					return;
}

void CObject_Mgr::Late_Update_Layer(const _double& dTimeDelta) {
	if (nullptr == m_pMapLayer)
		return;
	for (_ubyte i = 0; i < m_byScene; ++i)
		for (auto& pair : m_pMapLayer[i])
			if (nullptr != pair.second)
				pair.second->Late_Update_Object(dTimeDelta);
}

const HRESULT CObject_Mgr::Add_Prototype(const _tchar* const& pPrototypeKey, CObject* const& pObject) {
	if (nullptr == pObject)
		return E_FAIL;
	if (nullptr != Find_Prototype(pPrototypeKey)) {
		pObject->Release();
		return S_OK;
	}
	m_mapPrototype.emplace(pPrototypeKey, pObject);
	return S_OK;
}

map<wstring, class CObject*>& CObject_Mgr::Get_MapPrototype(void) {
	return m_mapPrototype;
}


const HRESULT CObject_Mgr::Create_Clone(const _tchar* const& pPrototypeKey, const _ubyte& byScene, const _tchar* const& pLayerKey, void* const& pArg) {
	if (byScene >= m_byScene || nullptr == m_pMapLayer)
		return E_FAIL;
	CObject* pPrototype = Find_Prototype(pPrototypeKey);
	if (nullptr == pPrototype)
		return E_FAIL;
	CObject* pClone = pPrototype->Clone(pArg);
	if (nullptr == pClone)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(byScene, pLayerKey);
	if (nullptr == pLayer) {
		pLayer = CLayer::Create();
		if (nullptr == pLayer) {
			Safe_Release(pClone);
			return E_FAIL;
		}
		if (FAILED(pLayer->Add_Object(pClone))) {
			Safe_Release(pClone);
			Safe_Release(pLayer);
			return E_FAIL;
		}
		m_pMapLayer[byScene].emplace(pLayerKey, pLayer);
	}
	else
		if (FAILED(pLayer->Add_Object(pClone))) {
			Safe_Release(pClone);
			return E_FAIL;
		}
	return S_OK;
}

CObject* const CObject_Mgr::CreateGet_Clone(const _tchar* const& pPrototypeKey, const _ubyte& byScene, const _tchar* const& pLayerKey, void* const& pArg) {
	if (byScene >= m_byScene || nullptr == m_pMapLayer)
		return nullptr;
	CObject* pPrototype = Find_Prototype(pPrototypeKey);
	if (nullptr == pPrototype)
		return nullptr;
	CObject* pClone = pPrototype->Clone(pArg);
	if (nullptr == pClone)
		return nullptr;

	CLayer* pLayer = Find_Layer(byScene, pLayerKey);
	if (nullptr == pLayer) {
		pLayer = CLayer::Create();
		if (nullptr == pLayer) {
			Safe_Release(pClone);
			return nullptr;
		}
		if (FAILED(pLayer->Add_Object(pClone))) {
			Safe_Release(pClone);
			Safe_Release(pLayer);
			return nullptr;
		}
		m_pMapLayer[byScene].emplace(pLayerKey, pLayer);
	}
	else
		if (FAILED(pLayer->Add_Object(pClone))) {
			Safe_Release(pClone);
			return nullptr;
		}
	return pClone;
}

map<wstring, class CLayer*>* const CObject_Mgr::Get_LayerClone(const _ubyte& byScene) const {
	if (byScene >= m_byScene)
		return nullptr;
	return &(m_pMapLayer[byScene]);
}

list<class CObject*>* const CObject_Mgr::Get_ListClone(const _ubyte& byScene, const _tchar* const& pLayerKey) const {
	if (byScene >= m_byScene)
		return nullptr;
	CLayer* pLayer = Find_Layer(byScene, pLayerKey);
	if (nullptr == pLayer)
		return nullptr;
	return &pLayer->Get_listObject();
}

CObject* const CObject_Mgr::Get_Clone(const _ubyte& byScene, const _tchar* const& pLayerKey, const _uint& iIndex) const {
	if (byScene >= m_byScene)
		return nullptr;
	CLayer* pLayer = Find_Layer(byScene, pLayerKey);
	if (nullptr == pLayer)
		return nullptr;
	return pLayer->Get_Object(iIndex);
}

CObject* const CObject_Mgr::Get_CloneID(const _ubyte& byScene, const _tchar* const& pLayerKey, const _uint& iID) const {
	if (byScene >= m_byScene)
		return nullptr;
	CLayer* pLayer = Find_Layer(byScene, pLayerKey);
	if (nullptr == pLayer)
		return nullptr;
	return pLayer->Get_ObjectID(iID);
}

CObject* const CObject_Mgr::Get_CloneControl(const _ubyte& byScene, const _tchar* const& pLayerKey) const {
	if (byScene >= m_byScene)
		return nullptr;
	CLayer* pLayer = Find_Layer(byScene, pLayerKey);
	if (nullptr == pLayer)
		return nullptr;
	return pLayer->Get_ObjectControl();
};

const HRESULT CObject_Mgr::Clear_Clone(const _ubyte& byScene) {
	if (byScene >= m_byScene)
		return E_FAIL;
	for (auto& pair : m_pMapLayer[byScene])
		Safe_Release(pair.second);
	m_pMapLayer[byScene].clear();
	return S_OK;
}

const HRESULT CObject_Mgr::Delete_Clone(CObject* const& pObject) {
	for (_ubyte i = 0; i < m_byScene; ++i)
		for (auto& pair : m_pMapLayer[i])
			pair.second->Delete_Object(pObject);
	return S_OK;
}

CObject* const CObject_Mgr::Find_Prototype(const _tchar* const& pPrototypeKey) const {
	auto iter = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), [&](map<wstring, class CObject*>::value_type sour) {
		return sour.first == pPrototypeKey;
		});
	if (m_mapPrototype.end() == iter)
		return nullptr;
	return iter->second;
}

CLayer* const CObject_Mgr::Find_Layer(const _ubyte& byScene, const _tchar* const& pLayerKey) const {
	if (byScene >= m_byScene || nullptr == m_pMapLayer)
		return nullptr;
	auto iter = m_pMapLayer[byScene].find(pLayerKey);
	//auto iter = find_if(m_pMapLayer[byScene].begin(), m_pMapLayer[byScene].end(), [&](map<wstring, class CLayer*>::value_type sour) {
	//	return sour.first == pLayerKey;
	//	});
	if (m_pMapLayer[byScene].end() == iter)
		return nullptr;
	return iter->second;
}
