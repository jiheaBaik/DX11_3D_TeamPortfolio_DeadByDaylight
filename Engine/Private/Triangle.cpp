#include "Triangle.h"
#include "Shader.h"

CTriangle* const CTriangle::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _float3* const& pPoint) {
	CTriangle* pInstance = new CTriangle(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(pPoint)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CTriangle::Clone(void* const& pArg) {
	CTriangle* pInstance = new CTriangle(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CTriangle::CTriangle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBuffer(pDevice, pContext) {
}

CTriangle::CTriangle(const CTriangle& rhs)
	: CBuffer(rhs) {
}

void CTriangle::Delete(void) {
	__super::Delete();
}

const HRESULT CTriangle::Init_Create(const _float3* const& pPoint) {
	m_iNumBuffer = 1;
	m_iStride = sizeof(VTX);

	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = m_iStride * 3;
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.StructureByteStride = m_iStride;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Buffer_Desc.MiscFlags = 0;

	VTX* pVertex = new VTX[3];
	ZeroMemory(pVertex, sizeof(VTX) * 3);
	m_pVertexPos = new _float3[3];
	ZeroMemory(m_pVertexPos, sizeof(_float3) * 3);
	m_pVertexPos[0] = pVertex[0].vPos = pPoint[0];
	m_pVertexPos[1] = pVertex[1].vPos = pPoint[1];
	m_pVertexPos[2] = pVertex[2].vPos = pPoint[2];

	D3D11_SUBRESOURCE_DATA SubResource_Data;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = pVertex;
	if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;
	Safe_Delete_Array(pVertex);

	m_iFace = 4;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_iIndexCount =  m_iFace;

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = sizeof(_ushort) * m_iFace;
	Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Buffer_Desc.StructureByteStride = 0;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;

	m_pIndex = new _ushort[m_iFace];
	ZeroMemory(m_pIndex, sizeof(_ushort) * m_iFace);
	reinterpret_cast<_ushort*>(m_pIndex)[0] = 0;
	reinterpret_cast<_ushort*>(m_pIndex)[1] = 1;
	reinterpret_cast<_ushort*>(m_pIndex)[2] = 2;
	reinterpret_cast<_ushort*>(m_pIndex)[3] = 0;

	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;
	if (FAILED(__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;

	return S_OK;
}

const HRESULT CTriangle::Init_Clone(void* const& pArg) {
	return S_OK;
}

void CTriangle::Set_Vertex(const _float3* const& vPoint) {
	if (nullptr == m_pContext)
		return;

	D3D11_MAPPED_SUBRESOURCE Mapped_Subresource;
	m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_Subresource);
	for (_uint i = 0; i < 3; ++i) {
		((VTX*)Mapped_Subresource.pData)[i].vPos.x = vPoint[i].x;
		((VTX*)Mapped_Subresource.pData)[i].vPos.y = vPoint[i].y;
		((VTX*)Mapped_Subresource.pData)[i].vPos.z = vPoint[i].z;
	}
	m_pContext->Unmap(m_pVertexBuffer, 0);
}
