#include "Transform.h"
#include "Navi.h"
#include "Game_Instance.h"


CTransform* const CTransform::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CTransform* pInstance = new CTransform(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CTransform::Clone(void* const& pArg) {
	CTransform* pInstance = new CTransform(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CTransform::CTransform(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
	XMStoreFloat4x4(&m_Float4x4, XMMatrixIdentity());
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs),
	m_Float4x4(rhs.m_Float4x4) {
}

void CTransform::Delete(void) {
	__super::Delete();
}

const HRESULT CTransform::Init_Create(void) {
	return S_OK;
}

const HRESULT CTransform::Init_Clone(void* const& pArg) {
	return S_OK;
}

void CTransform::Update_Edit(const _int& iPushID) {
	ImGui::PushID(iPushID);
	ImGui::Separator();
	ImGui::Text("Transform");
	//ImGui::Separator();


	float vPos[3];
	memcpy(vPos, &Get_Row(CTransform::ERow::POS), sizeof(_float3));
	if (ImGui::DragFloat3("Pos", vPos, 0.01f))
		Set_Row(CTransform::ERow::POS, XMLoadFloat3((_float3*)vPos));


	//회전
	static int iID = 0;
	static bool bRot[3] = { false, false, false };
	if (ImGui::Button("Right")) {}ImGui::SameLine();
	if (ImGui::IsItemClicked(0)) { bRot[0] = true; iID = iPushID; }
	if (ImGui::Button(" Up  ")) {}ImGui::SameLine();
	if (ImGui::IsItemClicked(0)) { bRot[1] = true; iID = iPushID; }
	if (ImGui::Button("Look ")) {} ImGui::SameLine();
	if (ImGui::IsItemClicked(0)) { bRot[2] = true; iID = iPushID; }
	if (ImGui::Button("Init "))
		Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f);
	ImGui::SameLine();
	ImGui::Text("Rot");

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
		bRot[0] = false;
		bRot[1] = false;
		bRot[2] = false;
	}
	_long MouseMove;
	if (iID == iPushID) {
		if (MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X)) {
			if (true == bRot[0])
				Rotate(Get_Row(CTransform::ERow::RIGHT), static_cast<_float>(MouseMove) * 0.005f, 1.f);
			if (true == bRot[1])
				Rotate(Get_Row(CTransform::ERow::UP), static_cast<_float>(MouseMove) * 0.005f, 1.f);
			if (true == bRot[2])
				Rotate(Get_Row(CTransform::ERow::LOOK), static_cast<_float>(MouseMove) * 0.005f, 1.f);
		}
	}
	static float s_fDegree = 0;
	if (ImGui::Button("Right2")) {
		Rotate(Get_Row(CTransform::ERow::RIGHT),  XMConvertToRadians(s_fDegree), 1.f);
	}ImGui::SameLine();
	if (ImGui::Button(" Up  2")) {
		Rotate(Get_Row(CTransform::ERow::UP), XMConvertToRadians(s_fDegree), 1.f);
	}ImGui::SameLine();
	if (ImGui::Button("Look 2")) {
		Rotate(Get_Row(CTransform::ERow::LOOK),XMConvertToRadians(s_fDegree), 1.f);
	} ImGui::SameLine();
	ImGui::Text("Rot2");
	ImGui::DragFloat("Degree", &s_fDegree);


	//스케일
	static float vScale[3];
	memcpy(vScale, &Get_Scale(), sizeof(_float3));
	if (ImGui::DragFloat3("Scale", vScale, 0.01f))
		if (vScale[0] > 0.f && vScale[1] > 0.f && vScale[2] > 0.f)
			Set_Scale(*(_float3*)vScale);

	ImGui::Separator();
	ImGui::PopID();
}

_matrix CTransform::Get_Matrix(void) const {
	return XMLoadFloat4x4(&m_Float4x4);
}

_matrix CTransform::Get_Matrix_Inverse(void) const {
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_Float4x4));
}

_matrix CTransform::Get_Matrix_Transpose(void) const {
	return XMMatrixTranspose(XMLoadFloat4x4(&m_Float4x4));
}

void CTransform::Set_Matrix(const _float4x4& Float4x4) {
	m_Float4x4 = Float4x4;
}

void CTransform::Set_Matrix(_fmatrix Matrix) {
	XMStoreFloat4x4(&m_Float4x4, Matrix);
}

