#include "Collider.h"
#include "DebugDraw.h"
#include "Camera.h"
#include "Game_Instance.h"
#include "Cell.h"
#include "UI.h"

CCollider* const CCollider::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EType& eType) {
	CCollider* pInstance = new CCollider(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(eType)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CCollider::Clone(void* const& pArg) {
	CCollider* pInstance = new CCollider(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CCollider::CCollider(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs),
	m_eType(rhs.m_eType),
	m_pBatch(rhs.m_pBatch),
	m_pEffect(rhs.m_pEffect),
	m_pInputLayout(rhs.m_pInputLayout) {
	Safe_AddRef(m_pInputLayout);
	switch (m_eType) {
	case EType::AABB:
		m_pAABB_Local = new BoundingBox(*rhs.m_pAABB_Local);
		break;
	case EType::OBB:
		m_pOBB_Local = new BoundingOrientedBox(*rhs.m_pOBB_Local);
		break;
	case EType::SPHERE:
		m_pSphere_Local = new BoundingSphere(*rhs.m_pSphere_Local);
		break;
	}
}

void CCollider::Delete(void) {
	__super::Delete();
	Safe_Delete(m_pSphere_World);
	Safe_Delete(m_pOBB_World);
	Safe_Delete(m_pAABB_World);

	Safe_Delete(m_pSphere_Local);
	Safe_Delete(m_pOBB_Local);
	Safe_Delete(m_pAABB_Local);

	if (false == m_bClone) {
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
	}
	Safe_Release(m_pInputLayout);
}

const HRESULT CCollider::Init_Create(const EType& eType) {
	m_eType = eType;
	switch (eType) {
	case EType::AABB:
		m_pAABB_Local = new BoundingBox(_float3{ 0.f,0.f,0.f }, _float3{ 0.5f, 0.5f, 0.5f });
		break;
	case EType::OBB:
		m_pOBB_Local = new BoundingOrientedBox(_float3{ 0.f,0.f,0.f }, _float3{ 0.5f, 0.5f, 0.5f }, _float4{ 0.f, 0.f, 0.f, 1.f });
		break;
	case EType::SPHERE:
		m_pSphere_Local = new BoundingSphere(_float3{ 0.f,0.f,0.f }, 0.5f);
		break;
	}

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t	ShaderByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &ShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, ShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
	return S_OK;
}

const HRESULT CCollider::Init_Clone(void* const& pArg) {
	SCollider sCollider;

	if (nullptr != pArg)
		sCollider = *reinterpret_cast<SCollider*>(pArg);

	switch (m_eType) {
	case EType::AABB:
		m_pAABB_Local->Center = sCollider.vPos;
		m_pAABB_Local->Extents = _float3{ sCollider.vScale.x * 0.5f, sCollider.vScale.y * 0.5f, sCollider.vScale.z * 0.5f };

		m_pAABB_World = new BoundingBox();
		break;
	case EType::OBB:
		m_pOBB_Local->Center = sCollider.vPos;
		m_pOBB_Local->Extents = _float3{ sCollider.vScale.x * 0.5f, sCollider.vScale.y * 0.5f, sCollider.vScale.z * 0.5f };

		m_pOBB_World = new BoundingOrientedBox();
		break;
	case EType::SPHERE:
		m_pSphere_Local->Center = sCollider.vPos;
		m_pSphere_Local->Radius = sCollider.vRadius * 0.5f;

		m_pSphere_World = new BoundingSphere();
		break;
	}
	return S_OK;
}

void CCollider::Update(_fmatrix Matrix) {
	switch (m_eType) {
	case EType::AABB:
		_matrix	Result = Matrix;

		_vector	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[0]));
		_vector	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[1]));
		_vector	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[2]));

		Result.r[0] = vRight;
		Result.r[1] = vUp;
		Result.r[2] = vLook;

		m_pAABB_Local->Transform(*m_pAABB_World, Result);
		break;
	case EType::OBB:
		m_pOBB_Local->Transform(*m_pOBB_World, Matrix);
		break;
	case EType::SPHERE:
		m_pSphere_Local->Transform(*m_pSphere_World, Matrix);
		break;
	}
}

