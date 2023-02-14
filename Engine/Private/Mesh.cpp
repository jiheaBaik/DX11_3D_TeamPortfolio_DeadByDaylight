#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"

CMesh* const CMesh::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const CModel::EType& eType, const _uint& iInst, aiMesh* const& paiMesh, const vector<class CBone*>& pVecBone, _fmatrix PivotMatrix) {
	CMesh* pInstance = new CMesh(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(eType, iInst, paiMesh, pVecBone, PivotMatrix)))
		Safe_Release(pInstance);
	return pInstance;
}

CMesh* const CMesh::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const CModel::EType& eType, const _uint& iInst, ifstream& ifs, const vector<class CBone*>& pVecBone) {
	CMesh* pInstance = new CMesh(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(eType, iInst, ifs, pVecBone)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CMesh::Clone(void* const& pArg) {
	CMesh* pInstance = new CMesh(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CMesh::CMesh(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBuffer(pDevice, pContext) {
}

CMesh::CMesh(const CMesh& rhs)
	: CBuffer(rhs),
	m_vecBone(rhs.m_vecBone),
	m_iMaterial(rhs.m_iMaterial) {
	strcpy_s(m_szName, rhs.m_szName);
	for (auto& iter : m_vecBone)
		Safe_AddRef(iter);
}

void CMesh::Delete(void) {
	__super::Delete();
	for (auto& iter : m_vecBone)
		Safe_Release(iter);

}

const HRESULT CMesh::Init_Create(const CModel::EType& eType, const _uint& iInst, aiMesh* const& paiMesh, const vector<class CBone*>& pVecBone, _fmatrix PivotMatrix) {
	strcpy_s(m_szName, paiMesh->mName.data);

	D3D11_BUFFER_DESC Buffer_Desc;
	D3D11_SUBRESOURCE_DATA SubResource_Data;
	if (0 == iInst)
		m_iNumBuffer = 1;
	else
		m_iNumBuffer = 2;
	m_iVertex = paiMesh->mNumVertices;

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;

	switch (eType) {
	case CModel::EType::NOANIM: {
		m_iStride = sizeof(VTXMODEL);

		Buffer_Desc.ByteWidth = m_iStride * m_iVertex;
		Buffer_Desc.StructureByteStride = m_iStride;

		m_pVertex = new VTXMODEL[m_iVertex];
		ZeroMemory(m_pVertex, sizeof(VTXMODEL) * m_iVertex);
		m_pVertexPos = new _float3[m_iVertex];
		ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iVertex);

		for (_uint i = 0; i < m_iVertex; ++i) {
			memcpy(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vPos, &paiMesh->mVertices[i], sizeof(_float3));
			XMStoreFloat3(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vPos, XMVector3TransformCoord(XMLoadFloat3(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vPos), PivotMatrix));
			m_pVertexPos[i] = reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vPos;
			memcpy(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vNormal, &paiMesh->mNormals[i], sizeof(_float3));
			XMStoreFloat3(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vNormal), PivotMatrix)));
			memcpy(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vTexCoord, &paiMesh->mTextureCoords[0][i], sizeof(_float2));
			memcpy(&reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vTangent, &paiMesh->mTangents[i], sizeof(_float3));
		}

		ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
		SubResource_Data.pSysMem = m_pVertex;

		if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
			return E_FAIL;
	}
							  break;
	case CModel::EType::ANIM: {
		m_iStride = sizeof(VTXANIMMODEL);

		Buffer_Desc.ByteWidth = m_iStride * m_iVertex;
		Buffer_Desc.StructureByteStride = m_iStride;

		m_pVertex = new VTXANIMMODEL[m_iVertex];
		ZeroMemory(m_pVertex, sizeof(VTXANIMMODEL) * m_iVertex);
		m_pVertexPos = new _float3[m_iVertex];
		ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iVertex);

		for (_uint i = 0; i < m_iVertex; ++i) {
			memcpy(&reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[i].vPos, &paiMesh->mVertices[i], sizeof(_float3));
			m_pVertexPos[i] = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[i].vPos;
			memcpy(&reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[i].vNormal, &paiMesh->mNormals[i], sizeof(_float3));
			memcpy(&reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[i].vTexCoord, &paiMesh->mTextureCoords[0][i], sizeof(_float2));
			memcpy(&reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[i].vTangent, &paiMesh->mTangents[i], sizeof(_float3));
		}

		for (_uint i = 0; i < paiMesh->mNumBones; ++i) {
			aiBone* paiBone = paiMesh->mBones[i];

			auto iter = find_if(pVecBone.begin(), pVecBone.end(), [&](CBone* pTemp) {
				return !strcmp(paiBone->mName.data, pTemp->Get_Name());
				});
			if (pVecBone.end() == iter)
				return E_FAIL;
			_float4x4 OffsetFloat4x4;
			memcpy(&OffsetFloat4x4, &paiBone->mOffsetMatrix, sizeof(_float4x4));
			(*iter)->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetFloat4x4)));
			m_vecBone.emplace_back(*iter);
			Safe_AddRef(*iter);

			for (_uint j = 0; j < paiBone->mNumWeights; ++j) {
				if (0.f == reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.x) {
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.x = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.x = i;
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.x = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.x = paiBone->mWeights[j].mWeight;
				}
				else if (0.f == reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.y) {
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.y = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.y = i;
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.y = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.y = paiBone->mWeights[j].mWeight;
				}
				else if (0.f == reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.z) {
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.z = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.z = i;
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.z = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.z = paiBone->mWeights[j].mWeight;
				}
				else if (0.f == reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.w) {
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.w = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendIndex.w = i;
					reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.w = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[paiBone->mWeights[j].mVertexId].vBlendWeight.w = paiBone->mWeights[j].mWeight;
				}
			}
		}
		if (0 == paiMesh->mNumBones) {
			auto iter = find_if(pVecBone.begin(), pVecBone.end(), [&](CBone* pTemp) {
				return !strcmp(m_szName, pTemp->Get_Name());
				});
			if (pVecBone.end() != iter) {
				m_vecBone.emplace_back(*iter);
				Safe_AddRef(*iter);
			}
		}

		ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
		SubResource_Data.pSysMem = m_pVertex;

		if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
			return E_FAIL;
	}
							break;
	}

	if (0 == iInst)
		m_iFace = paiMesh->mNumFaces;
	else
		m_iFace = paiMesh->mNumFaces * iInst;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexCount = 3 * m_iFace;

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = sizeof(IDXFACE32) * m_iFace;
	Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Buffer_Desc.StructureByteStride = 0;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;

	m_pIndex = new IDXFACE32[m_iFace];
	ZeroMemory(m_pIndex, sizeof(IDXFACE32) * m_iFace);

	for (_uint i = 0; i < m_iFace; ++i) {
		if (0 == iInst)
		{
			reinterpret_cast<IDXFACE32*>(m_pIndex)[i]._1 = paiMesh->mFaces[i].mIndices[0];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[i]._2 = paiMesh->mFaces[i].mIndices[1];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[i]._3 = paiMesh->mFaces[i].mIndices[2];
		}
		else {
			reinterpret_cast<IDXFACE32*>(m_pIndex)[i]._1 = paiMesh->mFaces[i % (m_iFace / iInst)].mIndices[0];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[i]._2 = paiMesh->mFaces[i % (m_iFace / iInst)].mIndices[1];
			reinterpret_cast<IDXFACE32*>(m_pIndex)[i]._3 = paiMesh->mFaces[i % (m_iFace / iInst)].mIndices[2];
		}
	}

	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;

	if (FAILED(__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;

	m_iMaterial = paiMesh->mMaterialIndex;

	return S_OK;
}

const HRESULT CMesh::Init_Create(const CModel::EType& eType, const _uint& iInst, ifstream& ifs, const vector<class CBone*>& pVecBone) {
	ifs.read(reinterpret_cast<_char*>(m_szName), sizeof(_char) * MAX_PATH);

	D3D11_BUFFER_DESC Buffer_Desc;
	D3D11_SUBRESOURCE_DATA SubResource_Data;
	if (0 == iInst)
		m_iNumBuffer = 1;
	else
		m_iNumBuffer = 2;

	ifs.read(reinterpret_cast<_char*>(&m_iVertex), sizeof(_uint));
	switch (eType) {
	case CModel::EType::NOANIM: {
		m_iStride = sizeof(VTXMODEL);

		ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
		Buffer_Desc.ByteWidth = m_iStride * m_iVertex;
		Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
		Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		Buffer_Desc.StructureByteStride = m_iStride;
		Buffer_Desc.CPUAccessFlags = 0;
		Buffer_Desc.MiscFlags = 0;

		m_pVertex = new VTXMODEL[m_iVertex];
		ZeroMemory(m_pVertex, sizeof(VTXMODEL) * m_iVertex);
		m_pVertexPos = new _float3[m_iVertex];
		ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iVertex);

		ifs.read(reinterpret_cast<char*>(m_pVertex), sizeof(VTXMODEL) * m_iVertex);
		for (_uint i = 0; i < m_iVertex; ++i)
			m_pVertexPos[i] = reinterpret_cast<VTXMODEL*>(m_pVertex)[i].vPos;

		ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
		SubResource_Data.pSysMem = m_pVertex;

		if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
			return E_FAIL;
	}
							  break;
	case CModel::EType::ANIM: {
		m_iStride = sizeof(VTXANIMMODEL);

		ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
		Buffer_Desc.ByteWidth = m_iStride * m_iVertex;
		Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
		Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		Buffer_Desc.StructureByteStride = m_iStride;
		Buffer_Desc.CPUAccessFlags = 0;
		Buffer_Desc.MiscFlags = 0;

		m_pVertex = new VTXANIMMODEL[m_iVertex];
		ZeroMemory(m_pVertex, sizeof(VTXANIMMODEL) * m_iVertex);
		m_pVertexPos = new _float3[m_iVertex];
		ZeroMemory(m_pVertexPos, sizeof(_float3) * m_iVertex);

		ifs.read(reinterpret_cast<char*>(m_pVertex), sizeof(VTXANIMMODEL) * m_iVertex);
		for (_uint i = 0; i < m_iVertex; ++i)
			m_pVertexPos[i] = reinterpret_cast<VTXANIMMODEL*>(m_pVertex)[i].vPos;

		size_t iSize = 0;
		ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
		for (_uint i = 0; i < iSize; ++i) {
			_uint iIndex = 0;
			ifs.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
			m_vecBone.emplace_back(pVecBone[iIndex]);
			Safe_AddRef(pVecBone[iIndex]);
		}

		ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
		SubResource_Data.pSysMem = m_pVertex;

		if (FAILED(__super::Create_VertexBuffer(Buffer_Desc, SubResource_Data)))
			return E_FAIL;
	}
							break;
	}

	ifs.read(reinterpret_cast<char*>(&m_iFace), sizeof(_uint));

	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexCount = 3 * m_iFace;

	ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
	Buffer_Desc.ByteWidth = sizeof(IDXFACE32) * m_iFace;
	Buffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Buffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Buffer_Desc.StructureByteStride = 0;
	Buffer_Desc.CPUAccessFlags = 0;
	Buffer_Desc.MiscFlags = 0;

	m_pIndex = new IDXFACE32[m_iFace];
	ZeroMemory(m_pIndex, sizeof(IDXFACE32) * m_iFace);

	ifs.read(reinterpret_cast<_char*>(m_pIndex), sizeof(IDXFACE32) * m_iFace);

	ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
	SubResource_Data.pSysMem = m_pIndex;

	if (FAILED(__super::Create_IndexBuffer(Buffer_Desc, SubResource_Data)))
		return E_FAIL;

	ifs.read(reinterpret_cast<_char*>(&m_iMaterial), sizeof(_uint));
	return S_OK;
}

