#include "pch.h"
#include "Slasher_FPV_Trail.h"
#include "Weapon_Slash.h"
#include "Slasher_Huntress.h"

CSlasher_FPV_Trail* const CSlasher_FPV_Trail::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CSlasher_FPV_Trail* pInstnace = new CSlasher_FPV_Trail(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CSlasher_FPV_Trail::Clone(void* const& pArg) {
	CSlasher_FPV_Trail* pInstnace = new CSlasher_FPV_Trail(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CSlasher_FPV_Trail::CSlasher_FPV_Trail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CSlasher_FPV_Trail::CSlasher_FPV_Trail(const CSlasher_FPV_Trail& rhs)
	: CObject(rhs) {
}

void CSlasher_FPV_Trail::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pCollider);
}

const HRESULT CSlasher_FPV_Trail::Init_Create(void) {
	return S_OK;
}

const HRESULT CSlasher_FPV_Trail::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Tex"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_Trail"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Trail"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(-0.1f, 0.5f, 0.f, -0.04f));

	return hr;
}

const _ubyte CSlasher_FPV_Trail::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CWeapon_Slash* pWeapon_Slash = dynamic_cast<CWeapon_Slash*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Slash")));
	CTransform* pWeaponTransform = dynamic_cast<CTransform*>(pWeapon_Slash->Get_Component(TEXT("Transform_FPV")));

	_float3 vPos;
	XMStoreFloat3(&vPos, XMVector3TransformCoord(m_pTransform->Get_Row(CTransform::ERow::POS), pWeaponTransform->Get_Matrix()));
	_float3 vPos2;
	XMStoreFloat3(&vPos2, XMVector3TransformCoord(m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, -0.2f, 0.f, 0.f), pWeaponTransform->Get_Matrix()));
	m_pBuffer->Set_Trail(vPos, vPos2);

	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CSlasher_FPV_Trail::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CSlasher_FPV_Trail::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::GLOW, this);
	//m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
	//m_pRenderer->Add_Object(CRenderer::EOrder::ALPHABLEND, this);
}

const HRESULT CSlasher_FPV_Trail::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::GLOW: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		_float2 vGlow_Buffer{ 1.f, 2.f };
		m_pShader->Set_RawValue("g_vGlow_Buffer", &vGlow_Buffer, sizeof(_float2));
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
		if (true == pSlasher->Get_Control()) {
			if (pSlasher->Get_FM_Slasher()->Get_State() == CFM_Slasher::EState::ATTACK_SLASH_SWING || pSlasher->Get_FM_Slasher()->Get_State() == CFM_Slasher::EState::ATTACK_CARRY_SWING) {
				m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				m_pBuffer->Render(m_pShader, 2);
			}
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);

		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));

		if (true == pSlasher->Get_Control()) {
			if (pSlasher->Get_FM_Slasher()->Get_State() == CFM_Slasher::EState::ATTACK_SLASH_SWING || pSlasher->Get_FM_Slasher()->Get_State() == CFM_Slasher::EState::ATTACK_CARRY_SWING) {
				m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				m_pBuffer->Render(m_pShader, 2);
			}
		}

		break;
	}
	case CRenderer::EOrder::ALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));

		if (true == pSlasher->Get_Control()) {
			if (pSlasher->Get_FM_Slasher()->Get_State() == CFM_Slasher::EState::ATTACK_SLASH_SWING || pSlasher->Get_FM_Slasher()->Get_State() == CFM_Slasher::EState::ATTACK_CARRY_SWING) {
				m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				m_pBuffer->Render(m_pShader, 2);
			}
		}
		break;
	}
	}
	return S_OK;
}
