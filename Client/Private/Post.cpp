#include "pch.h"
#include "Post.h"

CPost* const CPost::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CPost* pInstnace = new CPost(pDevice, pContext);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CPost::CPost(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CEdit(pDevice, pContext) {
}

void CPost::Delete(void) {
	__super::Delete();
}

const HRESULT CPost::Init(void) {
	m_bOpen = false;
	m_pCompute = CCompute::Get_Instance();
	m_pFragment = CFragment::Get_Instance();
	return S_OK;
}

void CPost::Update(void) {
	if (false == m_bOpen)
		return;
	if (true == m_bFocus) {
		ImGui::SetWindowFocus("Post");
		m_bFocus = false;
	}
	ImGui::Begin("Post", &m_bOpen, m_iWindow_Flag);
	m_pFragment->Update_Edit(0);
	m_pCompute->Update_Edit(0);
	ImGui::End();
}
