#include "pch.h"
#include "Throw_Trail.h"
#include "Weapon_Throw.h"
#include "Slasher_Huntress.h"

CThrow_Trail* const CThrow_Trail::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CThrow_Trail* pInstnace = new CThrow_Trail(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CThrow_Trail::Clone(void* const& pArg) {
	CThrow_Trail* pInstnace = new CThrow_Trail(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CThrow_Trail::CThrow_Trail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CThrow_Trail::CThrow_Trail(const CThrow_Trail& rhs)
	: CObject(rhs) {
}

void CThrow_Trail::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pCollider);
}

const HRESULT CThrow_Trail::Init_Create(void) {
	return S_OK;
}

const HRESULT CThrow_Trail::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Tex"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Trail"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Trail"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	//m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(0.06f, 0.047f, 0.18f, 0.f));
	//m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(0.1f, 0.49f, 0.15f, 0.f));

	return hr;
}

const _ubyte CThrow_Trail::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CWeapon_Throw* pWeapon_Throw = dynamic_cast<CWeapon_Throw*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Throw")));
	CTransform* pWeaponTransform = dynamic_cast<CTransform*>(pWeapon_Throw->Get_Component(TEXT("Transform")));

	_float3 vPos;
	XMStoreFloat3(&vPos, pWeaponTransform->Get_Row(CTransform::ERow::POS) - XMVector3Normalize(XMLoadFloat3(&pWeapon_Throw->Get_Dir())));
	_float3 vPos2;
	XMStoreFloat3(&vPos2, pWeaponTransform->Get_Row(CTransform::ERow::POS) - XMVector3Normalize(XMLoadFloat3(&pWeapon_Throw->Get_Dir())) - XMVectorSet(0.f, -0.1f, 0.f, 0.f));
	m_pBuffer->Set_Trail(vPos, vPos2);

	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CThrow_Trail::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CThrow_Trail::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::GLOW, this);
	//m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
	//m_pRenderer->Add_Object(CRenderer::EOrder::ALPHABLEND, this);
}

const HRESULT CThrow_Trail::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::GLOW: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CWeapon_Throw* pWeapon_Throw = dynamic_cast<CWeapon_Throw*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Throw")));

		if (true == pWeapon_Throw->Get_Throw()) {
			m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			m_pBuffer->Render(m_pShader, 2);
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);

		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CWeapon_Throw* pWeapon_Throw = dynamic_cast<CWeapon_Throw*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Throw")));

		if (true == pWeapon_Throw->Get_Throw()) {
			m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			m_pBuffer->Render(m_pShader, 2);
		}
		break;
	}
	case CRenderer::EOrder::ALPHABLEND : {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CWeapon_Throw* pWeapon_Throw = dynamic_cast<CWeapon_Throw*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Throw")));

		if (true == pWeapon_Throw->Get_Throw()) {
			m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			m_pBuffer->Render(m_pShader, 2);
		}
		break;
	}
	}
	return S_OK;
}
