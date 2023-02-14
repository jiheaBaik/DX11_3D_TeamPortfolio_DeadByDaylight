#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Anim.h"
#include "Shader.h"

CModel* const CModel::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EType& eType, const _uint& iInst, const char* const& pFilePath, const char* pFileName, _fmatrix PivotMatrix) {
	CModel* pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(eType, iInst, pFilePath, pFileName, PivotMatrix)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CModel::Clone(void* const& pArg) {
	CModel* pInstance = new CModel(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CModel::CModel(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
	XMStoreFloat4x4(&m_PivotFloat4x4, XMMatrixIdentity());
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs),
	m_eType(rhs.m_eType),
	m_vecMaterial(rhs.m_vecMaterial),
	m_PivotFloat4x4(rhs.m_PivotFloat4x4),
	m_iInst(rhs.m_iInst) {

	for (auto& iter : rhs.m_vecBone)
		m_vecBone.emplace_back(iter->Clone(&m_vecBone));
	for (auto& iter : rhs.m_vecMesh)
		m_vecMesh.emplace_back(dynamic_cast<CMesh*>(iter->Clone(&m_vecBone)));
	for (auto& iter : m_vecMaterial)
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(iter.pTexture[i]);
	for (auto& iter : rhs.m_vecAnim)
		m_vecAnim.emplace_back(iter->Clone(&m_vecBone));
}

void CModel::Delete(void) {
	__super::Delete();

	for (auto& iter : m_vecAnim)
		Safe_Release(iter);
	m_vecAnim.clear();
	for (auto& iter : m_vecMaterial)
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(iter.pTexture[i]);
	m_vecMaterial.clear();
	for (auto& iter : m_vecMesh)
		Safe_Release(iter);
	m_vecMesh.clear();
	for (auto& iter : m_vecBone)
		Safe_Release(iter);
	m_vecMesh.clear();

	Safe_Release(m_pVertexBufferInst);
	m_Importer.FreeScene();
}

const HRESULT CModel::Init_Create(const EType& eType, const _uint& iInst, const char* const& pFilePath, const char* const& pFileName, _fmatrix PivotMatrix) {
	XMStoreFloat4x4(&m_PivotFloat4x4, PivotMatrix);
	m_eType = eType;
	m_iInst = iInst;
	char szFullPath[MAX_PATH] = "";
	strcpy_s(szFullPath, pFilePath);
	strcat_s(szFullPath, ("data.txt"));
	_tchar szFullPathW[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, szFullPath, -1, szFullPathW, MAX_PATH);

	WIN32_FIND_DATA Find_Data;
	ZeroMemory(&Find_Data, sizeof(WIN32_FIND_DATA));
	HANDLE hFile = FindFirstFile(szFullPathW, &Find_Data);
	if (INVALID_HANDLE_VALUE == hFile) {
		ofstream ofs(szFullPath, ios::binary);

		ZeroMemory(szFullPath, sizeof(char) * MAX_PATH);
		strcpy_s(szFullPath, pFilePath);
		strcat_s(szFullPath, pFileName);

		_uint iFlag = 0;
		switch (eType) {
		case EType::NOANIM:
			iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
			break;
		case EType::ANIM:
			iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
			break;
		}
		m_paiScene = m_Importer.ReadFile(szFullPath, iFlag);
		if (nullptr == m_paiScene)
			return E_FAIL;

		//Bone
		Create_Bone(m_paiScene->mRootNode, nullptr, 0);
		sort(m_vecBone.begin(), m_vecBone.end(), [](CBone* const& pSour, CBone* const& pDest) {
			return pSour->Get_Depth() < pDest->Get_Depth();
			});
		for (_uint i = 0; i < m_vecBone.size(); ++i)
			m_vecBone[i]->Set_Index(i);

		//Mesh
		for (_uint i = 0; i < m_paiScene->mNumMeshes; ++i) {
			CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_iInst, m_paiScene->mMeshes[i], m_vecBone, PivotMatrix);
			if (nullptr == pMesh)
				return E_FAIL;
			m_vecMesh.emplace_back(pMesh);
		}

		//Material
		for (_uint i = 0; i < m_paiScene->mNumMaterials; ++i) {
			SMaterial Material;
			ZeroMemory(&Material, sizeof(SMaterial));

			for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j) {
				aiString strPath;
				if (FAILED(m_paiScene->mMaterials[i]->GetTexture(static_cast<aiTextureType>(j), 0, &strPath)))
					continue;
				char szFileName[MAX_PATH] = "";
				char szExt[MAX_PATH] = "";
				_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

				ZeroMemory(szFullPath, sizeof(char) * MAX_PATH);
				strcpy_s(szFullPath, pFilePath);
				strcat_s(szFullPath, szFileName);
				strcat_s(szFullPath, szExt);

				ZeroMemory(szFullPathW, sizeof(_tchar) * MAX_PATH);
				MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szFullPathW, MAX_PATH);

				memcpy_s(Material.szFilePath[j], sizeof(_tchar) * MAX_PATH, szFullPathW, sizeof(_tchar) * MAX_PATH);
				Material.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPathW, 1);
				if (nullptr == Material.pTexture[j])
					return E_FAIL;
			}
			m_vecMaterial.emplace_back(Material);
		}

		//Anim
		for (_uint i = 0; i < m_paiScene->mNumAnimations; ++i) {
			CAnim* pAnim = CAnim::Create(m_paiScene->mAnimations[i], m_vecBone);
			if (nullptr == pAnim)
				return E_FAIL;
			m_vecAnim.emplace_back(pAnim);
		}

		size_t i = m_vecBone.size();
		ofs.write(reinterpret_cast<const char*>(&i), sizeof(size_t));
		for (auto& iter : m_vecBone)
			iter->Save_Data(ofs);

		i = m_vecMesh.size();
		ofs.write(reinterpret_cast<const char*>(&i), sizeof(size_t));
		for (auto& iter : m_vecMesh)
			iter->Save_Data(ofs, m_eType);

		i = m_vecMaterial.size();
		ofs.write(reinterpret_cast<const char*>(&i), sizeof(size_t));
		for (auto& iter : m_vecMaterial)
			for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
				ofs.write(reinterpret_cast<const char*>(&iter.szFilePath[j]), sizeof(_tchar) * MAX_PATH);

		i = m_vecAnim.size();
		ofs.write(reinterpret_cast<const char*>(&i), sizeof(size_t));
		for (auto& iter : m_vecAnim)
			iter->Save_Data(ofs);
		ofs.close();
	}
	else {
		ifstream ifs(szFullPath, ios::binary);
		size_t iSize = 0;
		ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
		for (_uint i = 0; i < iSize; ++i) {
			CBone* pBone = CBone::Create(ifs, m_vecBone);
			if (nullptr == pBone)
				return E_FAIL;
			m_vecBone.emplace_back(pBone);
		}

		ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
		for (_uint i = 0; i < iSize; ++i) {
			CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_iInst, ifs, m_vecBone);
			if (nullptr == pMesh)
				return E_FAIL;
			m_vecMesh.emplace_back(pMesh);
		}

		ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
		for (_uint i = 0; i < iSize; ++i) {
			SMaterial Material;
			ZeroMemory(&Material, sizeof(SMaterial));

			for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; ++j) {
				_tchar szFilePath[MAX_PATH] = TEXT("");
				ifs.read(reinterpret_cast<char*>(szFilePath), sizeof(_tchar) * MAX_PATH);
				if (!wcscmp(szFilePath, TEXT("")))
					continue;
				memcpy_s(Material.szFilePath[j], sizeof(_tchar) * MAX_PATH, szFilePath, sizeof(_tchar) * MAX_PATH);
				Material.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFilePath, 1);
			}
			m_vecMaterial.emplace_back(Material);
		}

		ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
		for (_uint i = 0; i < iSize; ++i) {
			CAnim* pAnim = CAnim::Create(ifs, m_vecBone);
			if (nullptr == pAnim)
				return E_FAIL;
			m_vecAnim.emplace_back(pAnim);
		}
		ifs.close();
	}
	FindClose(hFile);
	return S_OK;
}