void CCollider::Update_Edit(const _int& iPushID) {
	ImGui::PushID(iPushID);
	ImGui::Text("Collider");
	ImGui::Separator();

	static float vCenter[3];
	switch (m_eType) {
	case EType::AABB:
		memcpy(vCenter, &m_pAABB_Local->Center, sizeof(_float3));
		if (ImGui::DragFloat3("Center", vCenter, 0.01f))
			m_pAABB_Local->Center = *(_float3*)vCenter;
		break;
	case EType::OBB:
		memcpy(vCenter, &m_pOBB_Local->Center, sizeof(_float3));
		if (ImGui::DragFloat3("Center", vCenter, 0.01f))
			m_pOBB_Local->Center = *(_float3*)vCenter;
		break;
	case EType::SPHERE:
		memcpy(vCenter, &m_pSphere_Local->Center, sizeof(_float3));
		if (ImGui::DragFloat3("Center", vCenter, 0.01f))
			m_pSphere_Local->Center = *(_float3*)vCenter;
		break;
	}

	static float vExtent[3];
	static float vRadius = 0.f;
	switch (m_eType) {
	case EType::AABB:
		memcpy(vExtent, &m_pAABB_Local->Extents, sizeof(_float3));
		if (ImGui::DragFloat3("Extent", vExtent, 0.01f))
			m_pAABB_Local->Extents = *(_float3*)vExtent;
		break;
	case EType::OBB:
		memcpy(vExtent, &m_pOBB_Local->Extents, sizeof(_float3));
		if (ImGui::DragFloat3("Extent", vExtent, 0.01f))
			m_pOBB_Local->Extents = *(_float3*)vExtent;
		break;
	case EType::SPHERE:
		memcpy(&vRadius, &m_pSphere_Local->Radius, sizeof(_float));
		if (ImGui::DragFloat("vRadius", &vRadius, 0.01f))
			m_pSphere_Local->Radius = vRadius;
		break;
	}

	switch (m_eType) {
	case EType::AABB:
	case EType::OBB: {
		static bool bPlane[6] = { false, false, false, false, false, false };
		if (ImGui::Button("Front")) {}ImGui::SameLine();
		if (ImGui::IsItemClicked(0)) bPlane[0] = true;
		if (ImGui::Button("Back")) {}ImGui::SameLine();
		if (ImGui::IsItemClicked(0)) bPlane[1] = true;

		if (ImGui::Button("Left")) {}ImGui::SameLine();
		if (ImGui::IsItemClicked(0)) bPlane[2] = true;
		if (ImGui::Button("Right")) {}ImGui::SameLine();
		if (ImGui::IsItemClicked(0)) bPlane[3] = true;

		if (ImGui::Button("Top")) {} ImGui::SameLine();
		if (ImGui::IsItemClicked(0)) bPlane[4] = true;
		if (ImGui::Button("Bottom")) {}ImGui::SameLine();
		if (ImGui::IsItemClicked(0)) bPlane[5] = true;

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		if (false == (pGame_Instance->Get_Input_MouseButton(CInput::EMouseButton::L) & 0x80)) {
			ZeroMemory(bPlane, sizeof(bool) * 6);
		}
		_long MouseMove;
		if (MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X)) {
			_float fMove = static_cast<_float>(MouseMove) * 0.005f;
			switch (m_eType) {
			case EType::AABB:
				if (true == bPlane[0]) {
					m_pAABB_Local->Extents.z += fMove;
					m_pAABB_Local->Center.z += fMove;
				}
				if (true == bPlane[1]) {
					m_pAABB_Local->Extents.z += fMove;
					m_pAABB_Local->Center.z -= fMove;
				}
				if (true == bPlane[2]) {
					m_pAABB_Local->Extents.x += fMove;
					m_pAABB_Local->Center.x -= fMove;
				}
				if (true == bPlane[3]) {
					m_pAABB_Local->Extents.x += fMove;
					m_pAABB_Local->Center.x += fMove;
				}
				if (true == bPlane[4]) {
					m_pAABB_Local->Extents.y += fMove;
					m_pAABB_Local->Center.y += fMove;
				}
				if (true == bPlane[5]) {
					m_pAABB_Local->Extents.y += fMove;
					m_pAABB_Local->Center.y -= fMove;
				}
				break;
			case EType::OBB:
				if (true == bPlane[0]) {
					m_pOBB_Local->Extents.z += fMove;
					m_pOBB_Local->Center.z += fMove;
				}
				if (true == bPlane[1]) {
					m_pOBB_Local->Extents.z += fMove;
					m_pOBB_Local->Center.z -= fMove;
				}
				if (true == bPlane[2]) {
					m_pOBB_Local->Extents.x += fMove;
					m_pOBB_Local->Center.x -= fMove;
				}
				if (true == bPlane[3]) {
					m_pOBB_Local->Extents.x += fMove;
					m_pOBB_Local->Center.x += fMove;
				}
				if (true == bPlane[4]) {
					m_pOBB_Local->Extents.y += fMove;
					m_pOBB_Local->Center.y += fMove;
				}
				if (true == bPlane[5]) {
					m_pOBB_Local->Extents.y += fMove;
					m_pOBB_Local->Center.y -= fMove;
				}
				break;
			}
		}
	}
				   break;
	}

	ImGui::Separator();
	ImGui::PopID();
}