const HRESULT CMesh::Init_Clone(void* const& pArg) {
	vector<CBone*> vecBone;
	for (auto& iter : m_vecBone) {
		CBone* pBone = (*reinterpret_cast<vector<class CBone*>*>(pArg))[iter->Get_Index()];
		Safe_Release(iter);
		vecBone.emplace_back(pBone);
		Safe_AddRef(pBone);
	}
	m_vecBone.clear();
	m_vecBone = vecBone;
	return S_OK;
}

const HRESULT CMesh::Render(CShader* const& pShader, const _uint& iPass, ID3D11Buffer* const& pVertexBufferInst, const _uint& iStrideInst, const _uint& iInst, const _uint& iInstCount) {
	if (nullptr == m_pContext)
		return E_FAIL;

	if (FAILED(pShader->Apply(iPass)))
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { m_pVertexBuffer, pVertexBufferInst };
	_uint iStrides[] = { m_iStride, iStrideInst };
	_uint iOffset[] = { 0, 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumBuffer, pVertexBuffers, iStrides, iOffset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, m_eFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iIndexCount / iInst, iInstCount, 0, 0, 0);
	return S_OK;
}

const _uint& CMesh::Get_Material(void) const {
	return m_iMaterial;
}

void CMesh::Set_BoneFloat4x4(_float4x4* const& pBoneFloat4x4, _fmatrix PivotMatrix) {
	_uint iBone = 0;

	if (true == m_vecBone.empty()) {
		XMStoreFloat4x4(&pBoneFloat4x4[0], XMMatrixIdentity());
		return;
	}
	for (auto& iter : m_vecBone)
		XMStoreFloat4x4(&pBoneFloat4x4[iBone++], iter->Get_OffsetMatrix() * iter->Get_CombineMatrix() * PivotMatrix);
}

