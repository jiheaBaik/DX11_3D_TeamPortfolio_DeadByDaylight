#include "Edit_Mgr.h"
#include "Edit.h"

IMPLEMENT_SINGLETON(CEdit_Mgr)

CEdit_Mgr::CEdit_Mgr(void)
	: CBase() {
}

void CEdit_Mgr::Delete(void) {
	for (auto& iter : m_vecEdit)
		Safe_Release(iter);
	m_vecEdit.clear();
	for (auto& pair : m_mapEdit)
		Safe_Release(pair.second);
	m_mapEdit.clear();

	if (EMode::PLAY == m_eMode)
		return;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CEdit_Mgr::Init(const HWND& hWnd, ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EMode& eMode) {
	m_eMode = eMode;
	if (EMode::PLAY == m_eMode)
		return S_OK;

	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);
	m_pContext = pContext;
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	return S_OK;
}

void CEdit_Mgr::Update(void) {
	if (EMode::PLAY == m_eMode)
		return;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	for (auto& iter : m_vecEdit)
		if(nullptr != iter)
			iter->Update();
}

const HRESULT CEdit_Mgr::Render(void) {
	if (EMode::PLAY == m_eMode)
		return S_OK;
	for (auto& iter : m_vecEdit)
		if (nullptr != iter)
			iter->Render();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	return S_OK;
}

const CEdit_Mgr::EMode& CEdit_Mgr::Get_Mode(void) const {
	return m_eMode;
}

void CEdit_Mgr::Set_Mode(const EMode& eMode) {
	m_eMode = eMode;
}

const HRESULT CEdit_Mgr::Add_Edit(const _tchar* const& pEditKey, CEdit* const& pEdit) {
	if (EMode::PLAY == m_eMode)
		return S_OK;
	if (nullptr != Find_Edit(pEditKey))
		return E_FAIL;
	m_mapEdit.emplace(pEditKey, pEdit);
	m_vecEdit.emplace_back(pEdit);
	Safe_AddRef(pEdit);
	return S_OK;
}

CEdit* const CEdit_Mgr::Get_Edit(const _tchar* const& pEditKey) {
	return Find_Edit(pEditKey);
}

CEdit* const CEdit_Mgr::Find_Edit(const _tchar* pEditKey) const {
	auto iter = find_if(m_mapEdit.begin(), m_mapEdit.end(), CMapFinder(pEditKey));
	if (m_mapEdit.end() == iter)
		return nullptr;
	return iter->second;
}
