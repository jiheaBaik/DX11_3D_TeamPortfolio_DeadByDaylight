#include "Buffer_Terrain.h"
#include "Game_Instance.h"

CBuffer_Terrain* const CBuffer_Terrain::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CBuffer_Terrain* pInstance = new CBuffer_Terrain(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CBuffer_Terrain::Clone(void* const& pArg) {
	CBuffer_Terrain* pInstance = new CBuffer_Terrain(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CBuffer_Terrain::CBuffer_Terrain(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBuffer(pDevice, pContext) {
}

CBuffer_Terrain::CBuffer_Terrain(const CBuffer_Terrain& rhs)
	: CBuffer(rhs),
	m_iCountX(rhs.m_iCountX),
	m_iCountZ(rhs.m_iCountZ),
	m_fDist(rhs.m_fDist) {
}

void CBuffer_Terrain::Delete(void) {
	__super::Delete();
	Safe_Delete_Array(m_pIndex);
	Safe_Delete_Array(m_pVertex);
	Safe_Delete_Array(m_pVertexPos);
	Safe_Release(m_pShaderResourceView);
	Safe_Release(m_pTexture2D);
}

const HRESULT CBuffer_Terrain::Init_Create(void) {
	return S_OK;
}

const HRESULT CBuffer_Terrain::Init_Clone(void* const& pArg) {
	return S_OK;
}

void CBuffer_Terrain::Update_Edit(const _int& iPushID) {
	ImGui::PushID(iPushID);
	ImGui::Separator();
	ImGui::Text("Terrain");

	ImGui::InputText("Scene", m_szScene, IM_ARRAYSIZE(m_szScene));
	_int iCount[2]{ static_cast<_int>(m_iCountX), static_cast<_int>(m_iCountZ) };
	if (ImGui::InputInt2("Count", iCount)) {
		m_iCountX = iCount[0];
		m_iCountZ = iCount[1];

	} ImGui::SameLine();
	if (ImGui::Button("Create"))
		Create_Terrain(m_iCountX, m_iCountZ);
	if (ImGui::DragFloat("Dist", &m_fDist, 0.01f))
		Set_Dist(m_fDist);
	if (ImGui::DragFloat("TexCoord", &m_fTexCoord, 0.01f))
		Set_TexCoord(m_fTexCoord);

	ImGui::Text("");
	ImGui::Text("Edit");
	ImGui::Text("Mode : "); ImGui::SameLine();
	if (ImGui::Checkbox("Height", &m_bMode[0])) { m_bMode[0] = true; m_bMode[1] = false; } ImGui::SameLine();
	if (ImGui::Checkbox("Brush", &m_bMode[1])) { m_bMode[0] = false; m_bMode[1] = true; }

	ImGui::DragFloat("Radius", &m_fRadius, 0.01f);
	ImGui::DragFloat("Depth", &m_fDepth, 0.01f);
	if (true == m_bMode[0]) {
		if (ImGui::Checkbox("Up", &m_bDepth[0])) { m_bDepth[0] = true; m_bDepth[1] = false; m_bDepth[2] = false; } ImGui::SameLine();
		if (ImGui::Checkbox("Down", &m_bDepth[1])) { m_bDepth[0] = false; m_bDepth[1] = true; m_bDepth[2] = false; } ImGui::SameLine();
		if (ImGui::Checkbox("Fix", &m_bDepth[2])) { m_bDepth[0] = false; m_bDepth[1] = false; m_bDepth[2] = true; }
	}

	if (true == m_bMode[1]) {
		if (ImGui::Checkbox("Brush0", &m_bBrush[0])) { m_bBrush[0] = true; m_bBrush[1] = false; m_bBrush[2] = false; m_bBrush[3] = false; m_bBrush[4] = false; } ImGui::SameLine();
		if (ImGui::Checkbox("Brush1", &m_bBrush[1])) { m_bBrush[0] = false; m_bBrush[1] = true; m_bBrush[2] = false; m_bBrush[3] = false; m_bBrush[4] = false; } ImGui::SameLine();
		if (ImGui::Checkbox("Brush2", &m_bBrush[2])) { m_bBrush[0] = false; m_bBrush[1] = false; m_bBrush[2] = true; m_bBrush[3] = false; m_bBrush[4] = false; } ImGui::SameLine();
		if (ImGui::Checkbox("Brush3", &m_bBrush[3])) { m_bBrush[0] = false; m_bBrush[1] = false; m_bBrush[2] = false; m_bBrush[3] = true; m_bBrush[4] = false; } ImGui::SameLine();
		if (ImGui::Checkbox("Brush4", &m_bBrush[4])) { m_bBrush[0] = false; m_bBrush[1] = false; m_bBrush[2] = false; m_bBrush[3] = false; m_bBrush[4] = true; }
	}

	//ÇÇÅ·
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	ImGuiIO& io = ImGui::GetIO();
	if (false == ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && false == io.WantCaptureMouse) {
		_float3 vPos;
		if (pGame_Instance->Is_Picking(CCamera::Get_Camera(static_cast<_uint>(0)), this, &vPos)) {
			ImGui::Text("Pos: %f", vPos.y);

			_int iIndex = Find_Index(vPos);
			_int x = iIndex / m_iCountX;
			_int z = iIndex % m_iCountX;
			x -= static_cast<_int>(m_fRadius / m_fDist);
			z -= static_cast<_int>(m_fRadius / m_fDist);

			_int x2 = iIndex / m_iCountX;
			_int z2 = iIndex % m_iCountX;
			x2 += static_cast<_int>(m_fRadius / m_fDist);
			z2 += static_cast<_int>(m_fRadius / m_fDist);
			if (pGame_Instance->Get_Input_MouseButton(CInput::EMouseButton::L)) {
				if (pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X)) {
					for (_int i = x; i < x2; ++i) {
						for (_int j = z; j < z2; ++j) {
							if (i < 0 || j < 0 || i > static_cast<_int>(m_iCountZ - 1) || j >  static_cast<_int>(m_iCountX - 1))
								continue;
							_uint _iIndex = i * m_iCountX + j;
							_float fDist = XMVectorGetX(XMVector3Length(
								XMVectorSet(m_pVertexPos[iIndex].x, 0.f, m_pVertexPos[iIndex].z, 1.f) -
								XMVectorSet(m_pVertexPos[_iIndex].x, 0.f, m_pVertexPos[_iIndex].z, 1.f)
							));
							_float fRatio = fDist / m_fRadius;
							if (fRatio <= 1.f) {
								_float fCos = (cosf(XMConvertToRadians(fRatio * 180.f)) + 1) / 2;
								if (true == m_bMode[1])
									Set_Texture(i, j, fCos);


							}
						}
					}
				}
				if (pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::Y)) {
					for (_int i = x; i < x2; ++i) {
						for (_int j = z; j < z2; ++j) {
							if (i < 0 || j < 0 || i > static_cast<_int>(m_iCountZ - 1) || j >  static_cast<_int>(m_iCountX - 1))
								continue;
							_uint _iIndex = i * m_iCountX + j;
							_float fDist = XMVectorGetX(XMVector3Length(
								XMVectorSet(m_pVertexPos[iIndex].x, 0.f, m_pVertexPos[iIndex].z, 1.f) -
								XMVectorSet(m_pVertexPos[_iIndex].x, 0.f, m_pVertexPos[_iIndex].z, 1.f)
							));
							_float fRatio = fDist / m_fRadius;
							if (fRatio <= 1.f) {
								_float fCos = (cosf(XMConvertToRadians(fRatio * 180.f)) + 1) / 2;
								if (true == m_bMode[1])
									Set_Texture(i, j, fCos);

							}
						}
					}
				}
			}
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				for (_int i = x; i < x2; ++i) {
					for (_int j = z; j < z2; ++j) {
						if (i < 0 || j < 0 || i > static_cast<_int>(m_iCountZ - 1) || j >  static_cast<_int>(m_iCountX - 1))
							continue;
						_uint _iIndex = i * m_iCountX + j;
						_float fDist = XMVectorGetX(XMVector3Length(
							XMVectorSet(m_pVertexPos[iIndex].x, 0.f, m_pVertexPos[iIndex].z, 1.f) -
							XMVectorSet(m_pVertexPos[_iIndex].x, 0.f, m_pVertexPos[_iIndex].z, 1.f)
						));
						_float fRatio = fDist / m_fRadius;
						if (fRatio <= 1.f) {
							_float fCos = (cosf(XMConvertToRadians(fRatio * 180.f)) + 1) / 2;
							if (true == m_bMode[0]) {
								if (true == m_bDepth[0])
									Add_Vertex(_iIndex, fCos * m_fDepth);
								if (true == m_bDepth[1])
									Add_Vertex(_iIndex, -fCos * m_fDepth);
								if (true == m_bDepth[2])
									Set_Vertex(_iIndex, m_fDepth);
							}
							if (true == m_bMode[1])
								Set_Texture(i, j, fCos);

						}
					}
				}
			}
		}
	}

	ImGui::Separator();
	ImGui::PopID();
}

const HRESULT CBuffer_Terrain::Create_Terrain(const _int& iCountX, const _int& iCountZ) {
	Safe_Delete_Array(m_pIndex);
	Safe_Delete_Array(m_pVertex);
	Safe_Delete_Array(m_pVertexPos);
	Safe_Release(m_pIndexBuffer);
	Safe_Release(m_pVertexBuffer);
	Safe_Release(m_pShaderResourceView);
	Safe_Release(m_pTexture2D);

	m_iCountX = iCountX;
	m_iCountZ = iCountZ;

	m_iNumBuffer = 1;
	m_iStride = sizeof(VTXNORTEX);

	VTXNORTEX* pVertex = new VTXNORTEX[m_iCountX * m_iCountZ];
	ZeroMemory(pVertex, sizeof(VTXNORTEX) * m_iCountX * m_iCountZ);
	m_pVertexPos = new _float3[m_iCountX * m_iCountZ];
	ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iCountX * m_iCountZ);
	for (_uint i = 0; i < m_iCountZ; ++i)
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iIndex = i * m_iCountX + j;
			m_pVertexPos[iIndex] = pVertex[iIndex].vPos = _float3{ static_cast<_float>(j) * m_fDist, 0.f, static_cast<_float>(i) * m_fDist };
			pVertex[iIndex].vTexCoord = _float2{ j / m_fTexCoord, i / m_fTexCoord };
			pVertex[iIndex].vTexCoord1 = _float2{ j / (m_iCountX - 1.f), i / (m_iCountZ - 1.f) };
		}

	m_iFace = (m_iCountX - 1) * (m_iCountZ - 1) * 2;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexCount = 3 * m_iFace;

	m_pIndex = new IDXFACE32[m_iFace];
	ZeroMemory(m_pIndex, sizeof(IDXFACE32) * m_iFace);
	_uint iFace = 0;
	_vector	vSour, vDest, vNormal;
	for (_uint i = 0; i < m_iCountZ - 1; ++i)
		for (_uint j = 0; j < m_iCountX - 1; ++j) {
			_uint iIndex = i * m_iCountX + j;
			_uint iIndices[] = { iIndex + m_iCountX, iIndex + m_iCountX + 1, iIndex + 1, iIndex };

			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1 = iIndices[0];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2 = iIndices[1];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3 = iIndices[2];
			vSour = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vPos);
			vDest = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal) + vNormal);
			++iFace;

			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1 = iIndices[0];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2 = iIndices[2];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3 = iIndices[3];
			vSour = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vPos);
			vDest = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal) + vNormal);
			++iFace;
		}
	for (_uint i = 0; i < m_iCountX * m_iCountZ; ++i)
		XMStoreFloat3(&pVertex[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertex[i].vNormal)));

	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = m_iStride * m_iCountX * m_iCountZ;
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.StructureByteStride = m_iStride;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Buffer_Desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA SubResource_Data;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = pVertex;
	if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;
	Safe_Delete_Array(pVertex);

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = sizeof(IDXFACE32) * m_iFace;
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Buffer_Desc.StructureByteStride = 0;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Buffer_Desc.MiscFlags = 0;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;
	if (FAILED(__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;

	D3D11_TEXTURE2D_DESC Texture_Desc;
	ZeroMemory(&Texture_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture_Desc.Width = m_iCountX;
	Texture_Desc.Height = m_iCountZ;
	Texture_Desc.MipLevels = 1;
	Texture_Desc.ArraySize = 1;
	Texture_Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	Texture_Desc.SampleDesc.Count = 1;
	Texture_Desc.SampleDesc.Quality = 0;
	Texture_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Texture_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Texture_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Texture_Desc.MiscFlags = 0;
	if (FAILED(m_pDevice->CreateTexture2D(&Texture_Desc, nullptr, &m_pTexture2D)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pShaderResourceView)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iCountZ; ++i) {
		_uint iRow = i * Mapped_SubResource.RowPitch;
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iCol = j * 4;
			_uint iIndex = i * m_iCountX + j;
			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0] = 0;
			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1] = 0;
			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2] = 0;
			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3] = 0;
		}
	}
	m_pContext->Unmap(m_pTexture2D, 0);

	return S_OK;
}

