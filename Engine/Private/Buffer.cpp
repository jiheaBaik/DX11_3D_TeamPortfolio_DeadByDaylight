#include "Buffer.h"
#include "Shader.h"

CBuffer::CBuffer(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
}

CBuffer::CBuffer(const CBuffer& rhs)
	: CComponent(rhs),
	m_pVertexBuffer(rhs.m_pVertexBuffer),
	m_iNumBuffer(rhs.m_iNumBuffer),
	m_iStride(rhs.m_iStride),
	m_pIndexBuffer(rhs.m_pIndexBuffer), 
	m_eFormat(rhs.m_eFormat),
	m_eTopology(rhs.m_eTopology),
	m_iIndexCount(rhs.m_iIndexCount),
	m_pVertexPos(rhs.m_pVertexPos),
	m_pVertex(rhs.m_pVertex),
	m_pIndex(rhs.m_pIndex),
	m_iFace(rhs.m_iFace) {
	Safe_AddRef(m_pVertexBuffer);
	Safe_AddRef(m_pIndexBuffer);
}

void CBuffer::Delete(void) {
	__super::Delete();
	if (false == m_bClone) {
		Safe_Delete_Array(m_pIndex);
		Safe_Delete_Array(m_pVertex);
		Safe_Delete_Array(m_pVertexPos);
	}
	Safe_Release(m_pIndexBuffer);
	Safe_Release(m_pVertexBuffer);
}

const HRESULT CBuffer::Render(void) {
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { m_pVertexBuffer };
	_uint iStrides[] = { m_iStride };
	_uint iOffset[] = { 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumBuffer, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iIndexCount, 0, 0);
	return S_OK;
}

const HRESULT CBuffer::Render(CShader* const& pShader, const _uint& iPass) {
	if (nullptr == m_pContext)
		return E_FAIL;

	if(FAILED(pShader->Apply(iPass)))
		return E_FAIL;
	
	ID3D11Buffer* pVertexBuffers[] = { m_pVertexBuffer };
	_uint iStrides[] = { m_iStride };
	_uint iOffset[] = { 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumBuffer, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexed(m_iIndexCount, 0, 0);
	return S_OK;
}

const HRESULT CBuffer::Create_VertexBuffer(const D3D11_BUFFER_DESC& Buffer_Desc, const D3D11_SUBRESOURCE_DATA& SubResource_Data) {
	if (nullptr == m_pDevice)
		return E_FAIL;
	if (FAILED(m_pDevice->CreateBuffer(&Buffer_Desc, &SubResource_Data, &m_pVertexBuffer)))
		return E_FAIL;
	return S_OK;
}

const HRESULT CBuffer::Create_IndexBuffer(const D3D11_BUFFER_DESC& Buffer_Desc, const D3D11_SUBRESOURCE_DATA& SubResource_Data) {
	if (nullptr == m_pDevice)
		return E_FAIL;
	if (FAILED(m_pDevice->CreateBuffer(&Buffer_Desc, &SubResource_Data, &m_pIndexBuffer)))
		return E_FAIL;
	return S_OK;
}