const _float4x4& CTransform::Get_Float4x4(void) const {
	return m_Float4x4;
}

const _float4x4 CTransform::Get_Float4x4_Transpose(void) const {
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_Float4x4)));
	return WorldMatrix;
}

_vector CTransform::Get_Row(const ERow& eRow) const {
	return XMLoadFloat4((_float4*)&m_Float4x4.m[static_cast<_ubyte>(eRow)][0]);
}

const _float3& CTransform::Get_Float3(const ERow& eRow) const {
	return *((_float3*)&m_Float4x4.m[static_cast<_ubyte>(eRow)][0]);
}

void CTransform::Set_Row(const ERow& eRow, _fvector vState) {
	XMStoreFloat3((_float3*)&m_Float4x4.m[static_cast<_ubyte>(eRow)][0], vState);
}

void CTransform::Translate(_fvector vDir, const _float& fDistance, const _double& dTimeDelta, class CNavi* pNavi) {
	_vector vPos = Get_Row(ERow::POS);
	vPos += XMVector3Normalize(vDir) * fDistance * static_cast<_float>(dTimeDelta);	//타임델타 플롯 하면 안되지않나

	if (nullptr != pNavi)
		if (false == pNavi->Is_Move(vPos))
			return;
	Set_Row(ERow::POS, vPos);
}

void CTransform::Rotation(_fvector vAxis, const _float& fRadian) {
	_float3 vScale = Get_Scale();
	_vector vRight = _vector{ 1.f, 0.f, 0.f } *vScale.x;
	_vector vUp = _vector{ 0.f, 1.f, 0.f } *vScale.y;
	_vector vLook = _vector{ 0.f, 0.f, 1.f } *vScale.z;

	_matrix RotMatrix;
	RotMatrix = XMMatrixRotationAxis(vAxis, static_cast<_float>(fRadian));

	Set_Row(ERow::RIGHT, XMVector3TransformNormal(vRight, RotMatrix));
	Set_Row(ERow::UP, XMVector3TransformNormal(vUp, RotMatrix));
	Set_Row(ERow::LOOK, XMVector3TransformNormal(vLook, RotMatrix));
}

void CTransform::Rotate(_fvector vAxis, const _float& fRadian, const _double& dTimeDelta) {
	_matrix RotMatrix;
	RotMatrix = XMMatrixRotationAxis(vAxis, static_cast<_float>(fRadian * dTimeDelta));

	_vector vRight = Get_Row(CTransform::ERow::RIGHT);
	_vector vUp = Get_Row(CTransform::ERow::UP);
	_vector vLook = Get_Row(CTransform::ERow::LOOK);

	Set_Row(ERow::RIGHT, XMVector3TransformNormal(vRight, RotMatrix));
	Set_Row(ERow::UP, XMVector3TransformNormal(vUp, RotMatrix));
	Set_Row(ERow::LOOK, XMVector3TransformNormal(vLook, RotMatrix));
}

const _float3 CTransform::Get_Scale(void) {
	_float3 vScale;
	ZeroMemory(&vScale, sizeof(_float3));
	vScale.x = XMVectorGetX(XMVector3Length(Get_Row(ERow::RIGHT)));
	vScale.y = XMVectorGetX(XMVector3Length(Get_Row(ERow::UP)));
	vScale.z = XMVectorGetX(XMVector3Length(Get_Row(ERow::LOOK)));
	return vScale;
}

void CTransform::Set_Scale(const _float3& vScale) {
	if (isnan(vScale.x) || isnan(vScale.y) || isnan(vScale.z) ||
		0.f == vScale.x || 0.f == vScale.y || 0.f == vScale.z)
		return;
	_vector vRight = Get_Row(ERow::RIGHT);
	_vector vUp = Get_Row(ERow::UP);
	_vector vLook = Get_Row(ERow::LOOK);

	vRight = XMVector3Normalize(vRight) * vScale.x;
	vUp = XMVector3Normalize(vUp) * vScale.y;
	vLook = XMVector3Normalize(vLook) * vScale.z;

	Set_Row(ERow::RIGHT, vRight);
	Set_Row(ERow::UP, vUp);
	Set_Row(ERow::LOOK, vLook);
}