void CMesh::Set_BoneFloat4x4_Transpose(_float4x4* const& pBoneFloat4x4, _fmatrix PivotMatrix) {
	_uint iBone = 0;

	if (true == m_vecBone.empty()) {
		XMStoreFloat4x4(&pBoneFloat4x4[0], XMMatrixIdentity());
		return;
	}
	for (auto& iter : m_vecBone)
		XMStoreFloat4x4(&pBoneFloat4x4[iBone++], XMMatrixTranspose(iter->Get_OffsetMatrix() * iter->Get_CombineMatrix() * PivotMatrix));
}

void CMesh::Save_Data(ofstream& ofs, const CModel::EType& eType) {
	ofs.write(reinterpret_cast<const _char*>(m_szName), sizeof(_char) * MAX_PATH);
	ofs.write(reinterpret_cast<const _char*>(&m_iVertex), sizeof(_uint));
	switch (eType) {
	case CModel::EType::NOANIM:
		ofs.write(reinterpret_cast<const _char*>(m_pVertex), sizeof(VTXMODEL) * m_iVertex);
		break;
	case CModel::EType::ANIM:
		ofs.write(reinterpret_cast<const _char*>(m_pVertex), sizeof(VTXANIMMODEL) * m_iVertex);
		size_t iSize = m_vecBone.size();
		ofs.write(reinterpret_cast<const _char*>(&iSize), sizeof(size_t));
		for (auto& iter : m_vecBone)
			ofs.write(reinterpret_cast<const _char*>(&iter->Get_Index()), sizeof(_uint));
		break;
	}
	ofs.write(reinterpret_cast<const _char*>(&m_iFace), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(m_pIndex), sizeof(IDXFACE32) * m_iFace);
	ofs.write(reinterpret_cast<const _char*>(&m_iMaterial), sizeof(_uint));
}