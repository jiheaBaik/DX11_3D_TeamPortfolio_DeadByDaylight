#include "Layer.h"
#include "Object.h"

CLayer* const CLayer::Create(void) {
	CLayer* pInstnace = new CLayer;
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CLayer::CLayer(void)
	: CBase() {
}

void CLayer::Delete(void) {
	for (auto& iter : m_listObject)
		Safe_Release(iter);
	m_listObject.clear();
}

const HRESULT CLayer::Init(void) {
	return S_OK;
}

const _ubyte CLayer::Update_Object(const _double& dTimeDelta) {
	for (auto iter = m_listObject.begin(); iter != m_listObject.end();) {
		switch ((*iter)->Update(dTimeDelta)) {
		case OBJ_ERASE:
			Safe_Release(*iter);
			iter = m_listObject.erase(iter);
			break;
		case OBJ_SCENE:
			return OBJ_SCENE;
			break;
		case OBJ_NOEVENT:
			++iter;
			break;
		}
	}
	return OBJ_NOEVENT;
}

void CLayer::Late_Update_Object(const _double& dTimeDelta) const {
	for (auto& iter : m_listObject)
		iter->Late_Update(dTimeDelta);
}

CObject* const CLayer::Get_Object(const _uint& iIndex) const {
	if (iIndex >= m_listObject.size())
		return nullptr;
	auto iter = m_listObject.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;
	return *iter;
}

CObject* const CLayer::Get_ObjectID(const _uint& iID) const {
	for (auto iter = m_listObject.begin(); iter != m_listObject.end(); ++iter) {
		if ((*iter)->Get_Id() == iID)
			return *iter;
	}
	return nullptr;
}

CObject* const CLayer::Get_ObjectControl(void) const {
	for (auto iter = m_listObject.begin(); iter != m_listObject.end(); ++iter) {
		if (true == (*iter)->Get_Control())
			return *iter;
	}
	return nullptr;
};

list<class CObject*>& CLayer::Get_listObject(void) {
	return m_listObject;
}

const HRESULT CLayer::Add_Object(CObject* const& pObject) {
	if (nullptr == pObject)
		return E_FAIL;
	m_listObject.emplace_back(pObject);
	return S_OK;
}

const HRESULT CLayer::Delete_Object(const CObject* const& pObject) {
	if (nullptr == pObject)
		return E_FAIL;
	for (auto iter = m_listObject.begin(); iter != m_listObject.end(); ++iter)
		if (*iter == pObject) {
			Safe_Release(*iter);
			m_listObject.erase(iter);
			break;
		}
	return S_OK;
}
