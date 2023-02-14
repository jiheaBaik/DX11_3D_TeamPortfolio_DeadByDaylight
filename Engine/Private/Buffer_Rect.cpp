#include "Buffer_Rect.h"

CBuffer_Rect* const CBuffer_Rect::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CBuffer_Rect* pInstance = new CBuffer_Rect(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CBuffer_Rect::Clone(void* const& pArg) {
	CBuffer_Rect* pInstance = new CBuffer_Rect(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CBuffer_Rect::CBuffer_Rect(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBuffer(pDevice, pContext) {
}

CBuffer_Rect::CBuffer_Rect(const CBuffer_Rect& rhs)
	: CBuffer(rhs) {
}

void CBuffer_Rect::Delete(void) {
	__super::Delete();
}

const HRESULT CBuffer_Rect::Init_Create(void) {
	m_iNumBuffer = 1;
	m_iStride = sizeof(VTXTEX);
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = m_iStride * 4;
	Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.StructureByteStride = m_iStride;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;

	VTXTEX* pVertex = new VTXTEX[4];
	ZeroMemory(pVertex, sizeof(VTXTEX) * 4);
	m_pVertexPos = new _float3[4];
	ZeroMemory(m_pVertexPos, sizeof(_float3) * 4);
	m_pVertexPos[0] = pVertex[0].vPos = _float3{ -0.5f, 0.5f, 0.f };
	pVertex[0].vTexCoord = _float2{ 0.f, 0.f };
	m_pVertexPos[1] = pVertex[1].vPos = _float3{ 0.5f, 0.5f, 0.f };
	pVertex[1].vTexCoord = _float2{ 1.f, 0.f };
	m_pVertexPos[2] = pVertex[2].vPos = _float3{ 0.5f, -0.5f, 0.f };
	pVertex[2].vTexCoord = _float2{ 1.f, 1.f };
	m_pVertexPos[3] = pVertex[3].vPos = _float3{ -0.5f, -0.5f, 0.f };
	pVertex[3].vTexCoord = _float2{ 0.f, 1.f };

	D3D11_SUBRESOURCE_DATA SubResource_Data;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = pVertex;
	if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;
	Safe_Delete_Array(pVertex);

	m_iFace = 2;
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
	reinterpret_cast<IDXFACE16*>(m_pIndex)[0]._1 = 0;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[0]._2 = 1;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[0]._3 = 2;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[1]._1 = 0;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[1]._2 = 2;
	reinterpret_cast<IDXFACE16*>(m_pIndex)[1]._3 = 3;

	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;
	if (FAILED(__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;
	return S_OK;
}

const HRESULT CBuffer_Rect::Init_Clone(void* const& pArg) {
	return S_OK;
}