const HRESULT CModel::Init_Clone(void* const& pArg) {
	if (0 != m_iInst) {
		m_iStrideInst = sizeof(VTXMODELINST);
		D3D11_BUFFER_DESC Buffer_Desc;
		ZeroMemory(&Buffer_Desc, sizeof(D3D11_BUFFER_DESC));
		Buffer_Desc.ByteWidth = sizeof(VTXMODELINST) * m_iInst;
		Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
		Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		Buffer_Desc.StructureByteStride = m_iStrideInst;
		Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Buffer_Desc.MiscFlags = 0;

		VTXMODELINST* pVertexInst = new VTXMODELINST[m_iInst];
		ZeroMemory(pVertexInst, sizeof(VTXMODELINST) * m_iInst);

		for (_uint i = 0; i < m_iInst; ++i) {
			pVertexInst[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
			pVertexInst[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
			pVertexInst[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
			pVertexInst[i].vTrans = _float4(0.f, 0.f, 0.f, 1.f);
		}

		D3D11_SUBRESOURCE_DATA SubResource_Data;
		ZeroMemory(&SubResource_Data, sizeof(D3D11_SUBRESOURCE_DATA));
		SubResource_Data.pSysMem = pVertexInst;

		if (FAILED(m_pDevice->CreateBuffer(&Buffer_Desc, &SubResource_Data, &m_pVertexBufferInst)))
			return E_FAIL;
		Safe_Delete_Array(pVertexInst);
	}
	return S_OK;
}

const _uint CModel::Update_Anim(const _double& dTimeDelta, const _bool& bLoop) {
	if (EType::ANIM != m_eType)
		return ANIM_END;
	_uint iEvent = 0;
	iEvent = m_vecAnim[m_iAnim]->Update(dTimeDelta, bLoop);

	m_dLerpTime += dTimeDelta;
	_float fRatio = static_cast<_float>(m_dLerpTime / m_dLerpMaxTime);
	fRatio = fRatio > 1.f ? 1.f : fRatio;
	for (auto& iter : m_vecBone)
		iter->Update(fRatio);
	if (fRatio == 1.f) {
		m_iPreAnim = m_iAnim;
	}

	return iEvent;
}

const HRESULT CModel::Render(const _uint& iMesh, CShader* const& pShader, const _uint& iDesc) {
	if (iMesh >= m_vecMesh.size() || nullptr == m_vecMesh[iMesh])
		return E_FAIL;

	_float4x4 BoneFloat4x4[256];
	m_vecMesh[iMesh]->Set_BoneFloat4x4_Transpose(BoneFloat4x4, XMLoadFloat4x4(&m_PivotFloat4x4));

	if (EType::ANIM == m_eType)
		if (FAILED(pShader->Set_RawValue("g_Bone", BoneFloat4x4, sizeof(_float4x4) * 256)))
			return E_FAIL;

	if (0 == m_iInst)
		dynamic_cast<CBuffer*>(m_vecMesh[iMesh])->Render(pShader, iDesc);
	else {
		switch (m_bMode) {
		case false:
			m_vecMesh[iMesh]->Render(pShader, iDesc, m_pVertexBufferInst, m_iStrideInst, m_iInst, m_iInstCount);
			break;
		case true:
			m_vecMesh[iMesh]->Render(pShader, iDesc, m_pVertexBufferInst, m_iStrideInst, m_iInst, m_iInstX * m_iInstZ);
			break;
		}
	}
	return S_OK;
}

void CModel::Update_Edit(const _int& iPushID) {

	if (m_iInst == 0)
		return;
	ImGui::PushID(iPushID);
	ImGui::Separator();
	ImGui::Text("Model");

	ImGui::Text("Mode");
	if (ImGui::RadioButton("Point", !m_bMode)) m_bMode = false; ImGui::SameLine();
	if (ImGui::RadioButton("Rect", m_bMode)) m_bMode = true;

	switch (m_bMode) {
	case false: {
		ImGui::InputInt("InstCount", (int*)&m_iInstCount);
		ImGui::DragFloat("InstDistX", &m_fInstDistX);
		ImGui::DragFloat("InstDistZ", &m_fInstDistZ);
		if (ImGui::Button("Random")) {
			D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
			m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
			for (_uint i = 0; i < m_iInstCount; ++i) {
				reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vTrans.x = (float)(rand()) / ((float)(RAND_MAX / m_fInstDistX));
				reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vTrans.z = (float)(rand()) / ((float)(RAND_MAX / m_fInstDistZ));
			}
			m_pContext->Unmap(m_pVertexBufferInst, 0);
		}
		if (ImGui::Button("Rotate")) {
			D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
			m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
			for (_uint i = 0; i < m_iInstCount; ++i) {
				_matrix RotMatrix;
				RotMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 1.f), static_cast<_float>((float)(rand()) / ((float)(RAND_MAX / M_PI))));
				_vector vRight = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight);
				_vector vUp = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp);
				_vector vLook = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook);
				XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight, XMVector3TransformNormal(vRight, RotMatrix));
				XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp, XMVector3TransformNormal(vUp, RotMatrix));
				XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook, XMVector3TransformNormal(vLook, RotMatrix));
			}
			m_pContext->Unmap(m_pVertexBufferInst, 0);
		}
		static _float3 minSize{ 1.f,1.f,1.f };
		static _float3 maxSize{ 2.f,2.f,2.f };
		ImGui::DragFloat3("MinScale", (_float*)&minSize);
		ImGui::DragFloat3("MaxScale", (_float*)&maxSize);
		if (ImGui::Button("Scale")) {
			D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
			m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
			for (_uint i = 0; i < m_iInstCount; ++i) {
				_float fScaleX = minSize.x + (float)(rand()) / ((_float)(RAND_MAX / (maxSize.x - minSize.x)));
				_float fScaleY = minSize.y + (float)(rand()) / ((_float)(RAND_MAX / (maxSize.y - minSize.y)));
				_float fScaleZ = minSize.z + (float)(rand()) / ((_float)(RAND_MAX / (maxSize.z - minSize.z)));
				_vector vRight = XMVector3Normalize(XMLoadFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight)) * fScaleX;
				_vector vUp = XMVector3Normalize(XMLoadFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp)) * fScaleY;
				_vector vLook = XMVector3Normalize(XMLoadFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook)) * fScaleZ;
				XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight, vRight);
				XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp, vUp);
				XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook, vLook);
			}
			m_pContext->Unmap(m_pVertexBufferInst, 0);
		}
		break;
	}
	case true: {
		if (ImGui::InputInt("InstX", (int*)&m_iInstX) ||
			ImGui::InputInt("InstZ", (int*)&m_iInstZ) ||
			ImGui::DragFloat("InstDistX", &m_fInstDistX, 0.1f) ||
			ImGui::DragFloat("InstDistZ", &m_fInstDistZ, 0.1f)) {

			D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
			m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
			for (_uint i = 0; i < m_iInstX; ++i) {
				for (_uint j = 0; j < m_iInstZ; ++j) {
					_uint iIndex = i * m_iInstZ + j;

					if (iIndex > m_iInst)
						continue;
					reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vTrans.x = static_cast<_float>(i) * m_fInstDistX;
					reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vTrans.z = static_cast<_float>(j) * m_fInstDistZ;
				}
			}
			m_pContext->Unmap(m_pVertexBufferInst, 0);
		}
		if (ImGui::Button("Rotate")) {
			D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
			m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
			for (_uint i = 0; i < m_iInstX; ++i) {
				for (_uint j = 0; j < m_iInstZ; ++j) {
					_uint iIndex = i * m_iInstZ + j;
					if (iIndex > m_iInst)
						continue;

					_matrix RotMatrix;
					RotMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 1.f), static_cast<_float>((float)(rand()) / ((float)(RAND_MAX / M_PI))));
					_vector vRight = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vRight);
					_vector vUp = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vUp);
					_vector vLook = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vLook);
					XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vRight, XMVector3TransformNormal(vRight, RotMatrix));
					XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vUp, XMVector3TransformNormal(vUp, RotMatrix));
					XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[iIndex].vLook, XMVector3TransformNormal(vLook, RotMatrix));
				}
			}
			m_pContext->Unmap(m_pVertexBufferInst, 0);
		}
		break;
	}
	}

	ImGui::Separator();
	ImGui::PopID();
}

