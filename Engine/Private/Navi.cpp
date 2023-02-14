#include "Navi.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Transform.h"
#include "Target_Mgr.h"

_float CNavi::s_fDivCell;
vector<class CCell*> CNavi::s_vecCell[100][100];

CNavi* const CNavi::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath) {
	CNavi* pInstance = new CNavi(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(pFilePath)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CNavi::Clone(void* const& pArg) {
	CNavi* pInstance = new CNavi(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CNavi::CNavi(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
}

CNavi::CNavi(const CNavi& rhs)
	: CComponent(rhs),
	m_vecCell(rhs.m_vecCell),
	m_pShader(rhs.m_pShader) {
	for (auto& iter : m_vecCell)
		Safe_AddRef(iter);
	Safe_AddRef(m_pShader);
}

void CNavi::Delete(void) {
	__super::Delete();
	Safe_Release(m_pShader);
	for (auto& iter : m_vecCell)
		Safe_Release(iter);
	m_vecCell.clear();
}

const HRESULT CNavi::Init_Create(const _tchar* const& pFilePath) {
	if (nullptr != pFilePath) {
		ifstream ifs(pFilePath, ios::binary);
		size_t iSize = 0;
		ifs.read(reinterpret_cast<char*>(&iSize), sizeof(size_t));
		for (_uint i = 0; i < iSize; ++i) {
			_float3 vPoint[3];
			ifs.read(reinterpret_cast<char*>(vPoint), sizeof(_float3) * 3);
			CCell::EType eType;
			ifs.read(reinterpret_cast<char*>(&eType), sizeof(_ubyte));
			CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, eType, static_cast<_uint>(m_vecCell.size()));
			m_vecCell.emplace_back(pCell);
		}
	}

	for (auto& sour : m_vecCell) {
		if (CCell::EType::FLOOR != sour->Get_Type())
			continue;
		for (auto& dest : m_vecCell) {
			if (sour == dest)
				continue;
			if (CCell::EType::FLOOR != dest->Get_Type())
				continue;
			if (true == dest->Compare_Point(sour->Get_Point(CCell::EPoint::A), sour->Get_Point(CCell::EPoint::B)))
				sour->Set_Neighbor(CCell::ELine::AB, dest->Get_Index());
			if (true == dest->Compare_Point(sour->Get_Point(CCell::EPoint::B), sour->Get_Point(CCell::EPoint::C)))
				sour->Set_Neighbor(CCell::ELine::BC, dest->Get_Index());
			if (true == dest->Compare_Point(sour->Get_Point(CCell::EPoint::C), sour->Get_Point(CCell::EPoint::A)))
				sour->Set_Neighbor(CCell::ELine::CA, dest->Get_Index());
		}
	}

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Cell.hlsl"), VTX_DECLARATION::Input_Element_Desc, VTX_DECLARATION::iNumElement);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

const HRESULT CNavi::Init_Clone(void* const& pArg) {
	if (nullptr != pArg)
		m_iCurIndex = reinterpret_cast<SNavi*>(pArg)->iCurIndex;
	return S_OK;
}

void CNavi::Update_Edit(const _int& iPushID) {
	ImGui::PushID(iPushID);
	ImGui::Separator();
	ImGui::Text("Navi");
	ImGui::InputInt("Index", &m_iCurIndex);
	ImGui::Separator();
	ImGui::PopID();
}

const HRESULT CNavi::Render(void) {
	if (nullptr == m_pShader)
		return E_FAIL;

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	pCamera->Set_RawValue(m_pShader);

	CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Depth"))->Set_Resource(m_pShader, "g_DepthTexture");

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
		return E_FAIL;
	for (auto& iter : m_vecCell)
		if (nullptr != iter)
			iter->Render(m_pShader, 0);

	return S_OK;
}

const _int& CNavi::Get_Index(void) {
	return m_iCurIndex;
}

void CNavi::Set_Index(_int iCurIndex) {
	m_iCurIndex = iCurIndex;
}

const _bool CNavi::Is_Move(_fvector vPos) {
	if (m_iCurIndex >= m_vecCell.size())
		return false;
	if (-1 == m_iCurIndex)
		return true;

	_int iNeighbor = -1;

	if (true == m_vecCell[m_iCurIndex]->Is_In(vPos, iNeighbor))
		return true;
	else {
		if (0 <= iNeighbor) {
			while (true) {
				if (-1 == iNeighbor)
					return false;
				if (true == m_vecCell[iNeighbor]->Is_In(vPos, iNeighbor)) {
					m_iCurIndex = iNeighbor;
					break;
				}
			}
			return true;
		}
		else
			return false;
	}
	return false;
}

void CNavi::Check_Cell(_float3* const& pPoint) {
	_float fDist;
	_float vDist2 = 0.2f;
	_float3 vPoint;

	for (auto& iter : m_vecCell) {
		XMStoreFloat(&fDist, XMVector3Length(XMLoadFloat3(pPoint) - iter->Get_Point(CCell::EPoint::A)));
		if (vDist2 > fDist) {
			XMStoreFloat3(&vPoint, iter->Get_Point(CCell::EPoint::A));
			vDist2 = fDist;
		}
		XMStoreFloat(&fDist, XMVector3Length(XMLoadFloat3(pPoint) - iter->Get_Point(CCell::EPoint::B)));
		if (vDist2 > fDist) {
			XMStoreFloat3(&vPoint, iter->Get_Point(CCell::EPoint::B));
			vDist2 = fDist;
		}
		XMStoreFloat(&fDist, XMVector3Length(XMLoadFloat3(pPoint) - iter->Get_Point(CCell::EPoint::C)));
		if (vDist2 > fDist) {
			XMStoreFloat3(&vPoint, iter->Get_Point(CCell::EPoint::C));
			vDist2 = fDist;
		}
	}

	if (0.2f != vDist2) {
		pPoint->x = vPoint.x;
		pPoint->y = vPoint.y;
		pPoint->z = vPoint.z;
	}
}

const HRESULT CNavi::Create_Cell(_float3* const& pPoint, const CCell::EType& eType) {
	if (CCell::EType::FLOOR == eType) {
		_vector vPoint0 = XMLoadFloat3(&pPoint[0]);
		_vector vPoint1 = XMLoadFloat3(&pPoint[1]);
		_vector vPoint2 = XMLoadFloat3(&pPoint[2]);
		_float3 vCross;
		XMStoreFloat3(&vCross, XMVector3Cross(vPoint0 - vPoint1, vPoint1 - vPoint2));
		if (vCross.y < 0.f) {
			_float fTemp = 0.f;
			fTemp = pPoint[0].x;
			pPoint[0].x = pPoint[2].x;
			pPoint[2].x = fTemp;

			fTemp = pPoint[0].y;
			pPoint[0].y = pPoint[2].y;
			pPoint[2].y = fTemp;

			fTemp = pPoint[0].z;
			pPoint[0].z = pPoint[2].z;
			pPoint[2].z = fTemp;
		};
	}

	if (CCell::EType::CEILING == eType) {
		_vector vPoint0 = XMLoadFloat3(&pPoint[0]);
		_vector vPoint1 = XMLoadFloat3(&pPoint[1]);
		_vector vPoint2 = XMLoadFloat3(&pPoint[2]);
		_float3 vCross;
		XMStoreFloat3(&vCross, XMVector3Cross(vPoint0 - vPoint1, vPoint1 - vPoint2));
		if (vCross.y > 0.f) {
			_float fTemp = 0.f;
			fTemp = pPoint[0].x;
			pPoint[0].x = pPoint[2].x;
			pPoint[2].x = fTemp;

			fTemp = pPoint[0].y;
			pPoint[0].y = pPoint[2].y;
			pPoint[2].y = fTemp;

			fTemp = pPoint[0].z;
			pPoint[0].z = pPoint[2].z;
			pPoint[2].z = fTemp;
		};
	}


	CCell* pCell = CCell::Create(m_pDevice, m_pContext, pPoint, eType, static_cast<_uint>(m_vecCell.size()));
	if (nullptr == pCell)
		return E_FAIL;
	m_vecCell.emplace_back(pCell);
	return S_OK;
}

vector<class CCell*>& CNavi::Get_Cell(void) {
	return m_vecCell;
}

void CNavi::Delete_Cell(CCell* pCell) {
	for (auto iter = m_vecCell.begin(); iter != m_vecCell.end(); ++iter) {
		if (*iter == pCell) {
			Safe_Release((*iter));
			m_vecCell.erase(iter);
			return;
		}
	}
}

_vector CNavi::Get_FloorPos(_fvector vPos) {
	if (-1 == m_iCurIndex)
		return vPos;
	_vector vResult = vPos;
	_float4 vPlane;
	_float3* pPoint = m_vecCell[m_iCurIndex]->Get_Point();
	XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2])));
	vResult = XMVectorSetY(vResult, -((vPlane.x * XMVectorGetX(vPos) + vPlane.z * XMVectorGetZ(vPos) + vPlane.w) / vPlane.y));
	return vResult;
}