const HRESULT CCollider::Render(CCamera* const& pCamera) {
	m_pContext->OMSetDepthStencilState(NULL, 0);
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(pCamera->Get_ViewMatrix());
	m_pEffect->SetProjection(pCamera->Get_ProjMatrix());

	_vector vColor = true == m_bCollision ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	switch (m_eType) {
	case EType::AABB:
		DX::Draw(m_pBatch, *m_pAABB_World, vColor);
		break;
	case EType::OBB:
		DX::Draw(m_pBatch, *m_pOBB_World, vColor);
		break;
	case EType::SPHERE:
		DX::Draw(m_pBatch, *m_pSphere_World, vColor);
		break;
	}

	m_pBatch->End();
	return S_OK;
}

const HRESULT CCollider::Render(void) {
	m_pContext->GSSetShader(nullptr, nullptr, 0);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(CUI::Get_ViewMatrix());
	m_pEffect->SetProjection(CUI::Get_ProjMatrix());

	_vector vColor = true == m_bCollision ? DirectX::Colors::Yellow : DirectX::Colors::Coral;

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	switch (m_eType) {
	case EType::AABB:
		DX::Draw(m_pBatch, *m_pAABB_World, vColor);
		break;
	case EType::OBB:
		DX::Draw(m_pBatch, *m_pOBB_World, vColor);
		break;
	case EType::SPHERE:
		DX::Draw(m_pBatch, *m_pSphere_World, vColor);
		break;
	}

	m_pBatch->End();
	return S_OK;
}

const _float3 CCollider::Get_Center(void) {
	switch (m_eType) {
	case EType::AABB:
		return m_pAABB_World->Center;
	case EType::OBB:
		return m_pOBB_World->Center;
	case EType::SPHERE:
		return m_pSphere_World->Center;
	}
	return _float3{ 0.f,0.f,0.f };
}

const _float3 CCollider::Get_Center_Local(void) {
	switch (m_eType) {
	case EType::AABB:
		return m_pAABB_Local->Center;
	case EType::OBB:
		return m_pOBB_Local->Center;
	case EType::SPHERE:
		return m_pSphere_Local->Center;
	}
	return _float3{ 0.f,0.f,0.f };
}

