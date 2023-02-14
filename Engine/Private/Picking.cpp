#include "Picking.h"
#include "Camera.h"
#include "Transform.h"
#include "Buffer.h"
#include "Buffer_Terrain.h"
#include "Model.h"
#include "Mesh.h"
#include "Navi.h"
#include "Cell.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking(void)
	: CBase() {
}

void CPicking::Delete(void) {
	Safe_Release(m_pContext);
}

const HRESULT CPicking::Init(const HWND& hWnd, ID3D11DeviceContext* const& pContext) {
	m_hWnd = hWnd;
	m_pContext = pContext;
	Safe_AddRef(m_pContext);
	return S_OK;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CObject* const& pObject, _float* const& pOut) {
	if (nullptr == pCamera || nullptr == pObject)
		return false;
	CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Transform")));
	if (nullptr == pTransform)
		return false;
	CBuffer* pBuffer = dynamic_cast<CBuffer*>(pObject->Get_Component(TEXT("Buffer")));
	if (nullptr != pBuffer)
		return Is_Picking(pCamera, pTransform, pBuffer, pOut);
	else {
		CModel* pModel = dynamic_cast<CModel*>(pObject->Get_Component(TEXT("Model")));
		if (nullptr == pModel)
			return false;
		if (true == Is_Picking(pCamera, pTransform, pModel, pOut))
			return true;
	}
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CObject* const& pObject, _float3* const& pOut) {
	if (nullptr == pCamera || nullptr == pObject)
		return false;
	CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Transform")));
	if (nullptr == pTransform)
		return false;
	CBuffer* pBuffer = dynamic_cast<CBuffer*>(pObject->Get_Component(TEXT("Buffer")));
	if (nullptr != pBuffer)
		return Is_Picking(pCamera, pTransform, pBuffer, pOut);
	CBuffer_Terrain* pTerrain = dynamic_cast<CBuffer_Terrain*>(pObject->Get_Component(TEXT("Terrain")));
	if (nullptr != pTerrain)
		return Is_Picking(pCamera, pTransform, pTerrain, pOut);
	CModel* pModel = dynamic_cast<CModel*>(pObject->Get_Component(TEXT("Model")));
	if (nullptr != pModel)
	{
		if (true == Is_Picking(pCamera, pTransform, pModel, pOut))
			return true;
	}
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CTransform* const& pTransform, CBuffer* const& pBuffer, _float* const& pOut) {
	_float3 vRayDir2, vRayPos2;
	Get_Ray(pCamera, pTransform, &vRayDir2, &vRayPos2);
	_vector vRayDir, vRayPos;
	vRayDir = XMLoadFloat3(&vRayDir2);
	vRayPos = XMLoadFloat3(&vRayPos2);

	const _float3* pVertexPos = pBuffer->Get_VertexPos();
	const void* pIndex = pBuffer->Get_Index();

	_float fDist = 0.f;
	for (_uint i = 0; i < pBuffer->Get_Face(); ++i) {
		_uint iIndices[3];
		ZeroMemory(iIndices, sizeof(_uint) * 3);
		switch (pBuffer->Get_Format()) {
		case DXGI_FORMAT_R16_UINT:
			iIndices[0] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._1;
			iIndices[1] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._2;
			iIndices[2] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._3;
			break;
		case DXGI_FORMAT_R32_UINT:
			iIndices[0] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._1;
			iIndices[1] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._2;
			iIndices[2] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._3;
			break;
		}

		if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pVertexPos[iIndices[0]]), XMLoadFloat3(&pVertexPos[iIndices[1]]), XMLoadFloat3(&pVertexPos[iIndices[2]]), fDist)) {
			if (nullptr != pOut)
				*pOut = fDist;
			return true;
		}
	}
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CTransform* const& pTransform, CBuffer* const& pBuffer, _float3* const& pOut) {
	_float3 vRayDir2, vRayPos2;
	Get_Ray(pCamera, pTransform, &vRayDir2, &vRayPos2);
	_vector vRayDir, vRayPos;
	vRayDir = XMLoadFloat3(&vRayDir2);
	vRayPos = XMLoadFloat3(&vRayPos2);

	const _float3* pVertexPos = pBuffer->Get_VertexPos();
	const void* pIndex = pBuffer->Get_Index();

	_float fDist = 0.f;
	for (_uint i = 0; i < pBuffer->Get_Face(); ++i) {
		_uint iIndices[3];
		ZeroMemory(iIndices, sizeof(_uint) * 3);
		switch (pBuffer->Get_Format()) {
		case DXGI_FORMAT_R16_UINT:
			iIndices[0] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._1;
			iIndices[1] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._2;
			iIndices[2] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._3;
			break;
		case DXGI_FORMAT_R32_UINT:
			iIndices[0] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._1;
			iIndices[1] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._2;
			iIndices[2] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._3;
			break;
		}

		if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pVertexPos[iIndices[0]]), XMLoadFloat3(&pVertexPos[iIndices[1]]), XMLoadFloat3(&pVertexPos[iIndices[2]]), fDist)) {
			if (nullptr != pOut)
				XMStoreFloat3(pOut, XMVector3TransformCoord(vRayPos + vRayDir * fDist, pTransform->Get_Matrix()));
			return true;
		}
	}
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CTransform* const& pTransform, CModel* const& pModel, _float* const& pOut) {
	_float3 vRayDir2, vRayPos2;
	Get_Ray(pCamera, pTransform, &vRayDir2, &vRayPos2);
	_vector vRayDir, vRayPos;
	vRayDir = XMLoadFloat3(&vRayDir2);
	vRayPos = XMLoadFloat3(&vRayPos2);

	_float fDist2 = 9999.f;
	vector<CMesh*> const& vecMesh = pModel->Get_Mesh();
	for (auto& iter : vecMesh) {
		const _float3* pVertexPos = iter->Get_VertexPos();
		const void* pIndex = iter->Get_Index();

		const void* pVertex = iter->Get_Vertex();
		_matrix BoneMatrix[256];
		if (CModel::EType::ANIM == pModel->Get_Type()) {
			_float4x4 BoneFloat4x4[256];
			iter->Set_BoneFloat4x4(BoneFloat4x4, pModel->Get_PivotMatrix());

			for (_uint i = 0; i < 256; ++i)
				BoneMatrix[i] = XMLoadFloat4x4(&BoneFloat4x4[i]);
		}

		_float fDist = 0.f;
		for (_uint i = 0; i < iter->Get_Face(); ++i) {
			_uint iIndices[3];
			ZeroMemory(iIndices, sizeof(_uint) * 3);
			switch (iter->Get_Format()) {
			case DXGI_FORMAT_R16_UINT:
				iIndices[0] = ((IDXFACE16*)pIndex)[i]._1;
				iIndices[1] = ((IDXFACE16*)pIndex)[i]._2;
				iIndices[2] = ((IDXFACE16*)pIndex)[i]._3;
				break;
			case DXGI_FORMAT_R32_UINT:
				iIndices[0] = ((IDXFACE32*)pIndex)[i]._1;
				iIndices[1] = ((IDXFACE32*)pIndex)[i]._2;
				iIndices[2] = ((IDXFACE32*)pIndex)[i]._3;
				break;
			}

			if (CModel::EType::ANIM == pModel->Get_Type()) {
				_vector vPos[3];
				for (_uint j = 0; j < 3; ++j) {
					float fWeightW = 1.f - (reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.x +
						reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.y +
						reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.z);

					_matrix Matrix = BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.x] * reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.x +
						BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.y] * reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.y +
						BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.z] * reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.z +
						BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.w] * fWeightW;

					vPos[j] = XMVector3TransformCoord(XMLoadFloat3(&pVertexPos[iIndices[j]]), Matrix);
				}

				if (true == TriangleTests::Intersects(vRayPos, vRayDir, vPos[0], vPos[1], vPos[2], fDist))
					if (fDist2 > fDist)
						fDist2 = fDist;

			}
			else {
				if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pVertexPos[iIndices[0]]), XMLoadFloat3(&pVertexPos[iIndices[1]]), XMLoadFloat3(&pVertexPos[iIndices[2]]), fDist))
					if (fDist2 > fDist)
						fDist2 = fDist;
			}
		}
	}
	if (fDist2 != 9999.f) {
		if (nullptr != pOut)
			*pOut = fDist2;
		return true;
	}
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CTransform* const& pTransform, CModel* const& pModel, _float3* const& pOut) {
	_float3 vRayDir2, vRayPos2;
	Get_Ray(pCamera, pTransform, &vRayDir2, &vRayPos2);
	_vector vRayDir, vRayPos;
	vRayDir = XMLoadFloat3(&vRayDir2);
	vRayPos = XMLoadFloat3(&vRayPos2);

	_float fDist2 = 9999.f;
	vector<CMesh*> const& vecMesh = pModel->Get_Mesh();
	for (auto& iter : vecMesh) {
		const _float3* pVertexPos = iter->Get_VertexPos();
		const void* pIndex = iter->Get_Index();

		const void* pVertex = iter->Get_Vertex();
		_matrix BoneMatrix[256];
		if (CModel::EType::ANIM == pModel->Get_Type()) {
			_float4x4 BoneFloat4x4[256];
			iter->Set_BoneFloat4x4(BoneFloat4x4, pModel->Get_PivotMatrix());

			for (_uint i = 0; i < 256; ++i)
				BoneMatrix[i] = XMLoadFloat4x4(&BoneFloat4x4[i]);
		}

		_float fDist = 0.f;
		for (_uint i = 0; i < iter->Get_Face(); ++i) {
			_uint iIndices[3];
			ZeroMemory(iIndices, sizeof(_uint) * 3);
			switch (iter->Get_Format()) {
			case DXGI_FORMAT_R16_UINT:
				iIndices[0] = ((IDXFACE16*)pIndex)[i]._1;
				iIndices[1] = ((IDXFACE16*)pIndex)[i]._2;
				iIndices[2] = ((IDXFACE16*)pIndex)[i]._3;
				break;
			case DXGI_FORMAT_R32_UINT:
				iIndices[0] = ((IDXFACE32*)pIndex)[i]._1;
				iIndices[1] = ((IDXFACE32*)pIndex)[i]._2;
				iIndices[2] = ((IDXFACE32*)pIndex)[i]._3;
				break;
			}

			if (CModel::EType::ANIM == pModel->Get_Type()) {
				_vector vPos[3];
				for (_uint j = 0; j < 3; ++j) {
					float fWeightW = 1.f - (reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.x +
						reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.y +
						reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.z);

					_matrix Matrix = BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.x] * reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.x +
						BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.y] * reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.y +
						BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.z] * reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendWeight.z +
						BoneMatrix[reinterpret_cast<const VTXANIMMODEL*>(pVertex)[iIndices[j]].vBlendIndex.w] * fWeightW;

					vPos[j] = XMVector3TransformCoord(XMLoadFloat3(&pVertexPos[iIndices[j]]), Matrix);
				}

				if (true == TriangleTests::Intersects(vRayPos, vRayDir, vPos[0], vPos[1], vPos[2], fDist))
					if (fDist2 > fDist)
						fDist2 = fDist;

			}
			else {
				if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pVertexPos[iIndices[0]]), XMLoadFloat3(&pVertexPos[iIndices[1]]), XMLoadFloat3(&pVertexPos[iIndices[2]]), fDist))
					if (fDist2 > fDist)
						fDist2 = fDist;
			}
		}
	}
	if (fDist2 != 9999.f) {
		if (nullptr != pOut)
			XMStoreFloat3(pOut, XMVector3TransformCoord(vRayPos + vRayDir * fDist2, pTransform->Get_Matrix()));
		return true;
	}
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CBuffer_Terrain* const& pBuffer, _float3* const& pOut) {
	_float3 vRayDir2, vRayPos2;
	Get_Ray(pCamera, XMMatrixIdentity(), &vRayDir2, &vRayPos2);
	_vector vRayDir, vRayPos;
	vRayDir = XMLoadFloat3(&vRayDir2);
	vRayPos = XMLoadFloat3(&vRayPos2);

	const _float3* pVertexPos = pBuffer->Get_VertexPos();
	const void* pIndex = pBuffer->Get_Index();

	_float fDist = 0.f;
	for (_uint i = 0; i < pBuffer->Get_Face(); ++i) {
		_uint iIndices[3];
		ZeroMemory(iIndices, sizeof(_uint) * 3);
		switch (pBuffer->Get_Format()) {
		case DXGI_FORMAT_R16_UINT:
			iIndices[0] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._1;
			iIndices[1] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._2;
			iIndices[2] = reinterpret_cast<const IDXFACE16*>(pIndex)[i]._3;
			break;
		case DXGI_FORMAT_R32_UINT:
			iIndices[0] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._1;
			iIndices[1] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._2;
			iIndices[2] = reinterpret_cast<const IDXFACE32*>(pIndex)[i]._3;
			break;
		}

		if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pVertexPos[iIndices[0]]), XMLoadFloat3(&pVertexPos[iIndices[1]]), XMLoadFloat3(&pVertexPos[iIndices[2]]), fDist)) {
			if (nullptr != pOut)
				XMStoreFloat3(pOut, vRayPos + vRayDir * fDist);
			return true;
		}
	}
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CNavi* const& pNavi, _float3* const& pOut) {
	for (auto& iter : pNavi->Get_Cell())
		if (true == Is_Picking(pCamera, iter, pOut))
			return true;
	return false;
}

