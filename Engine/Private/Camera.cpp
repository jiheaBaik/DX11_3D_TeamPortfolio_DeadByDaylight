#include "Camera.h"
#include "Transform.h"
#include "Shader.h"

vector<class CCamera*> CCamera::s_vecCamera;
map<wstring, class CCamera*> CCamera::s_mapCamera;

CCamera::CCamera(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
	XMStoreFloat4x4(&m_ViewFloat4x4, XMMatrixIdentity());
}

CCamera::CCamera(const CCamera& rhs)
	: CObject(rhs),
	m_ViewFloat4x4(rhs.m_ViewFloat4x4),
	m_ProjFloat4x4(rhs.m_ViewFloat4x4) {
}

void CCamera::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	for (auto pair = s_mapCamera.begin(); pair != s_mapCamera.end(); ++pair)
		if (this == pair->second) {
			s_mapCamera.erase(pair);
			break;
		}
	for (auto pair = s_vecCamera.begin(); pair != s_vecCamera.end(); ++pair)
		if (this == *pair) {
			s_vecCamera.erase(pair);
			break;
		}
}

const HRESULT CCamera::Init_Clone(void* const& pArg) {
	SDesc sDesc;
	if (nullptr != pArg) {
		memcpy(&sDesc, pArg, sizeof(SDesc));
		s_vecCamera.emplace_back(this);
		if (nullptr != sDesc.pKey)
			s_mapCamera.emplace(sDesc.pKey, this);
	}

	XMStoreFloat4x4(&m_ProjFloat4x4, XMMatrixPerspectiveFovLH(sDesc.fFovy, sDesc.fAspect, sDesc.fNear, sDesc.fFar));

	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	m_mapComponent.emplace(TEXT("Transform"), m_pTransform);
	Safe_AddRef(m_pTransform);

	return S_OK;
}

_vector CCamera::Get_Pos(void) const {
	return m_pTransform->Get_Row(CTransform::ERow::POS);
}

_matrix CCamera::Get_ViewMatrix(void) const {
	return XMLoadFloat4x4(&m_ViewFloat4x4);
}

_matrix CCamera::Get_ViewMatrix_Transpose(void) const {
	return XMMatrixTranspose(XMLoadFloat4x4(&m_ViewFloat4x4));
}

_matrix CCamera::Get_ViewMatrix_Inverse(void) const {
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewFloat4x4));
}

_matrix CCamera::Get_ProjMatrix(void) const {
	return XMLoadFloat4x4(&m_ProjFloat4x4);
}

_matrix CCamera::Get_ProjMatrix_Transpose(void) const {
	return XMMatrixTranspose(XMLoadFloat4x4(&m_ProjFloat4x4));
}

_matrix CCamera::Get_ProjMatrix_Inverse(void) const {
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ProjFloat4x4));
}

void CCamera::Set_OldViewMatrix(void) {
	m_OldViewFloat4x4 = m_ViewFloat4x4;
}

void CCamera::Set_ViewMatrix(void) {
	XMStoreFloat4x4(&m_ViewFloat4x4, m_pTransform->Get_Matrix_Inverse());
}

const HRESULT CCamera::Set_RawValue(CShader* const& pShader) {
	//pShader->Set_RawValue("g_OldViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldViewFloat4x4)), sizeof(_float4x4));
	pShader->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ViewFloat4x4)), sizeof(_float4x4));
	pShader->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjFloat4x4)), sizeof(_float4x4));
	return S_OK;
}

void CCamera::Set_Camera(const _uint& iCamera) {
	for (auto iter = s_vecCamera.begin(); iter != s_vecCamera.end(); ++iter)
		if (*iter == this) {
			s_vecCamera.erase(iter);
			break;
		}
	auto iter = s_vecCamera.begin();
	iter += iCamera;
	s_vecCamera.insert(iter, this);
}

CCamera* const CCamera::Get_Camera(const _uint& iCamera) {
	if (iCamera >= s_vecCamera.size())
		return nullptr;
	return s_vecCamera[iCamera];
}

CCamera* const CCamera::Get_Camera(const _tchar* const& pCameraKey) {
	auto iter = find_if(s_mapCamera.begin(), s_mapCamera.end(), [&](map<wstring, class CCamera*>::value_type pair) {
		return pair.first == pCameraKey;
		});
	if (s_mapCamera.end() == iter)
		return nullptr;
	return iter->second;
}