#include "pch.h"
#include "Camera_Reflect.h"

CCamera_Reflect* const CCamera_Reflect::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCamera_Reflect* pInstnace = new CCamera_Reflect(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCamera_Reflect::Clone(void* const& pArg) {
	CCamera_Reflect* pInstnace = new CCamera_Reflect(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCamera_Reflect::CCamera_Reflect(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CCamera(pDevice, pContext, pNetwork) {
}

CCamera_Reflect::CCamera_Reflect(const CCamera_Reflect& rhs)
	: CCamera(rhs) {
}

void CCamera_Reflect::Delete(void) {
	__super::Delete();
}

const HRESULT CCamera_Reflect::Init_Create(void) {
	return S_OK;
}

const HRESULT CCamera_Reflect::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	return hr;
}

const _ubyte CCamera_Reflect::Update(const _double& dTimeDelta) {
	return OBJ_NOEVENT;
}

void CCamera_Reflect::Late_Update(const _double& dTimeDelta) {
	Set_ViewMatrix();
}

const HRESULT CCamera_Reflect::Render(const CRenderer::EOrder& eOrder) {
	return S_OK;
}