const _float3 CCollider::Get_Extents(void) {
	switch (m_eType) {
	case EType::AABB:
		return m_pAABB_World->Extents;
	case EType::OBB:
		return m_pOBB_World->Extents;
	}
	return _float3{ 0.f,0.f,0.f };
}

const _float CCollider::Get_Radius(void) {
	if (EType::SPHERE == m_eType)
		return m_pSphere_World->Radius;
	return 0.f;
}

void CCollider::Set_Center(const _float3& vCenter) {
	switch (m_eType) {
	case EType::AABB:
		m_pAABB_Local->Center = vCenter;
		break;
	case EType::OBB:
		m_pOBB_Local->Center = vCenter;
		break;
	case EType::SPHERE:
		m_pSphere_Local->Center = vCenter;
		break;
	}
}

void CCollider::Set_Extents(const _float3& vExtents) {
	switch (m_eType) {
	case EType::AABB:
		m_pAABB_Local->Extents = vExtents;
		break;
	case EType::OBB:
		m_pOBB_Local->Extents = vExtents;
		break;
	}
}

const _bool CCollider::Is_Collision(CCollider* const& pCollider) {
	switch (m_eType) {
	case EType::AABB:
		switch (pCollider->m_eType) {
		case EType::AABB:
			return m_bCollision = m_pAABB_World->Intersects(*pCollider->m_pAABB_World);
		case EType::OBB:
			return m_bCollision = m_pAABB_World->Intersects(*pCollider->m_pOBB_World);
		case EType::SPHERE:
			return m_bCollision = m_pAABB_World->Intersects(*pCollider->m_pSphere_World);
		}
		break;
	case EType::OBB:
		switch (pCollider->m_eType) {
		case EType::AABB:
			return m_bCollision = m_pOBB_World->Intersects(*pCollider->m_pAABB_World);
		case EType::OBB:
			return m_bCollision = m_pOBB_World->Intersects(*pCollider->m_pOBB_World);
		case EType::SPHERE:
			return m_bCollision = m_pOBB_World->Intersects(*pCollider->m_pSphere_World);
		}
		break;
	case EType::SPHERE:
		switch (pCollider->m_eType) {
		case EType::AABB:
			return m_bCollision = m_pSphere_World->Intersects(*pCollider->m_pAABB_World);
		case EType::OBB:
			return m_bCollision = m_pSphere_World->Intersects(*pCollider->m_pOBB_World);
		case EType::SPHERE:
			return m_bCollision = m_pSphere_World->Intersects(*pCollider->m_pSphere_World);
		}
		break;
	}
	return false;
}
const _bool CCollider::Is_CollisionEnter(_fvector vOrigin, _fvector vDir, _float3* const& pOut) {
	_bool bCollision = Is_Collision(vOrigin, vDir, pOut);

	if (false == bCollision) {
		m_bCollisionEnter = false;
		return false;
	}
	else {
		if (false == m_bCollisionEnter) {
			m_bCollisionEnter = true;
			return true;
		}
		else
			return false;
	}
}


const _bool CCollider::Is_Collision(_fvector vOrigin, _fvector vDir, _float3* const& pOut) {
	_float fDist = 0.f;
	m_bCollision = false;
	switch (m_eType) {
	case EType::AABB:
		m_bCollision = m_pAABB_World->Intersects(vOrigin, vDir, fDist);
		break;
	case EType::OBB:
		m_bCollision = m_pOBB_World->Intersects(vOrigin, vDir, fDist);
		break;
	case EType::SPHERE:
		m_bCollision = m_pSphere_World->Intersects(vOrigin, vDir, fDist);
		break;
	}
	if (nullptr != pOut)
		XMStoreFloat3(pOut, vOrigin + vDir * fDist);
	return m_bCollision;
}

