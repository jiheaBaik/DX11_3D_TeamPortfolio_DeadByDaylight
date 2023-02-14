#include "Scene_Mgr.h"
#include "Scene.h"
#include "Game_Instance.h"

IMPLEMENT_SINGLETON(CScene_Mgr)

CScene_Mgr::CScene_Mgr(void) 
	: CBase() {
}

void CScene_Mgr::Delete(void) {
	Safe_Release(m_pScene);
}

void CScene_Mgr::Update_Scene(const _double& dTimeDelta) {
	if (nullptr == m_pScene)
		return;
	m_pScene->Update(dTimeDelta);
}

void CScene_Mgr::Late_Update_Scene(const _double& dTimeDelta) {
	if (nullptr == m_pScene)
		return;
	m_pScene->Late_Update(dTimeDelta);
}

const _ubyte& CScene_Mgr::Get_LoadingScene(void) const {
	return m_byLoadingScene;
}

void CScene_Mgr::Set_LoadingScene(const _ubyte& byLoadingScene) {
	m_byLoadingScene = byLoadingScene;
}

const _ubyte& CScene_Mgr::Get_Scene(void) const {
	return m_byScene;
}

CScene* const& CScene_Mgr::Get_Scene2(void) const {
	return m_pScene;
}

const HRESULT CScene_Mgr::Change_Scene(const _ubyte& byScene, CScene* const& pScene) {
	if (nullptr == pScene)
		return E_FAIL;
	if (nullptr != m_pScene)
		Clear_Scene();
	Safe_Release(m_pScene);
	m_pScene = pScene;
	m_byScene = byScene;
	return S_OK;
}

const HRESULT CScene_Mgr::Clear_Scene(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	Safe_AddRef(pGame_Instance);
	pGame_Instance->Clear_Object_Clone(m_byScene);
	pGame_Instance->Clear_Component_Prototype(m_byScene);
	Safe_Release(pGame_Instance);
	return S_OK;
}
