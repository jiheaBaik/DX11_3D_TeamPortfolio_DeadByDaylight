#include "pch.h"
#include "NavMesh.h"
#include "Inspector.h"

CNavMesh* const CNavMesh::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CNavMesh* pInstnace = new CNavMesh(pDevice, pContext);
	if (FAILED(pInstnace->Init()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CNavMesh::CNavMesh(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	:CEdit(pDevice, pContext) {
}

void CNavMesh::Delete(void) {
	__super::Delete();
	Safe_Release(m_pNavi);
}

const HRESULT CNavMesh::Init(void) {
	m_bOpen = false;
	return S_OK;
}

void CNavMesh::Update(void) {
	if (false == m_bOpen)
		return;
	if (true == m_bFocus) {
		ImGui::SetWindowFocus("NavMesh");
		m_bFocus = false;
	}

	ImGui::Begin("NavMesh", &m_bOpen, m_iWindow_Flag);
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	//메쉬 찾기
	if (nullptr == m_pNavi) {
		Safe_Release(m_pNavi);
		m_pNavi = dynamic_cast<CNavi*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_Scene(), TEXT("Navi")));
		ImGui::End();
		return;
	}

	static int iEditMode = 0;
	ImGui::Text("Mode");
	ImGui::RadioButton("Create", &iEditMode, 0); ImGui::SameLine();
	ImGui::RadioButton("Modify", &iEditMode, 1);
	ImGui::Separator();

	ImGuiIO& io = ImGui::GetIO();
	switch (iEditMode) {
	case 0: {
		static int iType = 0;
		ImGui::Text("Type");
		ImGui::RadioButton("Floor", &iType, 0); ImGui::SameLine();
		ImGui::RadioButton("Wall", &iType, 1); ImGui::SameLine();
		ImGui::RadioButton("Ceiling", &iType, 2);

		static bool bSnap = true;
		ImGui::Checkbox("Snap", &bSnap);

		//피킹
		if (true == ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && false == ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && false == io.WantCaptureMouse) {
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
				EScene eScene = static_cast<EScene>(pGame_Instance->Get_Scene());
				map<wstring, class CLayer*>* pMapLayer = pGame_Instance->Get_Object_LayerClone(static_cast<_ubyte>(eScene));
				_float fDist2 = 9999.f;

				for (auto& sour : *pMapLayer) {
					if (L"Sky" == sour.first)
						continue;
					for (auto& dest : sour.second->Get_listObject()) {
						_float fDist = 0.f;
						if (pGame_Instance->Is_Picking(dest, pCamera, &fDist)) {
							if (fDist2 > fDist) {
								fDist2 = fDist;
							}
						}
					}
				}
				if (fDist2 != 9999.f) {
					_float3 vDir;
					_float3 vPos;
					Get_Ray(pCamera, &vDir, &vPos);
					XMStoreFloat3(&vDir, XMVector3Normalize(XMLoadFloat3(&vDir)));
					_float3 vRay;
					XMStoreFloat3(&vRay, XMLoadFloat3(&vPos) + XMLoadFloat3(&vDir) * fDist2);
					if (bSnap)
						m_pNavi->Check_Cell(&vRay);
					m_vClickPos[m_iClickIndex++] = vRay;
					m_bFocus = true;
					if (m_iClickIndex > 2) {
						m_pNavi->Create_Cell(m_vClickPos, static_cast<CCell::EType>(iType));
						m_iClickIndex = 0;
					}
				}
			}
			//if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			//	_float3 vPos;
			//	for (auto& sour : *pMapLayer) {
			//		if (sour.first == TEXT("Sky"))
			//			continue;
			//		_bool bStop = false;
			//		for (auto& dest : sour.second->Get_listObject()) {
			//			if (pGame_Instance->Is_Picking(CCamera::Get_Camera(static_cast<_uint>(0)), dest, &vPos)) {
			//				if (bSnap)
			//					m_pNavi->Check_Cell(&vPos);
			//				m_vClickPos[m_iClickIndex++] = vPos;
			//				m_bFocus = true;
			//				bStop = true;
			//				break;
			//			}
			//		}
			//		if (bStop)
			//			break;
			//	}
			//	if (m_iClickIndex > 2) {
			//		m_pNavi->Create_Cell(m_vClickPos, static_cast<CCell::EType>(iType));
			//		m_iClickIndex = 0;
			//	}
			//}
		}
		ImGui::Text("Index");
		ImGui::Text(to_string(m_iClickIndex).c_str());
		if (ImGui::Button("Reset")) {
			m_iClickIndex = 0;
		}

		ImGui::Text("Collider");
		CInspector* pInspector = dynamic_cast<CInspector*>(pGame_Instance->Get_Edit(TEXT("Inspector")));
		CObject* pObject = pInspector->Get_Object();
		if (nullptr != pObject) {
			CCollider* pCollider = dynamic_cast<CCollider*>(pObject->Get_Component(TEXT("Collider")));
			if (nullptr != pCollider) {
				if (ImGui::Button("Add_Navi")) {
					_float3* pCorner = pCollider->Get_Corner();
					_float3 pFace[12][3];

					pFace[0][0] = pCorner[1]; pFace[1][0] = pCorner[1];
					pFace[0][1] = pCorner[3]; pFace[1][1] = pCorner[2];
					pFace[0][2] = pCorner[0]; pFace[1][2] = pCorner[3];

					pFace[2][0] = pCorner[4]; pFace[3][0] = pCorner[4];
					pFace[2][1] = pCorner[6]; pFace[3][1] = pCorner[7];
					pFace[2][2] = pCorner[5]; pFace[3][2] = pCorner[6];

					pFace[4][0] = pCorner[0]; pFace[5][0] = pCorner[0];
					pFace[4][1] = pCorner[7]; pFace[5][1] = pCorner[3];
					pFace[4][2] = pCorner[4]; pFace[5][2] = pCorner[7];

					pFace[6][0] = pCorner[5]; pFace[7][0] = pCorner[5];
					pFace[6][1] = pCorner[2]; pFace[7][1] = pCorner[6];
					pFace[6][2] = pCorner[1]; pFace[7][2] = pCorner[2];

					pFace[8][0] = pCorner[7]; pFace[9][0] = pCorner[7];
					pFace[8][1] = pCorner[2]; pFace[9][1] = pCorner[3];
					pFace[8][2] = pCorner[6]; pFace[9][2] = pCorner[2];

					pFace[10][0] = pCorner[0]; pFace[11][0] = pCorner[0];
					pFace[10][1] = pCorner[5]; pFace[11][1] = pCorner[4];
					pFace[10][2] = pCorner[1]; pFace[11][2] = pCorner[5];
					for (_uint i = 0; i < 8; ++i) {
						m_pNavi->Create_Cell(pFace[i], CCell::EType::WALL);
					}
					for (_uint i = 8; i < 10; ++i) {
						m_pNavi->Create_Cell(pFace[i], CCell::EType::FLOOR);
					}
					for (_uint i = 10; i < 12; ++i) {
						m_pNavi->Create_Cell(pFace[i], CCell::EType::CEILING);
					}
				}
			}
		}
	}
		  break;
	case 1: {
		static int iType2 = 0;
		//피킹
		if (true == ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && false == ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && false == io.WantCaptureMouse) {
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				for (auto& iter : m_pNavi->Get_Cell())
					if (true == pGame_Instance->Is_Picking(CCamera::Get_Camera(static_cast<_uint>(0)), iter)) {
						m_pCell = iter;
						iType2 = static_cast<int>(m_pCell->Get_Type());
						m_bFocus = true;
					}
			}
		}

		//네비 셀

		if (ImGui::BeginListBox("NaviCell", ImVec2{ 0,100 })) {
			int i = 0;
			for (auto& iter : m_pNavi->Get_Cell()) {
				if (ImGui::Selectable(to_string(i).c_str(), iter == m_pCell)) {
					m_pCell = m_pNavi->Get_Cell()[i];
				}
				if (iter == m_pCell)
					m_iIndex = i;
				i++;
			}
			ImGui::EndListBox();
		}
		ImGui::Text("Index");
		ImGui::Text(to_string(m_iIndex).c_str());


		ImGui::Text("Type");
		if (ImGui::RadioButton("Floor", &iType2, 0)) {
			m_pCell->Set_Type(static_cast<CCell::EType>(iType2));
		}; ImGui::SameLine();
		if (ImGui::RadioButton("Wall", &iType2, 1)) {
			m_pCell->Set_Type(static_cast<CCell::EType>(iType2));
		}; ImGui::SameLine();
		if (ImGui::RadioButton("Ceiling", &iType2, 2)) {
			m_pCell->Set_Type(static_cast<CCell::EType>(iType2));
		};
		if (ImGui::RadioButton("NoCameraWall", &iType2, 3)) {
			m_pCell->Set_Type(static_cast<CCell::EType>(iType2));
		};

		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_M)) {
			m_pNavi->Delete_Cell(m_pCell);
			m_pCell = nullptr;
		}
		if (ImGui::Button("Delete")) {
			m_pNavi->Delete_Cell(m_pCell);
			m_pCell = nullptr;
		}
	}
		  break;
	}

	m_bNaviRender = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	ImGui::End();
}

