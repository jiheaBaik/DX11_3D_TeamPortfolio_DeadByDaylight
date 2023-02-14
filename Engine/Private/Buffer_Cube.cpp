#include "Buffer_Cube.h"

CBuffer_Cube* const CBuffer_Cube::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CBuffer_Cube* pInstance = new CBuffer_Cube(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CBuffer_Cube::Clone(void* const& pArg) {
	CBuffer_Cube* pInstance = new CBuffer_Cube(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CBuffer_Cube::CBuffer_Cube(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBuffer(pDevice, pContext) {
}

CBuffer_Cube::CBuffer_Cube(const CBuffer_Cube& rhs)
	: CBuffer(rhs) {
}

void CBuffer_Cube::Delete(void) {
	__super::Delete();
}

const HRESULT CBuffer_Cube::Init_Create(void) {
	m_iNumBuffer = 1;
	m_iStride = sizeof(VTXCUBETEX);

	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = m_iStride * 8;
	Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.StructureByteStride = m_iStride;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;

	VTXCUBETEX* pVertex = new VTXCUBETEX[8];
	ZeroMemory(pVertex, sizeof(VTXCUBETEX) * 8);
	m_pVertexPos = new _float3[8];
	ZeroMemory(m_pVertexPos, sizeof(_float3) * 8);

	m_pVertexPos[0] = pVertex[0].vPos = _float3{ -0.5f, 0.5f, -0.5f };
	pVertex[0].vTexCoord = pVertex[0].vPos;

	m_pVertexPos[1] = pVertex[1].vPos = _float3{ 0.5f, 0.5f, -0.5f };
	pVertex[1].vTexCoord = pVertex[1].vPos;

	m_pVertexPos[2] = pVertex[2].vPos = _float3{ 0.5f, -0.5f, -0.5f };
	pVertex[2].vTexCoord = pVertex[2].vPos;

	m_pVertexPos[3] = pVertex[3].vPos = _float3{ -0.5f, -0.5f, -0.5f };
	pVertex[3].vTexCoord = pVertex[3].vPos;

	m_pVertexPos[4] = pVertex[4].vPos = _float3{ -0.5f, 0.5f, 0.5f };
	pVertex[4].vTexCoord = pVertex[4].vPos;

	m_pVertexPos[5] = pVertex[5].vPos = _float3{ 0.5f, 0.5f, 0.5f };
	pVertex[5].vTexCoord = pVertex[5].vPos;

	m_pVertexPos[6] = pVertex[6].vPos = _float3{ 0.5f, -0.5f, 0.5f };
	pVertex[6].vTexCoord = pVertex[6].vPos;

	m_pVertexPos[7] = pVertex[7].vPos = _float3{ -0.5f, -0.5f, 0.5f };
	pVertex[7].vTexCoord = pVertex[7].vPos;

	D3D11_SUBRESOURCE_DATA SubResource_Data;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = pVertex;

	if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;
	Safe_Delete_Array(pVertex);

	m_iFace = 12;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexCount = 3 * m_iFace;

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = sizeof(IDXFACE16) * m_iFace;
	Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Buffer_Desc.StructureByteStride = 0;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;

	m_pIndex = new IDXFACE16[m_iFace];
	ZeroMemory(m_pIndex, sizeof(IDXFACE16) * m_iFace);

	reinterpret_cast<IDXFACE16*>(m_pIndex)[0]._1 = 1; reinterpret_cast<IDXFACE16*>(m_pIndex)[0]._2 = 5; reinterpret_cast<IDXFACE16*>(m_pIndex)[0]._3 = 6;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[1]._1 = 1; reinterpret_cast<IDXFACE16*>(m_pIndex)[1]._2 = 6; reinterpret_cast<IDXFACE16*>(m_pIndex)[1]._3 = 2;

	reinterpret_cast<IDXFACE16*>(m_pIndex)[2]._1 = 4; reinterpret_cast<IDXFACE16*>(m_pIndex)[2]._2 = 0; reinterpret_cast<IDXFACE16*>(m_pIndex)[2]._3 = 3;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[3]._1 = 4; reinterpret_cast<IDXFACE16*>(m_pIndex)[3]._2 = 3; reinterpret_cast<IDXFACE16*>(m_pIndex)[3]._3 = 7;

	reinterpret_cast<IDXFACE16*>(m_pIndex)[4]._1 = 4; reinterpret_cast<IDXFACE16*>(m_pIndex)[4]._2 = 5; reinterpret_cast<IDXFACE16*>(m_pIndex)[4]._3 = 1;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[5]._1 = 4; reinterpret_cast<IDXFACE16*>(m_pIndex)[5]._2 = 1; reinterpret_cast<IDXFACE16*>(m_pIndex)[5]._3 = 0;

	reinterpret_cast<IDXFACE16*>(m_pIndex)[6]._1 = 3; reinterpret_cast<IDXFACE16*>(m_pIndex)[6]._2 = 2; reinterpret_cast<IDXFACE16*>(m_pIndex)[6]._3 = 6;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[7]._1 = 3; reinterpret_cast<IDXFACE16*>(m_pIndex)[7]._2 = 6; reinterpret_cast<IDXFACE16*>(m_pIndex)[7]._3 = 7;

	reinterpret_cast<IDXFACE16*>(m_pIndex)[8]._1 = 5; reinterpret_cast<IDXFACE16*>(m_pIndex)[8]._2 = 4; reinterpret_cast<IDXFACE16*>(m_pIndex)[8]._3 = 7;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[9]._1 = 5; reinterpret_cast<IDXFACE16*>(m_pIndex)[9]._2 = 7; reinterpret_cast<IDXFACE16*>(m_pIndex)[9]._3 = 6;

	reinterpret_cast<IDXFACE16*>(m_pIndex)[10]._1 = 0; reinterpret_cast<IDXFACE16*>(m_pIndex)[10]._2 = 1; reinterpret_cast<IDXFACE16*>(m_pIndex)[10]._3 = 2;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[11]._1 = 0; reinterpret_cast<IDXFACE16*>(m_pIndex)[11]._2 = 2; reinterpret_cast<IDXFACE16*>(m_pIndex)[11]._3 = 3;

	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;

	if (FAILED(__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;

	return S_OK;
}

const HRESULT CBuffer_Cube::Init_Clone(void* const& pArg) {
	return S_OK;
}