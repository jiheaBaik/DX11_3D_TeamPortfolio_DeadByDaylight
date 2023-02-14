#include "Cell.h"
#include "Triangle.h"
#include "Shader.h"

CCell* const CCell::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, _float3* const& pPoint, const EType& eType, const _uint& iIndex) {
	CCell* pInstance = new CCell(pDevice, pContext);
	if (FAILED(pInstance->Init(pPoint, eType, iIndex)))
		Safe_Release(pInstance);
	return pInstance;
}

CCell::CCell(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CCell::Delete(void) {
	Safe_Release(m_pBuffer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CCell::Init(_float3* const& pPoint, const EType& eType, const _uint& iIndex) {
	m_eType = eType;
	m_iIndex = iIndex;
	m_vPoint[static_cast<_ubyte>(EPoint::A)] = pPoint[static_cast<_ubyte>(EPoint::A)];
	m_vPoint[static_cast<_ubyte>(EPoint::B)] = pPoint[static_cast<_ubyte>(EPoint::B)];
	m_vPoint[static_cast<_ubyte>(EPoint::C)] = pPoint[static_cast<_ubyte>(EPoint::C)];

	XMStoreFloat3(&m_vLine[static_cast<_ubyte>(ELine::AB)],
		XMVector3Normalize(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::B)]) - XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::A)])));
	XMStoreFloat3(&m_vLine[static_cast<_ubyte>(ELine::BC)],
		XMVector3Normalize(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::C)]) - XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::B)])));
	XMStoreFloat3(&m_vLine[static_cast<_ubyte>(ELine::CA)],
		XMVector3Normalize(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::A)]) - XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::C)])));

	m_vNormal[static_cast<_ubyte>(ELine::AB)]
		= _float3{ -m_vLine[static_cast<_ubyte>(ELine::AB)].z , 0.f , m_vLine[static_cast<_ubyte>(ELine::AB)].x };
	m_vNormal[static_cast<_ubyte>(ELine::BC)]
		= _float3{ -m_vLine[static_cast<_ubyte>(ELine::BC)].z , 0.f , m_vLine[static_cast<_ubyte>(ELine::BC)].x };
	m_vNormal[static_cast<_ubyte>(ELine::CA)]
		= _float3{ -m_vLine[static_cast<_ubyte>(ELine::CA)].z , 0.f , m_vLine[static_cast<_ubyte>(ELine::CA)].x };

	m_pBuffer = CTriangle::Create(m_pDevice, m_pContext, pPoint);
	if (nullptr == m_pBuffer)
		return E_FAIL;

	_vector vPointA = XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::A)]);
	_vector vPointB = XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::B)]);
	_vector vPointC = XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::C)]);
	XMStoreFloat3(&m_vCross, XMVector3Cross(XMVector3Normalize(vPointA - vPointB), XMVector3Normalize(vPointB - vPointC)));

	return S_OK;
}

const HRESULT CCell::Render(CShader* const& pShader, const _uint& iPass) {
	if (nullptr == m_pBuffer)
		return E_FAIL;
	switch (m_eType) {
	case EType::FLOOR:
		if (FAILED(pShader->Set_RawValue("g_vColor", &_float4{ 0.f, 1.f, 1.f, 1.f }, sizeof(_float4))))
			return E_FAIL;
		break;
	case EType::WALL:
		if (FAILED(pShader->Set_RawValue("g_vColor", &_float4{ 0.f, 1.f, 0.f, 1.f }, sizeof(_float4))))
			return E_FAIL;
		break;
	case EType::CEILING:
		if (FAILED(pShader->Set_RawValue("g_vColor", &_float4{ 1.f, 0.f, 0.f, 1.f }, sizeof(_float4))))
			return E_FAIL;
		break;
	case EType::NOCAMERAWALL:
		if (FAILED(pShader->Set_RawValue("g_vColor", &_float4{ 0.f, 0.f, 1.f, 1.f }, sizeof(_float4))))
			return E_FAIL;
		break;
	}
	pShader->Apply(iPass);
	return m_pBuffer->Render();
}

const _uint& CCell::Get_Index(void) const {
	return m_iIndex;
}

_vector CCell::Get_Point(const EPoint& ePoint) const {
	return XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(ePoint)]);
}

_float3* CCell::Get_Point(void) {
	return m_vPoint;
}

const _bool CCell::Compare_Point(_fvector vSour, _fvector vDest) {
	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::A)]), vSour)) {
		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::B)]), vDest))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::C)]), vDest))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::B)]), vSour)) {
		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::A)]), vDest))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::C)]), vDest))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::C)]), vSour)) {
		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::A)]), vDest))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoint[static_cast<_ubyte>(EPoint::B)]), vDest))
			return true;
	}
	return false;
}

void CCell::Set_Neighbor(const ELine& eLine, const _uint& iIndex) {
	m_iNeighbor[static_cast<_ubyte>(eLine)] = iIndex;
}

const _bool CCell::Is_In(_fvector vPos, _int& iNeighbor) {
	for (_ubyte i = 0; i < static_cast<_ubyte>(ELine::END); ++i) {
		_vector	vDir = XMVector3Normalize(vPos - XMLoadFloat3(&m_vPoint[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormal[i])))) {
			iNeighbor = m_iNeighbor[i];
			return false;
		}
	}
	return true;
}

CBuffer* const CCell::Get_Buffer(void) {
	return m_pBuffer;
}

void CCell::Set_Type(const EType& eType) {
	m_eType = eType;
}

const CCell::EType& CCell::Get_Type(void) const {
	return m_eType;
}

_vector CCell::Get_Cross(void) {
	return XMVector3Normalize(XMLoadFloat3(&m_vCross));
}

void CCell::Set_Point(const _float3* const& vPoint) {
	memcpy(m_vPoint, vPoint, sizeof(_float3) * 3);

	m_pBuffer->Set_Vertex(m_vPoint);
}

void CCell::Save_Data(ofstream& ofs) {
	ofs.write(reinterpret_cast<const char*>(m_vPoint), sizeof(_float3) * 3);
	ofs.write(reinterpret_cast<const char*>(&m_eType), sizeof(_ubyte));
}