const HRESULT CBuffer_Terrain::Set_Vertex(const _int& iIndex, const _float& iDepth) {
	if (nullptr == m_pContext)
		return E_FAIL;
	D3D11_MAPPED_SUBRESOURCE Mapped_Subresource;
	m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_Subresource);
	m_pVertexPos[iIndex].y = static_cast<VTXNORTEX*>(Mapped_Subresource.pData)[iIndex].vPos.y = iDepth;
	m_pContext->Unmap(m_pVertexBuffer, 0);
	return S_OK;
}

const HRESULT CBuffer_Terrain::Add_Vertex(const _int& iIndex, const _float& iDepth) {
	if (nullptr == m_pContext)
		return E_FAIL;
	D3D11_MAPPED_SUBRESOURCE Mapped_Subresource;
	m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_Subresource);
	m_pVertexPos[iIndex].y = static_cast<VTXNORTEX*>(Mapped_Subresource.pData)[iIndex].vPos.y += iDepth;
	m_pContext->Unmap(m_pVertexBuffer, 0);
	return S_OK;
}

const HRESULT CBuffer_Terrain::Set_Dist(const _float& fDist) {
	m_fDist = fDist;
	if (nullptr == m_pContext)
		return E_FAIL;
	D3D11_MAPPED_SUBRESOURCE Mapped_Subresource;
	m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_Subresource);
	for (_uint i = 0; i < m_iCountZ; ++i) {
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iIndex = i * m_iCountX + j;
			m_pVertexPos[iIndex] = static_cast<VTXNORTEX*>(Mapped_Subresource.pData)[iIndex].vPos = _float3{ static_cast<_float>(j) * m_fDist, m_pVertexPos[iIndex].y, static_cast<_float>(i) * m_fDist };
		}
	}
	m_pContext->Unmap(m_pVertexBuffer, 0);
	return S_OK;
}