void CModel::Save_Data(Json::Value& root) {
	if (m_iInst == 0)
		return;
	root["Mode"] = m_bMode;
	root["InstCount"] = m_iInstCount;
	root["InstX"] = m_iInstX;
	root["InstZ"] = m_iInstZ;
	root["InstDistX"] = m_fInstDistX;
	root["InstDistZ"] = m_fInstDistZ;

	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iInst; ++i) {
		VTXMODELINST* pVertexInst = reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData);
		root["Inst"][i]["Right"]["x"] = pVertexInst[i].vRight.x;
		root["Inst"][i]["Right"]["y"] = pVertexInst[i].vRight.y;
		root["Inst"][i]["Right"]["z"] = pVertexInst[i].vRight.z;
		root["Inst"][i]["Right"]["w"] = pVertexInst[i].vRight.w;
		root["Inst"][i]["Up"]["x"] = pVertexInst[i].vUp.x;
		root["Inst"][i]["Up"]["y"] = pVertexInst[i].vUp.y;
		root["Inst"][i]["Up"]["z"] = pVertexInst[i].vUp.z;
		root["Inst"][i]["Up"]["w"] = pVertexInst[i].vUp.w;
		root["Inst"][i]["Look"]["x"] = pVertexInst[i].vLook.x;
		root["Inst"][i]["Look"]["y"] = pVertexInst[i].vLook.y;
		root["Inst"][i]["Look"]["z"] = pVertexInst[i].vLook.z;
		root["Inst"][i]["Look"]["w"] = pVertexInst[i].vLook.w;
		root["Inst"][i]["Trans"]["x"] = pVertexInst[i].vTrans.x;
		root["Inst"][i]["Trans"]["y"] = pVertexInst[i].vTrans.y;
		root["Inst"][i]["Trans"]["z"] = pVertexInst[i].vTrans.z;
		root["Inst"][i]["Trans"]["w"] = pVertexInst[i].vTrans.w;
	}
	m_pContext->Unmap(m_pVertexBufferInst, 0);
}

