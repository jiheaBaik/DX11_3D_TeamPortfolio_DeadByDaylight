#include "pch.h"
#include "Light_Slasher_Eye.h"

CLight_Slasher_Eye* const CLight_Slasher_Eye::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CLight_Slasher_Eye* pInstnace = new CLight_Slasher_Eye(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CLight_Slasher_Eye::Clone(void* const& pArg) {
	CLight_Slasher_Eye* pInstnace = new CLight_Slasher_Eye(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CLight_Slasher_Eye::CLight_Slasher_Eye(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CLight(pDevice, pContext, pNetwork) {
}

CLight_Slasher_Eye::CLight_Slasher_Eye(const CLight_Slasher_Eye& rhs)
	: CLight(rhs) {
}

void CLight_Slasher_Eye::Delete(void) {
	__super::Delete();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform_Eye);
}

const HRESULT CLight_Slasher_Eye::Init_Create(void) {
	return S_OK;
}

const HRESULT CLight_Slasher_Eye::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Eye"), reinterpret_cast<CComponent*&>(m_pTransform_Eye));
	m_pTransform_Eye->Set_Row(CTransform::ERow::POS, XMVectorSet(0.f, 1.2f, 0.3f, 1.f));
	m_pTransform_Eye->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(25.f));
	return hr;
}

const _ubyte CLight_Slasher_Eye::Update(const _double& dTimeDelta) {
	return OBJ_NOEVENT;
}

void CLight_Slasher_Eye::Update_Edit(void) {
	__super::Update_Edit();
	m_pTransform_Eye->Update_Edit(0);
}

void CLight_Slasher_Eye::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CObject* pSlasher = pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"));
	if (nullptr != pSlasher) {
		CTransform* pSlasherTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));
		_matrix SlasherMatrix = m_pTransform_Eye->Get_Matrix() * pSlasherTransform->Get_Matrix();
		m_pTransform->Set_Matrix(SlasherMatrix);
	}

	m_pCollider->Update(m_pTransform->Get_Matrix());
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CLight_Slasher_Eye::Render(const CRenderer::EOrder& eOrder) {
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	//m_pCollider->Render(pCamera);
	return S_OK;
}
