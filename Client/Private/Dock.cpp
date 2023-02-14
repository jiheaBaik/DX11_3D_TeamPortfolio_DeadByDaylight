#include "pch.h"
#include "Dock.h"

#include "Loading.h"
#include "Inspector.h"
#include "NavMesh.h"

CDebug* const CDebug::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CDebug* pInstnace = new CDebug(pDevice, pContext);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CDebug::CDebug(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CEdit(pDevice, pContext) {
}

void CDebug::Delete(void) {
	__super::Delete();
}

const HRESULT CDebug::Init(void) {
	//CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	//IM_ASSERT(font != NULL);
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Roboto-Regular.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
	//setlocale(LC_ALL, "Korean");
	return S_OK;
}

void CDebug::Update(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	EScene eScene = static_cast<EScene>(pGame_Instance->Get_Scene());
	CNetwork* pNetwork = pGame_Instance->Get_Network();

	const char* items[] = { "Static", "Loading", "Logo", "Lobby", "GamePlay" };
	static int item_current_idx = 0;
	const char* combo_preview_value = items[item_current_idx];
	if (ImGui::BeginCombo("Scene", combo_preview_value)) {
		for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected)) {
				item_current_idx = n;
				dynamic_cast<CInspector*>(pGame_Instance->Get_Edit(TEXT("Inspector")))->Set_Object(nullptr);
				dynamic_cast<CNavMesh*>(pGame_Instance->Get_Edit(TEXT("NavMesh")))->Set_Navi(nullptr);
				pGame_Instance->Clear_Scene();
				pGame_Instance->Change_Scene(static_cast<_ubyte>(EScene::LOADING), CLoading::Create(m_pDevice, m_pContext, pNetwork, static_cast<EScene>(item_current_idx)));
			}		
		}
		ImGui::EndCombo();
	}

	//
	ImGui::Checkbox("Asset", (_bool*)&pGame_Instance->Get_Edit(TEXT("Asset"))->Get_Open()); ImGui::SameLine();
	ImGui::Checkbox("Hierarchy", (_bool*)&pGame_Instance->Get_Edit(TEXT("Hierarchy"))->Get_Open()); ImGui::SameLine();
	ImGui::Checkbox("Inspector", (_bool*)&pGame_Instance->Get_Edit(TEXT("Inspector"))->Get_Open());
	ImGui::Checkbox("View", (_bool*)&pGame_Instance->Get_Edit(TEXT("View"))->Get_Open()); ImGui::SameLine();
	ImGui::Checkbox("Post", (_bool*)&pGame_Instance->Get_Edit(TEXT("Post"))->Get_Open()); ImGui::SameLine();
	ImGui::Checkbox("NavMesh", (_bool*)&pGame_Instance->Get_Edit(TEXT("NavMesh"))->Get_Open());

	//ImGui::Checkbox("Edit_Terrain", (_bool*)&pGame_Instance->Get_Edit(TEXT("Edit_Terrain"))->Get_Open());

	//CEdit* pAsset = pGame_Instance->Get_Edit(TEXT("Asset"));
	//if (ImGui::MenuItem("Asset", "", true == pAsset->Get_Open())) {
	//	pAsset->Set_Open(!pAsset->Get_Open());
	//}
	//CEdit* pInspector = pGame_Instance->Get_Edit(TEXT("Edit_Inspector"));
	//if (ImGui::MenuItem("Inspector", "", true == pInspector->Get_Open())) {
	//	pInspector->Set_Open(!pInspector->Get_Open());
	//}
	//CEdit* pHierarchy = pGame_Instance->Get_Edit(TEXT("Edit_Hierarchy"));
	//if (ImGui::MenuItem("Hierarchy", "", true == pHierarchy->Get_Open())) {
	//	pHierarchy->Set_Open(!pHierarchy->Get_Open());
	//}
	//CEdit* pNavMesh = pGame_Instance->Get_Edit(TEXT("Edit_NavMesh"));
	//if (ImGui::MenuItem("NavMesh", "", true == pNavMesh->Get_Open())) {
	//	pNavMesh->Set_Open(!pNavMesh->Get_Open());
	//}
	//ImGui::EndMenu();

	//Play
	if (ImGui::Button("Play")) {
		pGame_Instance->Set_Edit_Mode(CEdit_Mgr::EMode::DEBUG);
		dynamic_cast<CInspector*>(pGame_Instance->Get_Edit(TEXT("Inspector")))->Set_Object(nullptr);
		dynamic_cast<CNavMesh*>(pGame_Instance->Get_Edit(TEXT("NavMesh")))->Set_Navi(nullptr);
		pGame_Instance->Clear_Scene();
		pGame_Instance->Change_Scene(static_cast<_ubyte>(EScene::LOADING), CLoading::Create(m_pDevice, m_pContext, pNetwork, eScene));
	} ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		pGame_Instance->Set_Edit_Mode(CEdit_Mgr::EMode::EDIT);
		dynamic_cast<CInspector*>(pGame_Instance->Get_Edit(TEXT("Inspector")))->Set_Object(nullptr);
		dynamic_cast<CNavMesh*>(pGame_Instance->Get_Edit(TEXT("NavMesh")))->Set_Navi(nullptr);
		pGame_Instance->Clear_Scene();
		pGame_Instance->Change_Scene(static_cast<_ubyte>(EScene::LOADING), CLoading::Create(m_pDevice, m_pContext, pNetwork, eScene));
	} ImGui::SameLine();

	//Save
	if (ImGui::Button("Save")) {
		Json::Value root;
		map<wstring, class CLayer*>* pMapLayer = pGame_Instance->Get_Object_LayerClone(static_cast<_ubyte>(eScene));
		for (auto& sour : *pMapLayer)
			for (auto& dest : sour.second->Get_listObject()) {
				char sz[MAX_PATH] = "";
				WideCharToMultiByte(CP_ACP, 0, sour.first.c_str(), -1, sz, MAX_PATH, nullptr, nullptr);
				string str = sz;

				Json::Value obj;
				obj["Name"] = sz;
				if (!FAILED(dest->Save_Data(obj)))
					root.append(obj);
			}

		_tchar szScene[MAX_PATH] = TEXT("");
		switch (eScene) {
		case EScene::LOGO:
			wcscpy_s(szScene, TEXT("01.Logo"));
			break;
		case EScene::LOBBY:
			wcscpy_s(szScene, TEXT("02.Lobby"));
			break;
		case EScene::GAMEPLAY:
			wcscpy_s(szScene, TEXT("03.GamePlay"));
			break;
		}
		_tchar szFilePath[MAX_PATH] = TEXT("");
		wsprintfW(szFilePath, TEXT("../Bin/Resource/%s/Object.txt"), szScene);
		ofstream ofs(szFilePath);
		Json::StyledWriter sw;
		string str = sw.write(root);
		ofs << str;
		ofs.close();

		ZeroMemory(szFilePath, sizeof(_tchar) * MAX_PATH);
		wsprintfW(szFilePath, TEXT("../Bin/Resource/%s/Navi.txt"), szScene);
		CNavMesh* pNavMesh = dynamic_cast<CNavMesh*>(pGame_Instance->Get_Edit(TEXT("NavMesh")));
		pNavMesh->Save_Data(szFilePath);
	}
}