void CModel::Load_Data(Json::Value& root) {
	if (m_iInst == 0)
		return;
	m_bMode = root["Mode"].asBool();
	m_iInstCount = root["InstCount"].asInt();
	m_iInstX = root["InstX"].asInt();
	m_iInstZ = root["InstZ"].asInt();
	m_fInstDistX = root["InstDistX"].asFloat();
	m_fInstDistZ = root["InstDistZ"].asFloat();

	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iInst; ++i) {
		VTXMODELINST* pVertexInst = reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData);
		pVertexInst[i].vRight.x = root["Inst"][i]["Right"]["x"].asFloat();
		pVertexInst[i].vRight.y = root["Inst"][i]["Right"]["y"].asFloat();
		pVertexInst[i].vRight.z = root["Inst"][i]["Right"]["z"].asFloat();
		pVertexInst[i].vRight.w = root["Inst"][i]["Right"]["w"].asFloat();
		pVertexInst[i].vUp.x = root["Inst"][i]["Up"]["x"].asFloat();
		pVertexInst[i].vUp.y = root["Inst"][i]["Up"]["y"].asFloat();
		pVertexInst[i].vUp.z = root["Inst"][i]["Up"]["z"].asFloat();
		pVertexInst[i].vUp.w = root["Inst"][i]["Up"]["w"].asFloat();
		pVertexInst[i].vLook.x = root["Inst"][i]["Look"]["x"].asFloat();
		pVertexInst[i].vLook.y = root["Inst"][i]["Look"]["y"].asFloat();
		pVertexInst[i].vLook.z = root["Inst"][i]["Look"]["z"].asFloat();
		pVertexInst[i].vLook.w = root["Inst"][i]["Look"]["w"].asFloat();
		pVertexInst[i].vTrans.x = root["Inst"][i]["Trans"]["x"].asFloat();
		pVertexInst[i].vTrans.y = root["Inst"][i]["Trans"]["y"].asFloat();
		pVertexInst[i].vTrans.z = root["Inst"][i]["Trans"]["z"].asFloat();
		pVertexInst[i].vTrans.w = root["Inst"][i]["Trans"]["w"].asFloat();
	}
	m_pContext->Unmap(m_pVertexBufferInst, 0);
}

