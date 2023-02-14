#include "Frustum.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum(void)
	: CBase() {
}

void CFrustum::Delete(void) {
}

const HRESULT CFrustum::Init(void) {
	m_vOrigin_Point[0] = _float3{ -1.f, 1.f, 0.f };
	m_vOrigin_Point[1] = _float3{ 1.f, 1.f, 0.f };
	m_vOrigin_Point[2] = _float3{ 1.f, -1.f, 0.f };
	m_vOrigin_Point[3] = _float3{ -1.f, -1.f, 0.f };

	m_vOrigin_Point[4] = _float3{ -1.f, 1.f, 1.f };
	m_vOrigin_Point[5] = _float3{ 1.f, 1.f, 1.f };
	m_vOrigin_Point[6] = _float3{ 1.f, -1.f, 1.f };
	m_vOrigin_Point[7] = _float3{ -1.f, -1.f, 1.f };
	return S_OK;
}

void CFrustum::Transform_ToWorldSpace(void) {
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera)
	{
		_matrix ViewMatrix = pCamera->Get_ViewMatrix_Inverse();
		_matrix ProjMatrix = pCamera->Get_ProjMatrix_Inverse();

		for (_uint i = 0; i < 8; ++i) {
			XMStoreFloat3(&m_vWorld_Point[i], XMVector3TransformCoord(XMLoadFloat3(&m_vOrigin_Point[i]), ProjMatrix));
			XMStoreFloat3(&m_vWorld_Point[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorld_Point[i]), ViewMatrix));
		}

		XMStoreFloat4(&m_vPlaneInWorld[0], XMPlaneFromPoints(XMLoadFloat3(&m_vWorld_Point[1]), XMLoadFloat3(&m_vWorld_Point[5]), XMLoadFloat3(&m_vWorld_Point[6])));
		XMStoreFloat4(&m_vPlaneInWorld[1], XMPlaneFromPoints(XMLoadFloat3(&m_vWorld_Point[4]), XMLoadFloat3(&m_vWorld_Point[0]), XMLoadFloat3(&m_vWorld_Point[3])));
		XMStoreFloat4(&m_vPlaneInWorld[2], XMPlaneFromPoints(XMLoadFloat3(&m_vWorld_Point[4]), XMLoadFloat3(&m_vWorld_Point[5]), XMLoadFloat3(&m_vWorld_Point[1])));
		XMStoreFloat4(&m_vPlaneInWorld[3], XMPlaneFromPoints(XMLoadFloat3(&m_vWorld_Point[3]), XMLoadFloat3(&m_vWorld_Point[2]), XMLoadFloat3(&m_vWorld_Point[6])));
		XMStoreFloat4(&m_vPlaneInWorld[4], XMPlaneFromPoints(XMLoadFloat3(&m_vWorld_Point[5]), XMLoadFloat3(&m_vWorld_Point[4]), XMLoadFloat3(&m_vWorld_Point[7])));
		XMStoreFloat4(&m_vPlaneInWorld[5], XMPlaneFromPoints(XMLoadFloat3(&m_vWorld_Point[0]), XMLoadFloat3(&m_vWorld_Point[1]), XMLoadFloat3(&m_vWorld_Point[2])));
	}
}

const _bool CFrustum::IsIn_Frustum_World(_fvector vPoint, const _float& fRange) {
	for (_uint i = 0; i < 6; ++i) {
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlaneInWorld[i]), vPoint)))
			return false;
	}
	return true;
}
