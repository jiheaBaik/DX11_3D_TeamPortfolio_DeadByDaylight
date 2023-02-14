#include "pch.h"
#include "Inspector.h"

CInspector* const CInspector::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CInspector* pInstnace = new CInspector(pDevice, pContext);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CInspector::CInspector(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CEdit(pDevice, pContext) {
}

void CInspector::Delete(void) {
	__super::Delete();
}

const HRESULT CInspector::Init(void) {
	return S_OK;
}

void CInspector::Update(void) {
	if (false == m_bOpen)
		return;
	if (true == m_bFocus) {
		ImGui::SetWindowFocus("Inspector");
		m_bFocus = false;
	}

	ImGui::Begin("Inspector", &m_bOpen, m_iWindow_Flag);

	//오브젝트
	if (nullptr == m_pObject) {
		ImGui::End();
		return;
	}
	ImGuiIO& io = ImGui::GetIO();
	//if (true == ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && true == io.WantCaptureMouse) {
		m_pObject->Update_Edit();
	//}
	ImGui::End();
}

CObject* const CInspector::Get_Object(void) {
	return m_pObject;
}

void CInspector::Set_Object(CObject* const& pObject) {
	m_pObject = pObject;
}