void CTransform::Set_Scale(_fvector vScale) {
	_float3 vScale2;
	XMStoreFloat3(&vScale2, vScale);
	if (isnan(vScale2.x) || isnan(vScale2.y) || isnan(vScale2.z) ||
		0.f >= vScale2.x || 0.f >= vScale2.y || 0.f >= vScale2.z)
		return;
	_vector vRight = Get_Row(ERow::RIGHT);
	_vector vUp = Get_Row(ERow::UP);
	_vector vLook = Get_Row(ERow::LOOK);

	vRight = XMVector3Normalize(vRight) * vScale2.x;
	vUp = XMVector3Normalize(vUp) * vScale2.y;
	vLook = XMVector3Normalize(vLook) * vScale2.z;

	Set_Row(ERow::RIGHT, vRight);
	Set_Row(ERow::UP, vUp);
	Set_Row(ERow::LOOK, vLook);
}

void CTransform::Set_Scale(const ERow& eRow, const _float& fScale) {
	if (isnan(fScale) || 0.f >= fScale)
		return;
	_vector vScale = Get_Row(eRow);
	vScale = XMVector3Normalize(vScale) * fScale;
	Set_Row(eRow, vScale);
}

const _float CTransform::Get_Dist(CTransform* pTransform) const {
	_vector	vPos = Get_Row(CTransform::ERow::POS);
	_vector	vTargetPos = pTransform->Get_Row(CTransform::ERow::POS);
	return XMVectorGetX(XMVector3Length(vPos - vTargetPos));
}

const _float CTransform::Get_Dist(_fvector vTargetPos) const {
	_vector	vPos = Get_Row(CTransform::ERow::POS);
	_float fDist = XMVectorGetX(XMVector3Length(vPos - vTargetPos));
	if (isnan(fDist))
		return 0;
	return fDist;
}

void CTransform::LookAt(_fvector vAt) {
	_vector vLook = vAt - Get_Row(ERow::POS);
	if (0.f >= XMVectorGetX(XMVector3Length(vLook)))
		return;
	_vector vRight = XMVector3Cross(Get_Row(ERow::UP), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();
	Set_Row(ERow::RIGHT, vRight);
	Set_Row(ERow::UP, vUp);
	Set_Row(ERow::LOOK, vLook);
	Set_Scale(vScale);
}

void CTransform::UpAt(_fvector vAt) {
	_vector vUp = vAt - Get_Row(ERow::POS);
	if (0.f >= XMVectorGetX(XMVector3Length(vUp)))
		return;
	_vector vRight = XMVector3Cross(vUp, Get_Row(ERow::LOOK));
	_vector vLook = XMVector3Cross(vRight, vUp);

	_float3 vScale = Get_Scale();
	Set_Row(ERow::RIGHT, vRight);
	Set_Row(ERow::UP, vUp);
	Set_Row(ERow::LOOK, vLook);
	Set_Scale(vScale);
}

const HRESULT CTransform::Set_RawValue(CShader* const& pShader) {
	pShader->Set_RawValue("g_WorldMatrix", &Get_Matrix_Transpose(), sizeof(_float4x4));
	return S_OK;
}

void CTransform::Trace(_fvector vTargetPos, const _float& fSpeed ,const _float& fLimitDistance, const _double& dTimeDelta, CNavi* pNavi){
	_vector vPos = Get_Row(ERow::POS);
	_vector vDir = XMVector3Normalize(vTargetPos - vPos);
	if (XMVectorGetX(XMVector3Length(vPos - vTargetPos)) >= fLimitDistance){
		vPos += XMVector3Normalize(vDir) * fSpeed * static_cast<_float>(dTimeDelta);
	}
	if (nullptr != pNavi)
		if (false == pNavi->Is_Move(vPos))
			return;
	Set_Row(ERow::POS, vPos);
}

void CTransform::Save_Data(Json::Value& root) {
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			root["Matrix"][to_string(i)][to_string(j)] = m_Float4x4.m[i][j];
	/*m_Float4x4.m[3][0] = m_Float4x4.m[3][0] + 20.f;
	m_Float4x4.m[3][2] = m_Float4x4.m[3][2] + 20.f;
	root["Matrix"][to_string(3)][to_string(0)] = m_Float4x4.m[3][0];
	root["Matrix"][to_string(3)][to_string(2)] = m_Float4x4.m[3][2];*/
}

void CTransform::Load_Data(Json::Value& root) {
	_float4x4 Float4x4;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			Float4x4.m[i][j] = root["Matrix"][to_string(i)][to_string(j)].asFloat();
	Set_Matrix(Float4x4);
}