const HRESULT CBuffer_Terrain::Set_TexCoord(const _float& fTexCoord) {
	m_fTexCoord = fTexCoord;
	if (nullptr == m_pContext)
		return E_FAIL;
	D3D11_MAPPED_SUBRESOURCE Mapped_Subresource;
	m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_Subresource);
	for (_uint i = 0; i < m_iCountZ; ++i) {
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iIndex = i * m_iCountX + j;
			static_cast<VTXNORTEX*>(Mapped_Subresource.pData)[iIndex].vTexCoord = _float2{ j / m_fTexCoord, i / m_fTexCoord };
		}
	}
	m_pContext->Unmap(m_pVertexBuffer, 0);
	return S_OK;
}

const HRESULT CBuffer_Terrain::Set_Texture(const _int& i, const _int& j, const _float& fDepth) {
	//_int i = iIndex / m_iCountX;
	//_int j = iIndex % m_iCountX;
	_ubyte byColor[4] = { 0, 0, 0, 0 };
	if (true == m_bBrush[1]) byColor[0] = static_cast<_ubyte>(fDepth * 255);
	if (true == m_bBrush[2]) byColor[1] = static_cast<_ubyte>(fDepth * 255);
	if (true == m_bBrush[3]) byColor[2] = static_cast<_ubyte>(fDepth * 255);
	if (true == m_bBrush[4]) byColor[3] = static_cast<_ubyte>(fDepth * 255);

	HRESULT	hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	ZeroMemory(&Mapped_SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_SubResource);

	_uint iRow = i * Mapped_SubResource.RowPitch;
	_uint iCol = j * 4;

	_float fRatio = 1.f - fDepth;
	static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0] = static_cast<_ubyte>(byColor[0] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0] * fRatio);
	static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1] = static_cast<_ubyte>(byColor[1] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1] * fRatio);
	static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2] = static_cast<_ubyte>(byColor[2] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2] * fRatio);
	static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3] = static_cast<_ubyte>(byColor[3] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3] * fRatio);
	m_pContext->Unmap(m_pTexture2D, 0);

	//_uint a = iIndex / m_iCountX;
	//_uint b = iIndex % m_iCountZ;
	//_ubyte byColor[4] = { 0, 0, 0, 0 };
	//if (true == m_bBrush[1]) byColor[0] = static_cast<_ubyte>(fDepth * 255);
	//if (true == m_bBrush[2]) byColor[1] = static_cast<_ubyte>(fDepth * 255);
	//if (true == m_bBrush[3]) byColor[2] = static_cast<_ubyte>(fDepth * 255);
	//if (true == m_bBrush[4]) byColor[3] = static_cast<_ubyte>(fDepth * 255);

	////HRESULT	hr = S_OK;
	////D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	////ZeroMemory(&Mapped_SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	////hr = m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_SubResource);

	//_float fRatio = 1.f - fDepth;

	////m_pContext->Unmap(m_pTexture2D, 0);

	//D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	//m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_SubResource);
	//_uint iCurRow = a * Mapped_SubResource.RowPitch;
	//_uint iCurCol = b * 4;

	//for (_uint i = 0; i < m_iCountZ; ++i) {
	//	_uint iRow = i * Mapped_SubResource.RowPitch;
	//	for (_uint j = 0; j < m_iCountX; ++j) {
	//		_uint iCol = j * 4;
	//		if (iCurRow == iRow && iCurCol == iCol) {
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0] = 0;//static_cast<_ubyte>(byColor[0] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0] * fRatio);
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1] = 255;//static_cast<_ubyte>(byColor[1] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1] * fRatio);
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2] = 0;//static_cast<_ubyte>(byColor[2] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2] * fRatio);
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3] = 255; // static_cast<_ubyte>(byColor[3] + static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3] * fRatio);
	//		}
	//		else {
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0] = 255;//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0];
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1] = 255;//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1];
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2] = 255;//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2];
	//			static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3] = 255;//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3];
	//		}
	//	}
	//}
	//m_pContext->Unmap(m_pTexture2D, 0);
	return S_OK;
}