void CNavi::Save_Data(Json::Value& root) {
	root["Index"] = m_iCurIndex;
}

void CNavi::Load_Data(Json::Value& root) {
	m_iCurIndex = root["Index"].asInt();
}

void CNavi::Save_Data(ofstream& ofs) {
	size_t iSize = m_vecCell.size();
	ofs.write(reinterpret_cast<const char*>(&iSize), sizeof(size_t));
	for (auto& iter : m_vecCell)
		iter->Save_Data(ofs);
}

void CNavi::Clear_DivCell(void) {
	for (_uint x = 0; x < 100; ++x) {
		for (_uint z = 0; z < 100; ++z) {
			s_vecCell[x][z].clear();
		}
	}
}

void CNavi::Set_DivCell(const _float& fDivCell) {
	Clear_DivCell();

	s_fDivCell = fDivCell;
	for (auto& iter : m_vecCell) {
		_float3* pPoint = iter->Get_Point();
		_uint minX = 100, maxX = 0, minZ = 100, maxZ = 0;
		for (_uint i = 0; i < 3; ++i) {
			_uint x = static_cast<_uint>(pPoint[i].x / s_fDivCell);
			_uint z = static_cast<_uint>(pPoint[i].z / s_fDivCell);
			if (x < minX) minX = x;
			if (x > maxX) maxX = x;
			if (z < minZ) minZ = z;
			if (z > maxZ) maxZ = z;
		}
		for (_uint x = minX; x <= maxX; ++x) {
			for (_uint z = minZ; z <= maxZ; ++z) {
				s_vecCell[x][z].emplace_back(iter);
			}
		}
	}
}