const HRESULT CNavMesh::Render(void) {
	if (nullptr == m_pNavi)
		return S_OK;
	if (true == m_bNaviRender)
		m_pNavi->Render();

	return S_OK;
}

void CNavMesh::Set_Navi(CNavi* const& pNavi) {
	Safe_Release(m_pNavi);
	m_pNavi = pNavi;
}

void CNavMesh::Save_Data(const _tchar* const& pFilePath) {
	if (nullptr == m_pNavi)
		return;
	ofstream ofs(pFilePath, ios::binary);
	m_pNavi->Save_Data(ofs);
	ofs.close();
}



void CNavMesh::Get_Ray(CCamera* const& pCamera, _float3* const& vOutDir, _float3* const& vOutPos) {
	//마우스 커서 받기
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(g_hWnd, &ptCursor);

	//뷰포트 받기
	_uint iViewport = 0;
	m_pContext->RSGetViewports(&iViewport, nullptr);
	D3D11_VIEWPORT* pViewport = new D3D11_VIEWPORT[iViewport];
	m_pContext->RSGetViewports(&iViewport, pViewport);

	//광선 좌표 방향 구하기
	_vector vRayDir = XMVectorSet(ptCursor.x / (pViewport[0].Width * 0.5f) - 1.f, ptCursor.y / (pViewport[0].Height * -0.5f) + 1.f, 0.f, 1.f);
	Safe_Delete_Array(pViewport);
	//투영행렬 곱하기
	_matrix ProjMatrixInverse = pCamera->Get_ProjMatrix_Inverse();
	vRayDir = XMVector3TransformCoord(vRayDir, ProjMatrixInverse);

	//뷰행렬 곱하기
	_vector vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_matrix ViewMatrixInverse = pCamera->Get_ViewMatrix_Inverse();
	vRayPos = XMVector3TransformCoord(vRayPos, ViewMatrixInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInverse);

	XMStoreFloat3(vOutDir, vRayDir);
	XMStoreFloat3(vOutPos, vRayPos);
}