const HRESULT CBuffer_Terrain::Set_Resource(CShader* const& pShader, const _char* const& pName) {
	if (nullptr == pShader || nullptr == m_pShaderResourceView)
		return E_FAIL;
	return pShader->Set_Resource(pName, m_pShaderResourceView);
}

const _uint CBuffer_Terrain::Find_Index(const _float3& vPos) {
	return static_cast<_uint>(round(vPos.z / m_fDist)) * m_iCountX + static_cast<_uint>(round(vPos.x / m_fDist));
}

void CBuffer_Terrain::Save_Data(Json::Value& root) {
	string str = m_szScene;
	root["Scene"] = str;
	root["Count"]["x"] = m_iCountX;
	root["Count"]["z"] = m_iCountZ;
	root["Dist"] = m_fDist;
	root["TexCoord"] = m_fTexCoord;


	_tchar szFilePath[MAX_PATH];
	wstring wstr{ str.begin(), str.end() };
	ZeroMemory(szFilePath, sizeof(_tchar) * MAX_PATH);
	wsprintfW(szFilePath, TEXT("../Bin/Resource/%s/Terrain/Height.txt"), wstr.c_str());
	ofstream ofs(szFilePath, ios::binary);

	for (_uint i = 0; i < m_iCountZ; ++i) {
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iIndex = i * m_iCountX + j;
			ofs.write(reinterpret_cast<const char*>(&m_pVertexPos[iIndex].y), sizeof(_float));
		}
	}

	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iCountZ; ++i) {
		_uint iRow = i * Mapped_SubResource.RowPitch;
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iCol = j * 4;
			_uint iIndex = i * m_iCountX + j;
			ofs.write(reinterpret_cast<const char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0]), sizeof(_ubyte));
			ofs.write(reinterpret_cast<const char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1]), sizeof(_ubyte));
			ofs.write(reinterpret_cast<const char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2]), sizeof(_ubyte));
			ofs.write(reinterpret_cast<const char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3]), sizeof(_ubyte));
		}
	}
	m_pContext->Unmap(m_pTexture2D, 0);
	ofs.close();
}