const CModel::EType& CModel::Get_Type(void) const {
	return m_eType;
}

_matrix CModel::Get_PivotMatrix(void) const {
	return XMLoadFloat4x4(&m_PivotFloat4x4);
}

_matrix CModel::Get_BoneMatrix(const char* const& pName) {
	auto iter = find_if(m_vecBone.begin(), m_vecBone.end(), [&](CBone* pTemp) {
		return !strcmp(pName, pTemp->Get_Name());
		});
	if (m_vecBone.end() == iter)
		return _matrix{};
	return (*iter)->Get_CombineMatrix() * XMLoadFloat4x4(&m_PivotFloat4x4);
}

_matrix CModel::Get_RootBoneMatrix(const char* const& pName) {
	auto iter = find_if(m_vecBone.begin(), m_vecBone.end(), [&](CBone* pTemp) {
		return !strcmp(pName, pTemp->Get_Name());
		});
	if (m_vecBone.end() == iter)
		return _matrix{};
	return (*iter)->Get_RootMatrix() * XMLoadFloat4x4(&m_PivotFloat4x4);
}
_matrix CModel::Get_RootBoneMatrix(const _uint& idx) {
	if (idx >= m_vecBone.size())
		return _matrix{};

	return m_vecBone[idx]->Get_RootMatrix() * XMLoadFloat4x4(&m_PivotFloat4x4);
}
vector<class CBone*> const& CModel::Get_Bone(void) const {
	return m_vecBone;
}

