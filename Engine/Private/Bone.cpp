#include "Bone.h"

CBone* const CBone::Create(const char* const& pName, _fmatrix Matrix, CBone* const& pParent, const _uint& iDepth) {
	CBone* pInstance = new CBone();
	if (FAILED(pInstance->Init_Create(pName, Matrix, pParent, iDepth)))
		Safe_Release(pInstance);
	return pInstance;
}

CBone* const CBone::Create(ifstream& ifs, const vector<class CBone*>& pVecBone) {
	CBone* pInstance = new CBone();
	if (FAILED(pInstance->Init_Create(ifs, pVecBone)))
		Safe_Release(pInstance);
	return pInstance;
}

CBone* const CBone::Clone(void* const& pArg) {
	CBone* pInstance = new CBone(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CBone::CBone(void)
	:CBase() {
	XMStoreFloat4x4(&m_Float4x4, XMMatrixIdentity());
	XMStoreFloat4x4(&m_PerFloat4x4, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombineFloat4x4, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetFloat4x4, XMMatrixIdentity());
}

CBone::CBone(const CBone& rhs)
	: CBase(),
	m_iIndex(rhs.m_iIndex),
	m_Float4x4(rhs.m_Float4x4),
	m_PerFloat4x4(rhs.m_PerFloat4x4),
	m_CombineFloat4x4(rhs.m_CombineFloat4x4),
	m_OffsetFloat4x4(rhs.m_OffsetFloat4x4),
	m_pParent(rhs.m_pParent) {
	strcpy_s(m_szName, rhs.m_szName);
	Safe_AddRef(m_pParent);
}

void CBone::Delete(void) {
	Safe_Release(m_pParent);
}

const HRESULT CBone::Init_Create(const char* const& pName, _fmatrix Matrix, CBone* const& pParent, const _uint& iDepth) {
	strcpy_s(m_szName, pName);
	XMStoreFloat4x4(&m_Float4x4, XMMatrixTranspose(Matrix));
	m_pParent = pParent;
	Safe_AddRef(m_pParent);
	m_iDepth = iDepth;
	return S_OK;
}

const HRESULT CBone::Init_Create(ifstream& ifs, const vector<class CBone*>& pVecBone) {
	ifs.read((char*)m_szName, sizeof(char) * MAX_PATH);
	ifs.read((char*)&m_iIndex, sizeof(_uint));
	ifs.read((char*)&m_Float4x4, sizeof(_float4x4));
	ifs.read((char*)&m_OffsetFloat4x4, sizeof(_float4x4));
	_int iIndex = -1;
	ifs.read((char*)&iIndex, sizeof(_int));
	if (-1 != iIndex) {
		m_pParent = pVecBone[iIndex];
		Safe_AddRef(m_pParent);
	}
	return S_OK;
}

const HRESULT CBone::Init_Clone(void* const& pArg) {
	if (nullptr != m_pParent) {
		CBone* pParent = (*reinterpret_cast<vector<class CBone*>*>(pArg))[m_pParent->Get_Index()];
		Safe_Release(m_pParent);
		m_pParent = pParent;
		Safe_AddRef(m_pParent);
	}
	return S_OK;
}

void CBone::Update(const _float& fRatio) {
	if (fRatio < 1.f) {
		_vector vPos = XMLoadFloat3((_float3*)(&m_Float4x4.m[3][0]));
		_vector vPerPos = XMLoadFloat3((_float3*)(&m_PerFloat4x4.m[3][0]));
		_vector vScale = XMVectorSet(XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)(&m_Float4x4.m[0][0])))),
			XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)(&m_Float4x4.m[1][0])))),
			XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)(&m_Float4x4.m[2][0])))), 0.f);
		_vector vPerScale = XMVectorSet(XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)(&m_PerFloat4x4.m[0][0])))),
			XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)(&m_PerFloat4x4.m[1][0])))),
			XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)(&m_PerFloat4x4.m[2][0])))), 0.f);
		_float4x4 Float4x4, PreFloat4x4;
		XMStoreFloat3((_float3*)(&Float4x4.m[0][0]), XMVector3Normalize(XMLoadFloat3((_float3*)(&m_Float4x4.m[0][0]))));
		XMStoreFloat3((_float3*)(&Float4x4.m[1][0]), XMVector3Normalize(XMLoadFloat3((_float3*)(&m_Float4x4.m[1][0]))));
		XMStoreFloat3((_float3*)(&Float4x4.m[2][0]), XMVector3Normalize(XMLoadFloat3((_float3*)(&m_Float4x4.m[2][0]))));
		XMStoreFloat3((_float3*)(&PreFloat4x4.m[0][0]), XMVector3Normalize(XMLoadFloat3((_float3*)(&m_PerFloat4x4.m[0][0]))));
		XMStoreFloat3((_float3*)(&PreFloat4x4.m[1][0]), XMVector3Normalize(XMLoadFloat3((_float3*)(&m_PerFloat4x4.m[1][0]))));
		XMStoreFloat3((_float3*)(&PreFloat4x4.m[2][0]), XMVector3Normalize(XMLoadFloat3((_float3*)(&m_PerFloat4x4.m[2][0]))));
		_vector vRot = XMQuaternionRotationMatrix(XMLoadFloat4x4(&Float4x4));
		_vector vPerRot = XMQuaternionRotationMatrix(XMLoadFloat4x4(&PreFloat4x4));

		_vector vResultPos = XMVectorLerp(vPerPos, vPos, fRatio);
		_matrix vResultMatrix = XMMatrixRotationQuaternion(XMQuaternionSlerp(vPerRot, vRot, fRatio));
		_vector vResultScale = XMVectorLerp(vPerScale, vScale, fRatio);

		XMStoreFloat4x4(&m_Float4x4, vResultMatrix);
		_vector vResultRight = XMLoadFloat3((_float3*)(&m_Float4x4.m[0][0])) * XMVectorGetX(vResultScale);
		_vector vResultUp = XMLoadFloat3((_float3*)(&m_Float4x4.m[1][0])) * XMVectorGetY(vResultScale);
		_vector vResultLook = XMLoadFloat3((_float3*)(&m_Float4x4.m[2][0])) * XMVectorGetZ(vResultScale);
		XMStoreFloat3((_float3*)&m_Float4x4.m[0][0], vResultRight);
		XMStoreFloat3((_float3*)&m_Float4x4.m[1][0], vResultUp);
		XMStoreFloat3((_float3*)&m_Float4x4.m[2][0], vResultLook);
		XMStoreFloat3((_float3*)(&m_Float4x4.m[3][0]), vResultPos);
	}

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombineFloat4x4, XMLoadFloat4x4(&m_Float4x4) * XMLoadFloat4x4(&m_pParent->m_CombineFloat4x4));
	else 
		m_CombineFloat4x4 = m_Float4x4;
	
	if (!strcmp(m_szName, "joint_Char")) {
		m_RootFloat4x4 = m_CombineFloat4x4;
		_matrix CombineMatrix = XMLoadFloat4x4(&m_CombineFloat4x4);
		CombineMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		XMStoreFloat4x4(&m_CombineFloat4x4, CombineMatrix);
	}
}

