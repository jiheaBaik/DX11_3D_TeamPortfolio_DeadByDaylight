#include "pch.h"
#include "Hierarchy.h"
#include "Inspector.h"

CHierarchy* const CHierarchy::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CHierarchy* pInstnace = new CHierarchy(pDevice, pContext);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CHierarchy::CHierarchy(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CEdit(pDevice, pContext) {
}

void CHierarchy::Delete(void) {
	__super::Delete();
}

const HRESULT CHierarchy::Init(void) {
	return S_OK;
}

void CHierarchy::Update(void) {
	if (false == m_bOpen)
		return;
	if (true == m_bFocus) {
		ImGui::SetWindowFocus("Hierarchy");
		m_bFocus = false;
	}

	ImGui::Begin("Hierarchy", &m_bOpen, m_iWindow_Flag);
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	EScene eScene = static_cast<EScene>(pGame_Instance->Get_Scene());
	CInspector* pInspector = dynamic_cast<CInspector*>(pGame_Instance->Get_Edit(TEXT("Inspector")));

	//피킹
	if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
		ImGuiIO& io = ImGui::GetIO();
		if (true == ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && false == ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && false == io.WantCaptureMouse) {
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
				map<wstring, class CLayer*>* pMapLayer = pGame_Instance->Get_Object_LayerClone(static_cast<_ubyte>(eScene));
				_float fDist2 = 9999.f;
				CObject* pObject = nullptr;
				for (auto& sour : *pMapLayer) {
					if (L"Sky" == sour.first)
						continue;
					for (auto& dest : sour.second->Get_listObject()) {
						_float fDist = 0.f;
						if (pGame_Instance->Is_Picking(dest, pCamera, &fDist)) {
							if (fDist2 > fDist) {
								fDist2 = fDist;
								pObject = dest;
							}
						}
					}
				}
				if (nullptr != pObject) {
					pInspector->Set_Object(pObject);
					m_bFocus = true;
				}
			}
		}
	}
	//지우기
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_DELETE)) {
		pGame_Instance->Delete_Object_Clone(pInspector->Get_Object());
		pInspector->Set_Object(nullptr);
	}

	//목록
	map<wstring, class CLayer*>* pMapLayer = pGame_Instance->Get_Object_LayerClone(static_cast<_ubyte>(eScene));
	int i = 0;
	for (auto& sour : *pMapLayer) {
		for (auto& dest : sour.second->Get_listObject()) {
			char szFirst[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, sour.first.c_str(), -1, szFirst, MAX_PATH, NULL, NULL);
			char szFirst1[MAX_PATH];
			sprintf_s(szFirst1, "%s(%d)", szFirst, i++);

			if (ImGui::Selectable(szFirst1, pInspector->Get_Object() == dest)) {
				pInspector->Set_Object(dest);
			}
		}
	}

	ImGui::End();
}