const _char* const CModel::Get_bone_Name(const _uint& idx) {
	return m_vecBone[idx]->Get_Name();
}

void CModel::Change_Bone(CModel* const& pModel, vector<const char*> m_vecBoneName) {
	vector<CBone*> pBone = pModel->Get_Bone();
	if (m_vecChangeBone.empty()) {
		for (_uint i = 0; i < m_vecBone.size(); ++i) {
			for (auto j : m_vecBoneName) {
				if (!strcmp(m_vecBone[i]->Get_Name(), j)) {
					m_vecBone[i]->Set_Matrix(pBone[i]->Get_Matrix());
					m_vecChangeBone.emplace_back(i);
				}
			}
		}
	}
	else {
		for (auto i : m_vecChangeBone)
			m_vecBone[i]->Set_Matrix(pBone[i]->Get_Matrix());
	}

	_float fRatio = static_cast<_float>(m_dLerpTime / m_dLerpMaxTime);
	fRatio = fRatio > 1.f ? 1.f : fRatio;
	for (auto& iter : m_vecBone)
		iter->Update(fRatio);
}

vector<class CMesh*> const& CModel::Get_Mesh(void) const {
	return m_vecMesh;
}

const _uint CModel::Get_NumMesh(void) const {
	return static_cast<_uint>(m_vecMesh.size());
}