const _char* const CBone::Get_Name(void) const {
	return m_szName;
}

const _uint& CBone::Get_Index(void) const {
	return m_iIndex;
}

const _uint& CBone::Get_Depth(void) const {
	return m_iDepth;
}

_matrix CBone::Get_CombineMatrix(void) const {
	return XMLoadFloat4x4(&m_CombineFloat4x4);
}

_matrix CBone::Get_OffsetMatrix(void) const {
	return XMLoadFloat4x4(&m_OffsetFloat4x4);
}

const _float4x4& CBone::Get_OffsetFloat4x4(void) {
	return m_OffsetFloat4x4;
}

_matrix CBone::Get_RootMatrix(void){
	return XMLoadFloat4x4(&m_RootFloat4x4);
}

void CBone::Set_Index(const _uint& iIndex) {
	m_iIndex = iIndex;
}

_matrix CBone::Get_Matrix(void) {
	return XMLoadFloat4x4(&m_Float4x4);
}

void CBone::Set_Matrix(_fmatrix Matrix) {
	XMStoreFloat4x4(&m_Float4x4, Matrix);
}

void CBone::Set_OffsetMatrix(_fmatrix OffsetMatrix) {
	XMStoreFloat4x4(&m_OffsetFloat4x4, OffsetMatrix);
}

void CBone::Set_PerMatrix() {
	m_PerFloat4x4 = m_Float4x4;
}

void CBone::Save_Data(ofstream& ofs) {
	ofs.write((char*)m_szName, sizeof(char) * MAX_PATH);
	ofs.write((char*)&m_iIndex, sizeof(_uint));
	ofs.write((char*)&m_Float4x4, sizeof(_float4x4));
	ofs.write((char*)&m_OffsetFloat4x4, sizeof(_float4x4));
	_int iIndex = -1;
	if (nullptr != m_pParent)
		iIndex = m_pParent->Get_Index();
	ofs.write((char*)&iIndex, sizeof(_int));
}