const _bool CNavi::Check_DivCell(const _uint x, const _uint& z, CCell* const pCell) {
	auto iter = find_if(s_vecCell[x][z].begin(), s_vecCell[x][z].end(), [&](CCell* sour) {
		return sour == pCell;
		});
	if (s_vecCell[x][z].end() == iter)
		return false;
	return true;
}

list<class CCell*> CNavi::Get_DivCell(class CTransform* const pTransform) {
	_vector vPos = pTransform->Get_Row(CTransform::ERow::POS);
	_uint x = static_cast<_uint>(XMVectorGetX(vPos) / s_fDivCell);
	_uint z = static_cast<_uint>(XMVectorGetZ(vPos) / s_fDivCell);

	_float fCenX = x * s_fDivCell + (s_fDivCell / 2.f);
	_float fCenZ = z * s_fDivCell + (s_fDivCell / 2.f);

	_float fDistX = XMVectorGetX(vPos) - fCenX;
	_float fDistZ = XMVectorGetZ(vPos) - fCenZ;

	_uint iMinX = x - 1, iMaxX = x + 1, iMinZ = z - 1, iMaxZ = z + 1;
	if (fDistX > 0)
		iMinX = x;
	else 
		iMaxX = x;
	if (fDistZ > 0)
		iMinZ = z;
	else
		iMaxZ = z;

	list<class CCell*> listCell;
	for (_uint i = iMinX; i <= iMaxX; ++i) {
		for (_uint j = iMinZ; j <= iMaxZ; ++j) {
			listCell.insert(listCell.end(), s_vecCell[i][j].begin(), s_vecCell[i][j].end());
		}
	}
	return listCell;
}

vector<class CCell*> CNavi::Get_DivCell(const _float3& vSour, const _float3& vDest) {
	_uint iSourX = static_cast<_uint>(vSour.x / s_fDivCell);
	_uint iSourZ = static_cast<_uint>(vSour.z / s_fDivCell);
	_uint iDestX = static_cast<_uint>(vDest.x / s_fDivCell);
	_uint iDestZ = static_cast<_uint>(vDest.x / s_fDivCell);

	_uint iMinX = 100, iMaxX = 0, iMinZ = 100, iMaxZ = 0;

	if (iMinX > iSourX) iMinX = iSourX;
	if (iMinX > iDestX) iMinX = iDestX;
	if (iMaxX < iSourX) iMaxX = iSourX;
	if (iMaxX < iDestX) iMaxX = iDestX;
	if (iMinZ > iSourZ) iMinZ = iSourZ;
	if (iMinZ > iDestZ) iMinZ = iDestZ;
	if (iMaxZ < iSourZ) iMaxZ = iSourZ;
	if (iMaxZ < iDestZ) iMaxZ = iDestZ;

	vector<class CCell*> vecCell;
	for (_uint x = iMinX; x <= iMaxX; ++x) {
		for (_uint z = iMinZ; z <= iMaxZ; ++z) {
			vecCell.insert(vecCell.end(), s_vecCell[x][z].begin(), s_vecCell[x][z].end());
		}
	}

	return vecCell;
}