const _char* const CModel::Get_Anim_Name(void) {
	return m_vecAnim[m_iAnim]->Get_Name();
}

const _char* const CModel::Get_Anim_IdxName(const _uint& idx) {
	return m_vecAnim[idx]->Get_Name();
}

const _uint CModel::Get_AnimCount(void) {
	return static_cast<_uint>(m_vecAnim.size());
}

const _uint CModel::Get_KeyFrame(void) const {
	return m_vecAnim[m_iAnim]->Get_KeyFrame();
}

void CModel::Set_Anim(const _uint& iAnim, const _double& dLerpTime) {
	if (m_vecAnim.size() <= iAnim || m_iAnim == iAnim)
		return;
	for (auto& iter : m_vecBone)
		iter->Set_PerMatrix();
	m_dLerpTime = 0.;
	m_dLerpMaxTime = dLerpTime;

	m_iAnim = iAnim;
	m_vecAnim[m_iAnim]->Set_TimeAcc(0.);
}

void CModel::Set_Anim(const _char* const& pName, const _double& dLerpTime) {
	_uint iAnim = 0;
	for (auto& iter : m_vecAnim) {
		if (!strcmp(iter->Get_Name(), pName)) {
			break;
		}
		iAnim++;
	}
	if (m_vecAnim.size() <= iAnim || m_iAnim == iAnim)
		return;
	for (auto& iter : m_vecBone)
		iter->Set_PerMatrix();
	m_dLerpTime = 0.;
	m_dLerpMaxTime = dLerpTime;

	m_iAnim = iAnim;
	m_vecAnim[m_iAnim]->Set_TimeAcc(0.);
}

void CModel::Set_Anim_TimeAcc(const _double& dTimeAcc) {
	if (EType::ANIM != m_eType)
		return;
	m_vecAnim[m_iAnim]->Set_TimeAcc(dTimeAcc);
}

const _double& CModel::Get_Anim_TimeAcc(void) const {
	return m_vecAnim[m_iAnim]->Get_TimeAcc();
}

const _bool CModel::Get_Anim_KeyTimeAcc(const _uint& iKeyTimeAcc) const {
	return m_vecAnim[m_iAnim]->Get_KeyTimeAcc(iKeyTimeAcc);
}

const _double& CModel::Get_Anim_Duration(void) const {
	return m_vecAnim[m_iAnim]->Get_Duration();
}

const _uint& CModel::Get_AnimNum(void) const {
	return m_iAnim;
}

const _double& CModel::GetPosByKeyframe(vector<_float4>* vecPos, vector<_double>* vecdTime)const {
	m_vecAnim[m_iAnim]->Get_RootPos(vecPos);
	m_vecAnim[m_iAnim]->Get_dTime(vecdTime);
	return m_vecAnim[m_iAnim]->Get_TickPerSecond();
}

_float4x4 CModel::Get_BoneOffsetMatrix(const char* const& pName) {
	auto	iter = find_if(m_vecBone.begin(), m_vecBone.end(), [&](CBone* pBone) {
		return !strcmp(pName, pBone->Get_Name());
		});
	if (m_vecBone.end() == iter)
		return _float4x4{};
	return (*iter)->Get_OffsetFloat4x4();
}

const _uint& CModel::Get_Inst(void) {
	return m_iInst;
}

const _bool CModel::Get_Resource(const _uint& iMesh, const aiTextureType& eTexture, class CShader* const& pShader, const char* const& pName) {
	_uint iMaterial = m_vecMesh[iMesh]->Get_Material();
	if (nullptr == m_vecMaterial[iMaterial].pTexture[eTexture])
		return false;
	return true;
}

