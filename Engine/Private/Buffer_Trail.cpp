#include "Buffer_Trail.h"
#include "Shader.h"

CBuffer_Trail* const CBuffer_Trail::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CBuffer_Trail* pInstance = new CBuffer_Trail(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CBuffer_Trail::Clone(void* const& pArg) {
	CBuffer_Trail* pInstance = new CBuffer_Trail(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CBuffer_Trail::CBuffer_Trail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBuffer(pDevice, pContext) {
}

CBuffer_Trail::CBuffer_Trail(const CBuffer_Trail& rhs)
	: CBuffer(rhs) {
}

void CBuffer_Trail::Delete(void) {
	__super::Delete();
	Safe_Delete_Array(m_pIndex);
	Safe_Delete_Array(m_pVertexPos);
	Safe_Release(m_pIndexBuffer);
	Safe_Release(m_pVertexBuffer);
}

const HRESULT CBuffer_Trail::Init_Create(void) {
	return S_OK;
}

const HRESULT CBuffer_Trail::Init_Clone(void* const& pArg) {
	m_iVertexCount = 10;

	m_iNumBuffer = 1;
	m_iStride = sizeof(VTXTEX);
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = m_iStride * m_iVertexCount;
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.StructureByteStride = m_iStride;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Buffer_Desc.MiscFlags = 0;

	VTXTEX* pVertex = new VTXTEX[m_iVertexCount];
	ZeroMemory(pVertex, sizeof(VTXTEX) * m_iVertexCount);
	m_pVertexPos = new _float3[m_iVertexCount];
	ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iVertexCount);
	for (_int i = 0; i < m_iVertexCount; i += 2) {
		m_pVertexPos[i] = pVertex[i].vPos = _float3{ float(i), 0.5f, 0.f };
		pVertex[i].vTexCoord = _float2{ (_float)i / ((_float)m_iVertexCount - 2), 0.f };

		m_pVertexPos[i + 1] = pVertex[i + 1].vPos = _float3{ float(i), -0.5f, 0.f };
		pVertex[i + 1].vTexCoord = _float2{ (_float)i / ((_float)m_iVertexCount - 2), 1.f };
	}

	D3D11_SUBRESOURCE_DATA SubResource_Data;
	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = pVertex;
	if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;
	Safe_Delete_Array(pVertex);

	m_iFace = m_iVertexCount - 2;
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
	for (_uint i = 0; i < m_iFace; i += 2) {
		reinterpret_cast<IDXFACE16*>(m_pIndex)[i]._1 = i + 1;
		reinterpret_cast<IDXFACE16*>(m_pIndex)[i]._2 = i;
		reinterpret_cast<IDXFACE16*>(m_pIndex)[i]._3 = i + 2;

		reinterpret_cast<IDXFACE16*>(m_pIndex)[i + 1]._1 = i + 1;
		reinterpret_cast<IDXFACE16*>(m_pIndex)[i + 1]._2 = i + 2;
		reinterpret_cast<IDXFACE16*>(m_pIndex)[i + 1]._3 = i + 3;
	}

	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;
	if (FAILED(__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;
	return S_OK;
}

const HRESULT CBuffer_Trail::Render1(CShader* const& pShader, const _uint& iPass) {
	if (nullptr == m_pContext)
		return E_FAIL;

	if (FAILED(pShader->Apply(iPass)))
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { m_pVertexBuffer };
	_uint iStrides[] = { m_iStride };
	_uint iOffset[] = { 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumBuffer, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexed(m_iIndex * 3, 0, 0);
	return S_OK;
}

void CBuffer_Trail::Set_Trail(_float3 vPos, _float3 vPos2) {
	if (m_iIndex < m_iVertexCount) {
		m_iIndex += 2;
	}

	D3D11_MAPPED_SUBRESOURCE Mapped_Subresource;
	m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_Subresource);

	//for (_int i = 0; i < m_iIndex; i += 2) {
	//	static_cast<VTXTEX*>(Mapped_Subresource.pData)[i].vTexCoord = { 0.f, (_float)i / ((_float)m_iIndex) };
	//	static_cast<VTXTEX*>(Mapped_Subresource.pData)[i + 1].vTexCoord = { 1.f, (_float)i / ((_float)m_iIndex) };
	//}
	for (_int i = m_iIndex; i >= 2; i -= 2) {
		_float3 tPos;
		tPos.x = static_cast<VTXTEX*>(Mapped_Subresource.pData)[i - 2].vPos.x;
		tPos.y = static_cast<VTXTEX*>(Mapped_Subresource.pData)[i - 2].vPos.y;
		tPos.z = static_cast<VTXTEX*>(Mapped_Subresource.pData)[i - 2].vPos.z;
		static_cast<VTXTEX*>(Mapped_Subresource.pData)[i].vPos = static_cast<VTXTEX*>(Mapped_Subresource.pData)[i - 2].vPos;
		static_cast<VTXTEX*>(Mapped_Subresource.pData)[i + 1].vPos = static_cast<VTXTEX*>(Mapped_Subresource.pData)[i + 1 - 2].vPos;
	}

	m_pVertexPos[0].x = static_cast<VTXTEX*>(Mapped_Subresource.pData)[0].vPos.x = vPos.x;
	m_pVertexPos[0].y = static_cast<VTXTEX*>(Mapped_Subresource.pData)[0].vPos.y = vPos.y;
	m_pVertexPos[0].z = static_cast<VTXTEX*>(Mapped_Subresource.pData)[0].vPos.z = vPos.z;
	m_pVertexPos[1].x = static_cast<VTXTEX*>(Mapped_Subresource.pData)[1].vPos.x = vPos2.x;
	m_pVertexPos[1].y = static_cast<VTXTEX*>(Mapped_Subresource.pData)[1].vPos.y = vPos2.y;
	m_pVertexPos[1].z = static_cast<VTXTEX*>(Mapped_Subresource.pData)[1].vPos.z = vPos2.z;

	m_pContext->Unmap(m_pVertexBuffer, 0);
}