const _bool CPicking::Is_Picking(CCamera* const& pCamera, CCell* const& pCell, _float3* const& pOut) {
	_float3 vRayDir2, vRayPos2;
	Get_Ray(pCamera, XMMatrixIdentity(), &vRayDir2, &vRayPos2);
	_vector vRayDir, vRayPos;
	vRayDir = XMLoadFloat3(&vRayDir2);
	vRayPos = XMLoadFloat3(&vRayPos2);

	const _float3* pVertexPos = pCell->Get_Buffer()->Get_VertexPos();

	_float fDist = 0.f;
	if (true == TriangleTests::Intersects(vRayPos, vRayDir, XMLoadFloat3(&pVertexPos[0]), XMLoadFloat3(&pVertexPos[1]), XMLoadFloat3(&pVertexPos[2]), fDist)) {
		if (nullptr != pOut)
			XMStoreFloat3(pOut, vRayPos + vRayDir * fDist);
		return true;
	}
	return false;
}

void CPicking::Get_Ray(CCamera* const& pCamera, class CTransform* const& pTransform, _float3* const& vOutDir, _float3* const& vOutPos) {
	//마우스 커서 받기
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);

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

	//월드행렬 곱하기
	_matrix	WorldMatrixInverse = pTransform->Get_Matrix_Inverse();
	vRayPos = XMVector3TransformCoord(vRayPos, WorldMatrixInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, WorldMatrixInverse);
	vRayDir = XMVector3Normalize(vRayDir);

	XMStoreFloat3(vOutDir, vRayDir);
	XMStoreFloat3(vOutPos, vRayPos);
}

void CPicking::Get_Ray(CCamera* const& pCamera, _fmatrix Matrix, _float3* const& vOutDir, _float3* const& vOutPos) {
	//마우스 커서 받기
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);

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

	//월드행렬 곱하기
	vRayPos = XMVector3TransformCoord(vRayPos, Matrix);
	vRayDir = XMVector3TransformNormal(vRayDir, Matrix);
	vRayDir = XMVector3Normalize(vRayDir);

	XMStoreFloat3(vOutDir, vRayDir);
	XMStoreFloat3(vOutPos, vRayPos);
}