const HRESULT CModel::Set_Resource(const _uint& iMesh, const aiTextureType& eTexture, CShader* const& pShader, const char* const& pName) {
	_uint iMaterial = m_vecMesh[iMesh]->Get_Material();
	if (nullptr == m_vecMaterial[iMaterial].pTexture[eTexture])
		return E_FAIL;
	return m_vecMaterial[iMaterial].pTexture[eTexture]->Set_Resource(pShader, pName, 0);
}

void CModel::Set_Inst_Random(const _uint& iInstCount, const _float& fInstDistX, const _float& fInstDistZ) {
	m_iInstCount = iInstCount;
	m_fInstDistX = fInstDistX;
	m_fInstDistZ = fInstDistZ;
	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iInstCount; ++i) {
		reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vTrans.x = (float)(rand()) / ((float)(RAND_MAX / m_fInstDistX));
		reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vTrans.z = (float)(rand()) / ((float)(RAND_MAX / m_fInstDistZ));
	}
	m_pContext->Unmap(m_pVertexBufferInst, 0);
}

void CModel::Set_Inst_Rotate(void) {
	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iInstCount; ++i) {
		_matrix RotMatrix;
		RotMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 1.f), static_cast<_float>((float)(rand()) / ((float)(RAND_MAX / M_PI))));
		_vector vRight = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight);
		_vector vUp = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp);
		_vector vLook = XMLoadFloat4(&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook);
		XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight, XMVector3TransformNormal(vRight, RotMatrix));
		XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp, XMVector3TransformNormal(vUp, RotMatrix));
		XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook, XMVector3TransformNormal(vLook, RotMatrix));
	}
	m_pContext->Unmap(m_pVertexBufferInst, 0);
}

void CModel::Set_Inst_Scale(const _float3& minSize, const _float3& maxSize) {
	D3D11_MAPPED_SUBRESOURCE Mapped_SubResource;
	m_pContext->Map(m_pVertexBufferInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &Mapped_SubResource);
	for (_uint i = 0; i < m_iInstCount; ++i) {
		_float fScaleX = minSize.x + (float)(rand()) / ((_float)(RAND_MAX / (maxSize.x - minSize.x)));
		_float fScaleY = minSize.y + (float)(rand()) / ((_float)(RAND_MAX / (maxSize.y - minSize.y)));
		_float fScaleZ = minSize.z + (float)(rand()) / ((_float)(RAND_MAX / (maxSize.z - minSize.z)));
		_vector vRight = XMVector3Normalize(XMLoadFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight)) * fScaleX;
		_vector vUp = XMVector3Normalize(XMLoadFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp)) * fScaleY;
		_vector vLook = XMVector3Normalize(XMLoadFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook)) * fScaleZ;
		XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vRight, vRight);
		XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vUp, vUp);
		XMStoreFloat3((_float3*)&reinterpret_cast<VTXMODELINST*>(Mapped_SubResource.pData)[i].vLook, vLook);
	}
	m_pContext->Unmap(m_pVertexBufferInst, 0);
}

const HRESULT CModel::Create_Bone(aiNode* const& paiNode, class CBone* const& pParent, _uint iDepth) {
	if (nullptr == paiNode)
		return S_OK;
	_float4x4 Float4x4;
	memcpy(&Float4x4, &paiNode->mTransformation, sizeof(_float4x4));
	CBone* pBone = CBone::Create(paiNode->mName.data, XMLoadFloat4x4(&Float4x4), pParent, iDepth);
	if (nullptr == pBone)
		return E_FAIL;
	m_vecBone.emplace_back(pBone);
	++iDepth;
	for (_uint i = 0; i < paiNode->mNumChildren; ++i)
		Create_Bone(paiNode->mChildren[i], pBone, iDepth);
	return S_OK;
}