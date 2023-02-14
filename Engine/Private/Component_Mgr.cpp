#include "Component_Mgr.h"

IMPLEMENT_SINGLETON(CComponent_Mgr)

CComponent_Mgr::CComponent_Mgr(void)
	: CBase() {
}

void CComponent_Mgr::Delete(void) {
	for (_ubyte i = 0; i < m_byScene; ++i) {
		for (auto& pair : m_pMapPrototype[i])
			Safe_Release(pair.second);
		m_pMapPrototype[i].clear();
	}
	Safe_Delete_Array(m_pMapPrototype);
}

const HRESULT CComponent_Mgr::Init(const _ubyte& byScene) {
	if (nullptr != m_pMapPrototype)
		return E_FAIL;
	m_pMapPrototype = new map<wstring, CComponent*>[byScene];
	if (nullptr == m_pMapPrototype)
		return E_FAIL;
	m_byScene = byScene;
	return S_OK;
}

map<wstring, class CComponent*>& CComponent_Mgr::Get_Prototype(void) {
	return *m_pMapPrototype;
}

CComponent* CComponent_Mgr::Get_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey) {
	CComponent* pPrototype = Find_Prototype(byScene, pPrototypeKey);
	return pPrototype;
}

const HRESULT CComponent_Mgr::Add_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey, CComponent* const& pComponent) const {
 	if (byScene >= m_byScene || nullptr == m_pMapPrototype)
		return E_FAIL;
	if (nullptr != Find_Prototype(byScene, pPrototypeKey))
		return E_FAIL;
	m_pMapPrototype[byScene].emplace(pPrototypeKey, pComponent);
	return S_OK;
}

CComponent* const CComponent_Mgr::CreateGet_Clone(const _ubyte& byScene, const _tchar* const& pPrototypeKey, void* const& pArg) const {
	if (byScene >= m_byScene || nullptr == m_pMapPrototype)
		return nullptr;
	CComponent* pPrototype = Find_Prototype(byScene, pPrototypeKey);
	if (nullptr == pPrototype)
		return nullptr;
	return pPrototype->Clone(pArg);
}

const HRESULT CComponent_Mgr::Clear_Prototype(const _ubyte& byScene) const {
	if (byScene >= m_byScene || nullptr == m_pMapPrototype)
		return E_FAIL;
	for (auto& pair : m_pMapPrototype[byScene])
		Safe_Release(pair.second);
	m_pMapPrototype[byScene].clear();
	return S_OK;
}

CComponent* CComponent_Mgr::Find_Prototype(const _ubyte& byScene, const _tchar* const& pPrototypeKey) const {
	if (byScene >= m_byScene || nullptr == m_pMapPrototype)
		return nullptr;
	auto iter = m_pMapPrototype[byScene].find(pPrototypeKey);
	//auto iter = find_if(m_pMapPrototype[byScene].begin(), m_pMapPrototype[byScene].end(), [&](map<wstring, class CComponent*>::value_type sour) {
	//	return sour.first == pPrototypeKey;
	//	});
	if (m_pMapPrototype[byScene].end() == iter)
		return nullptr;
	return iter->second;
}