#include "pch.h"
#include "Loading.h"
#include "Loader.h"
#include "UI_Image.h"

#include "Scene_Logo.h"
#include "Scene_Lobby.h"
#include "Scene_GamePlay.h"

CLoading* const CLoading::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, const EScene& eScene) {
	CLoading* pInstnace = new CLoading(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init(eScene)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CLoading::CLoading(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CScene(pDevice, pContext, pNetwork) {
}

void CLoading::Delete(void) {
	__super::Delete();
	Safe_Release(m_pLoader);
}

const HRESULT CLoading::Init(const EScene& eScene) {
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eScene);
	if (nullptr == m_pLoader)
		return E_FAIL;
	HRESULT hr = S_OK;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	pGame_Instance->Set_LoadingScene(static_cast<_ubyte>(eScene));

	if (EScene::LOBBY == eScene) {
		_int i = -1; 
		pGame_Instance->Create_Object_Clone(TEXT("Loading_Back"), static_cast<_ubyte>(EScene::LOADING), TEXT("Loading_Back"), &i);
		pGame_Instance->Create_Object_Clone(TEXT("Loading_Back"), static_cast<_ubyte>(EScene::LOADING), TEXT("Loading_Back"), &(i = 1));
		pGame_Instance->Create_Object_Clone(TEXT("Loading_Bone"), static_cast<_ubyte>(EScene::LOADING), TEXT("Loading_Bone"));
		for (_uint i = 0; i < 5; ++i)
			pGame_Instance->Create_Object_Clone(TEXT("Loading_Bone2"), static_cast<_ubyte>(EScene::LOADING), TEXT("Loading_Bone"), &i);
	}
	else if (EScene::GAMEPLAY == eScene) {
		_int i = 0;
		pGame_Instance->Create_Object_Clone(TEXT("Loading_Back"), static_cast<_ubyte>(EScene::LOADING), TEXT("Loading_Back"), &i);
		CInfo* pInfo = CInfo::Get_Instance();
		(pInfo->Get_Player()) ? pGame_Instance->Create_Object_Clone(TEXT("UI_Loading_ToPlay_Slasher"), static_cast<_ubyte>(EScene::LOADING), TEXT("UI_Loading_ToPlay_Slasher")) :
			pGame_Instance->Create_Object_Clone(TEXT("UI_Loading_ToPlay_Camper"), static_cast<_ubyte>(EScene::LOADING), TEXT("UI_Loading_ToPlay_Camper"));
	}
	return hr;
}

void CLoading::Update(const _double& dTime_Delta) {
	if (false == m_pLoader->Get_IsFinish())
		return;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	CScene* pScene = nullptr;
	EScene eScene = m_pLoader->Get_EScene();
	switch (eScene) {
	case EScene::LOGO:
		pScene = CScene_Logo::Create(m_pDevice, m_pContext, m_pNetwork);
		break;
	case EScene::LOBBY:
		pScene = CScene_Lobby::Create(m_pDevice, m_pContext, m_pNetwork);
		break;
	case EScene::GAMEPLAY:
		pScene = CScene_GamePlay::Create(m_pDevice, m_pContext, m_pNetwork);
		break;
	}
	if (nullptr == pScene) {
		MSG_BOX("Fail to Scene");
		return;
	}

	if (FAILED(pGame_Instance->Change_Scene(static_cast<_ubyte>(eScene), pScene))) {
		Safe_Release(pScene);
		return;
	}
}

void CLoading::Late_Update(const _double& dTime_Delta) {

}