void CBuffer_Terrain::Load_Data(Json::Value& root) {
	string str = root["Scene"].asString();
	wstring wstr{ str.begin(), str.end() };
	strcpy_s(m_szScene, str.c_str());
	m_iCountX = root["Count"]["x"].asInt();
	m_iCountZ = root["Count"]["z"].asInt();
	m_fDist = root["Dist"].asFloat();
	m_fTexCoord = root["TexCoord"].asFloat();

	_tchar szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof(_tchar) * MAX_PATH);
	wsprintfW(szFilePath, TEXT("../Bin/Resource/%s/Terrain/Height.txt"), wstr.c_str());
	ifstream ifs(szFilePath, ios::binary);

	m_iNumBuffer = 1;
	m_iStride = sizeof(VTXNORTEX);

	VTXNORTEX* pVertex = new VTXNORTEX[m_iCountX * m_iCountZ];
	ZeroMemory(pVertex, sizeof(VTXNORTEX) * m_iCountX * m_iCountZ);
	m_pVertexPos = new _float3[m_iCountX * m_iCountZ];
	ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iCountX * m_iCountZ);
	for (_uint i = 0; i < m_iCountZ; ++i)
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iIndex = i * m_iCountX + j;
			ifs.read(reinterpret_cast<char*>(&m_pVertexPos[iIndex].y), sizeof(_float));
			m_pVertexPos[iIndex] = pVertex[iIndex].vPos = _float3{ static_cast<_float>(j) * m_fDist, m_pVertexPos[iIndex].y, static_cast<_float>(i) * m_fDist };
			pVertex[iIndex].vTexCoord = _float2{ j / m_fTexCoord, i / m_fTexCoord };
			pVertex[iIndex].vTexCoord1 = _float2{ j / (m_iCountX - 1.f), i / (m_iCountZ - 1.f) };
		}

	m_iFace = (m_iCountX - 1) * (m_iCountZ - 1) * 2;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexCount = 3 * m_iFace;

	m_pIndex = new IDXFACE32[m_iFace];
	ZeroMemory(m_pIndex, sizeof(IDXFACE32) * m_iFace);
	_uint iFace = 0;
	_vector	vSour, vDest, vNormal;
	for (_uint i = 0; i < m_iCountZ - 1; ++i)
		for (_uint j = 0; j < m_iCountX - 1; ++j) {
			_uint iIndex = i * m_iCountX + j;
			_uint iIndices[] = { iIndex + m_iCountX, iIndex + m_iCountX + 1, iIndex + 1, iIndex };

			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1 = iIndices[0];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2 = iIndices[1];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3 = iIndices[2];
			vSour = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vPos);
			vDest = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal) + vNormal);
			++iFace;

			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1 = iIndices[0];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2 = iIndices[2];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3 = iIndices[3];
			vSour = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vPos);
			vDest = XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vPos) - XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vPos);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._2].vNormal) + vNormal);
			XMStoreFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal, XMLoadFloat3(&pVertex[reinterpret_cast<IDXFACE32*>(m_pIndex)[iFace]._3].vNormal) + vNormal);
			++iFace;
		}
	for (_uint i = 0; i < m_iCountX * m_iCountZ; ++i)
		XMStoreFloat3(&pVertex[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertex[i].vNormal)));

	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = m_iStride * m_iCountX * m_iCountZ;
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.StructureByteStride = m_iStride;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Buffer_Desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA SubResource_Data;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = pVertex;
	__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data);
	Safe_Delete_Array(pVertex);

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = sizeof(IDXFACE32) * m_iFace;
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Buffer_Desc.StructureByteStride = 0;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Buffer_Desc.MiscFlags = 0;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;
	__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data);

	D3D11_TEXTURE2D_DESC Texture_Desc;
	ZeroMemory(&Texture_Desc, sizeof(D3D11_TEXTURE2D_DESC));
	Texture_Desc.Width = m_iCountX;
	Texture_Desc.Height = m_iCountZ;
	Texture_Desc.MipLevels = 1;
	Texture_Desc.ArraySize = 1;
	Texture_Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	Texture_Desc.SampleDesc.Count = 1;
	Texture_Desc.SampleDesc.Quality = 0;
	Texture_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Texture_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Texture_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Texture_Desc.MiscFlags = 0;
	m_pDevice->CreateTexture2D(&Texture_Desc, nullptr, &m_pTexture2D);
	m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pShaderResourceView);

	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iCountZ; ++i) {
		_uint iRow = i * Mapped_SubResource.RowPitch;
		for (_uint j = 0; j < m_iCountX; ++j) {
			_uint iCol = j * 4;
			//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0] = 0;
			//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1] = 0;
			//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2] = 255;
			//static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3] = 255;
			ifs.read(reinterpret_cast<char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 0]), sizeof(_byte));
			ifs.read(reinterpret_cast<char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 1]), sizeof(_byte));
			ifs.read(reinterpret_cast<char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 2]), sizeof(_byte));
			ifs.read(reinterpret_cast<char*>(&static_cast<_ubyte*>(Mapped_SubResource.pData)[iRow + iCol + 3]), sizeof(_byte));
		}
	}
	m_pContext->Unmap(m_pTexture2D, 0);

	ifs.close();
}