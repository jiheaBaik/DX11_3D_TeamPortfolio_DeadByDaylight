#include "pch.h"
#include "Asset.h"

CAsset* const CAsset::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CAsset* pInstnace = new CAsset(pDevice, pContext);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CAsset::CAsset(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CEdit(pDevice, pContext) {
}

void CAsset::Delete(void) {
	__super::Delete();
}

const HRESULT CAsset::Init(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pMapPrototype = pGame_Instance->Get_Object_MapPrototype();
	return S_OK;
}

void CAsset::Update(void) {
	if (false == m_bOpen)
		return;
	if (true == m_bFocus) {
		ImGui::SetWindowFocus("Asset");
		m_bFocus = false;
	}

	ImGui::Begin("Asset", &m_bOpen, m_iWindow_Flag);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	EScene eScene = static_cast<EScene>(pGame_Instance->Get_Scene());

	//피킹
	ImGuiIO& io = ImGui::GetIO();
	if (true == ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && false == ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && false == io.WantCaptureMouse) {
		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			map<wstring, class CLayer*>* pMapLayer = pGame_Instance->Get_Object_LayerClone(static_cast<_ubyte>(eScene));
			for (auto& sour : *pMapLayer) {
				_bool bBreak = false;
				if (L"Sky" == sour.first)
					continue;
				for (auto& dest : sour.second->Get_listObject()) {
					_float3 vPos;
					if (pGame_Instance->Is_Picking(CCamera::Get_Camera(static_cast<_uint>(0)), dest, &vPos)) {
						CObject* pObject = pGame_Instance->CreateGet_Object_Clone(m_szPrototypeKey, static_cast<_ubyte>(eScene), m_szPrototypeKey);
						CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Transform")));
						pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos));
						m_bFocus = true;
						bBreak = true;
						break;
					}
				}
				if (true == bBreak)
					break;
			}
		}
	}

	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_INSERT) || pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F1)) {
		wstring wstr = m_szPrototypeKey;
		_tchar szLayerKey[MAX_PATH] = TEXT("");
		wsprintfW(szLayerKey, TEXT("%s"), wstr.c_str());
		CObject* pObject = pGame_Instance->CreateGet_Object_Clone(m_szPrototypeKey, static_cast<_ubyte>(eScene), szLayerKey);
	}

	for (auto& pair : *m_pMapPrototype) {
		char szFirst[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, pair.first.c_str(), -1, szFirst, MAX_PATH, NULL, NULL);
		if (ImGui::Selectable(szFirst, false == wcscmp(pair.first.c_str(), m_szPrototypeKey))) {
			ZeroMemory(m_szPrototypeKey, sizeof(MAX_PATH));
			wcscpy_s(m_szPrototypeKey, pair.first.c_str());
		}
	}
	ImGui::End();
}