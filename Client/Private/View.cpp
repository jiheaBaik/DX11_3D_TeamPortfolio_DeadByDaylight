#include "pch.h"
#include "View.h"

CView* const CView::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CView* pInstnace = new CView(pDevice, pContext);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CView::CView(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CEdit(pDevice, pContext) {
}

void CView::Delete(void) {
	__super::Delete();
}

const HRESULT CView::Init(void) {
	m_bOpen = false;
	CTarget_Mgr* pTarget_Mgr = CTarget_Mgr::Get_Instance();
	m_pTarget = pTarget_Mgr->Get_Target(TEXT("Diffuse"));
	return S_OK;
}

void CView::Update(void) {
	if (false == m_bOpen)
		return;
	if (true == m_bFocus) {
		ImGui::SetWindowFocus("View");
		m_bFocus = false;
	}
	ImGui::Begin("View", &m_bOpen, m_iWindow_Flag);

	CTarget_Mgr* pTarget_Mgr = CTarget_Mgr::Get_Instance();

	if (ImGui::TreeNode("Selection State: Single Selection")) {
		map<wstring, class CTarget*> mapTarget = pTarget_Mgr->Get_MapTarget();
		for (auto& iter : mapTarget) {
			char szFirst[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, iter.first.c_str(), -1, szFirst, MAX_PATH, NULL, NULL);
			if (ImGui::Selectable(szFirst, m_pTarget == iter.second))
				m_pTarget = iter.second;
		}
		ImGui::TreePop();
	}

	ID3D11ShaderResourceView* pShaderResourceView = m_pTarget->Get_ShaderResourceView();
	float my_tex_w = 500.f;
	float my_tex_h = 500.f;

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::Image(pShaderResourceView, ImVec2(my_tex_w, my_tex_h), ImVec2{ 0.f, 0.f }, ImVec2{ 1.f, 1.f }, ImVec4{ 1.f, 1.f, 1.f, 1.f }, ImVec4{ 1.f, 1.f, 1.f, 0.5f });
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		D3D11_TEXTURE2D_DESC Texture2D_Desc = m_pTarget->Get_Texture2d_Desc();
		D3D11_TEXTURE2D_DESC Texture2D_Desc2;
		ZeroMemory(&Texture2D_Desc2, sizeof(D3D11_TEXTURE2D_DESC));
		Texture2D_Desc2.Width = Texture2D_Desc.Width;
		Texture2D_Desc2.Height = Texture2D_Desc.Height;
		Texture2D_Desc2.MipLevels = Texture2D_Desc.MipLevels;
		Texture2D_Desc2.ArraySize = Texture2D_Desc.ArraySize;
		Texture2D_Desc2.Format = Texture2D_Desc.Format;
		Texture2D_Desc2.SampleDesc = Texture2D_Desc.SampleDesc;
		Texture2D_Desc2.Usage = D3D11_USAGE_STAGING;
		Texture2D_Desc2.BindFlags = 0;
		Texture2D_Desc2.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		Texture2D_Desc2.MiscFlags = 0;
		ID3D11Texture2D* pTexture2 = nullptr;
		m_pDevice->CreateTexture2D(&Texture2D_Desc2, nullptr, &pTexture2);
		ID3D11Texture2D* pTexture = m_pTarget->Get_Texture();
		m_pContext->CopyResource(pTexture2, pTexture);

		ImGuiIO& io = ImGui::GetIO();
		_float2 vTexCoord = { (io.MousePos.x - pos.x) / my_tex_w, (io.MousePos.y - pos.y) / my_tex_h };
		ImGui::Text("x:%d, y:%d", _uint(vTexCoord.x * Texture2D_Desc2.Width), _uint(vTexCoord.y * Texture2D_Desc2.Height));

		D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
		ZeroMemory(&Mapped_SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		m_pContext->Map(pTexture2, 0, D3D11_MAP_READ, 0, &Mapped_SubResource);

		switch (Texture2D_Desc2.Format) {
		case DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM: {
			_ulong iCol = _uint(vTexCoord.x * Texture2D_Desc2.Width) * 4;
			_ulong iRow = _uint(vTexCoord.y * Texture2D_Desc2.Height) * Mapped_SubResource.RowPitch;

			ImGui::Text("r:%d, g:%d, b:%d, a:%d",
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2],
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1],
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0],
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3]
			);
			break;
		}
		case DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_UNORM: 
		case DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT:{
			_ulong iCol = _uint(vTexCoord.x * Texture2D_Desc2.Width) * 8;
			_ulong iRow = _uint(vTexCoord.y * Texture2D_Desc2.Height) * Mapped_SubResource.RowPitch;

			ImGui::Text("r:%d, g:%d, b:%d, a:%d",
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1],
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3],
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 5],
				static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 7]
			);
			break;
		}
		case DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT: {
			_ulong iCol = _uint(vTexCoord.x * Texture2D_Desc2.Width) * 16;
			_ulong iRow = _uint(vTexCoord.y * Texture2D_Desc2.Height) * Mapped_SubResource.RowPitch;
			break;
		}
		}

		m_pContext->Unmap(pTexture2, 0);

		Safe_Release(pTexture2);
		ImGui::EndTooltip();
	}
	ImGui::End();
}