const _bool CCollider::Is_Collision(CCell* const& pCell) {
	_bool bCollision = false;
	switch (m_eType) {
	case EType::AABB:
		bCollision = m_pAABB_World->Intersects(pCell->Get_Point(CCell::EPoint::A), pCell->Get_Point(CCell::EPoint::B), pCell->Get_Point(CCell::EPoint::C));
		break;
	case EType::OBB:
		bCollision = m_pOBB_World->Intersects(pCell->Get_Point(CCell::EPoint::A), pCell->Get_Point(CCell::EPoint::B), pCell->Get_Point(CCell::EPoint::C));
		break;
	case EType::SPHERE:
		bCollision = m_pSphere_World->Intersects(pCell->Get_Point(CCell::EPoint::A), pCell->Get_Point(CCell::EPoint::B), pCell->Get_Point(CCell::EPoint::C));
		break;
	}
	return bCollision;
}

_float3* const CCollider::Get_Corner(void) {
	m_pOBB_World->GetCorners(m_vCorner);
	return m_vCorner;
}

void CCollider::Save_Data(Json::Value& root) {
	switch (m_eType) {
	case EType::AABB:
		root["Center"]["x"] = m_pAABB_Local->Center.x;
		root["Center"]["y"] = m_pAABB_Local->Center.y;
		root["Center"]["z"] = m_pAABB_Local->Center.z;
		root["Extents"]["x"] = m_pAABB_Local->Extents.x;
		root["Extents"]["y"] = m_pAABB_Local->Extents.y;
		root["Extents"]["z"] = m_pAABB_Local->Extents.z;
		break;
	case EType::OBB:
		root["Center"]["x"] = m_pOBB_Local->Center.x;
		root["Center"]["y"] = m_pOBB_Local->Center.y;
		root["Center"]["z"] = m_pOBB_Local->Center.z;
		root["Extents"]["x"] = m_pOBB_Local->Extents.x;
		root["Extents"]["y"] = m_pOBB_Local->Extents.y;
		root["Extents"]["z"] = m_pOBB_Local->Extents.z;
		break;
	case EType::SPHERE:
		root["Center"]["x"] = m_pSphere_Local->Center.x;
		root["Center"]["y"] = m_pSphere_Local->Center.y;
		root["Center"]["z"] = m_pSphere_Local->Center.z;
		root["Radius"] = m_pSphere_Local->Radius;
		break;
	}
}

void CCollider::Load_Data(Json::Value& root) {
	switch (m_eType) {
	case EType::AABB:
		m_pAABB_Local->Center.x = root["Center"]["x"].asFloat();
		m_pAABB_Local->Center.y = root["Center"]["y"].asFloat();
		m_pAABB_Local->Center.z = root["Center"]["z"].asFloat();
		m_pAABB_Local->Extents.x = root["Extents"]["x"].asFloat();
		m_pAABB_Local->Extents.y = root["Extents"]["y"].asFloat();
		m_pAABB_Local->Extents.z = root["Extents"]["z"].asFloat();
		break;
	case EType::OBB:
		m_pOBB_Local->Center.x = root["Center"]["x"].asFloat();
		m_pOBB_Local->Center.y = root["Center"]["y"].asFloat();
		m_pOBB_Local->Center.z = root["Center"]["z"].asFloat();
		m_pOBB_Local->Extents.x = root["Extents"]["x"].asFloat();
		m_pOBB_Local->Extents.y = root["Extents"]["y"].asFloat();
		m_pOBB_Local->Extents.z = root["Extents"]["z"].asFloat();
		break;
	case EType::SPHERE:
		m_pSphere_Local->Center.x = root["Center"]["x"].asFloat();
		m_pSphere_Local->Center.y = root["Center"]["y"].asFloat();
		m_pSphere_Local->Center.z = root["Center"]["z"].asFloat();
		m_pSphere_Local->Radius = root["Radius"].asFloat();
		break;
	}
}
