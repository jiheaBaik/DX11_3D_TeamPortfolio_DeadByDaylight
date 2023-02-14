#include "Object.h"
#include "Game_Instance.h"
#include "Component.h"

CObject::CObject(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pNetwork(pNetwork) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pNetwork);
}

CObject::CObject(const CObject& rhs)
	: CBase(),
	m_pDevice(rhs.m_pDevice),
	m_pContext(rhs.m_pContext),
	m_pNetwork(rhs.m_pNetwork) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pNetwork);
}

void CObject::Delete(void) {
	for (auto& pair : m_mapComponent)
		Safe_Release(pair.second);
	m_mapComponent.clear();
	Safe_Release(m_pNetwork);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

CComponent* const CObject::Get_Component(const _tchar* const& pComponentKey) const {
	return Find_Component(pComponentKey);
}

const _bool& CObject::Get_Control(void) {
	return m_bControl;
}

void CObject::Set_Control(const _bool& bControl) {
	m_bControl = bControl;
}

const _int& CObject::Get_Id(void) {
	return m_iId;
}

void CObject::Set_Id(const _int& iId) {
	m_iId = iId;
}

const HRESULT CObject::Add_Component(const _ubyte& byScene, const _tchar* const& pPrototypeKey, const _tchar* const& pComponentKey, CComponent*& ppOut, void* const& pArg) {
	if (nullptr != Find_Component(pComponentKey))
		return E_FAIL;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	Safe_AddRef(pGame_Instance);

	CComponent* pComponent = pGame_Instance->CreateGet_Component_Clone(byScene, pPrototypeKey, pArg);
	if (nullptr == pComponent) {
		Safe_Release(pGame_Instance);
		return E_FAIL;
	}
	m_mapComponent.emplace(pComponentKey, pComponent);
	ppOut = pComponent;
	Safe_AddRef(pComponent);

	Safe_Release(pGame_Instance);
	return S_OK;
}

CComponent* const CObject::Find_Component(const _tchar* const& pComponentKey) const {
	auto iter = m_mapComponent.find(pComponentKey);
	//auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CMapFinder(pComponentKey));
	if (m_mapComponent.end() == iter)
		return nullptr;
	return iter->second;
}
