#include "pch.h"
#include "Weapon_Spirit_Hand.h"
#include "Slasher_Huntress.h"

CWeapon_Spirit_Hand* const CWeapon_Spirit_Hand::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CWeapon_Spirit_Hand* pInstance = new CWeapon_Spirit_Hand(pDevice, pContext, pNetwork);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CObject* const CWeapon_Spirit_Hand::Clone(void* const& pArg) {
	CWeapon_Spirit_Hand* pInstance = new CWeapon_Spirit_Hand(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CWeapon_Spirit_Hand::CWeapon_Spirit_Hand(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CWeapon_Spirit_Hand::CWeapon_Spirit_Hand(const CWeapon_Spirit_Hand& rhs)
	: CObject(rhs) {
}

void CWeapon_Spirit_Hand::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pTransform_FPV);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pModel_FPV);
}

const HRESULT CWeapon_Spirit_Hand::Init_Create(void) {
	return S_OK;
}

const HRESULT CWeapon_Spirit_Hand::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_FPV"), reinterpret_cast<CComponent*&>(m_pTransform_FPV));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Weapon_Spirit_Hand"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Weapon_Spirit_Hand"), TEXT("Model_FPV"), reinterpret_cast<CComponent*&>(m_pModel_FPV));

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pSlasher_Huntress = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	m_pSlasher_Transform = dynamic_cast<CTransform*>(m_pSlasher_Huntress->Get_Component(TEXT("Transform")));

	return hr;
}

const _ubyte CWeapon_Spirit_Hand::Update(const _double& dTimeDelta) {
	//m_pCollider->Update(m_pTransform->Get_Matrix());
	//m_pCollider_FPV->Update(m_pTransform_FPV->Get_Matrix());
	return OBJ_NOEVENT;
}

void CWeapon_Spirit_Hand::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	//m_pCollider->Update_Edit(1);
	m_pTransform_FPV->Update_Edit(2);
}

void CWeapon_Spirit_Hand::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CModel* pModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model")));
	CModel* pModel_FPV = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model_FPV")));
	_matrix BoneMatrix = pModel->Get_BoneMatrix("joint_RingART_01");
	_matrix BoneMatrix_FPV = pModel_FPV->Get_BoneMatrix("joint_RingART_01");

	_matrix SocketMatrix;
	_matrix SocketMatrix_FPV;

	SocketMatrix = BoneMatrix * m_pSlasher_Transform->Get_Matrix();
	SocketMatrix_FPV = BoneMatrix_FPV * m_pSlasher_Transform->Get_Matrix();

	m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
	m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
	m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
	m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);

	m_pTransform_FPV->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix_FPV.r[0]);
	m_pTransform_FPV->Set_Row(CTransform::ERow::UP, SocketMatrix_FPV.r[1]);
	m_pTransform_FPV->Set_Row(CTransform::ERow::LOOK, -SocketMatrix_FPV.r[2]);
	m_pTransform_FPV->Set_Row(CTransform::ERow::POS, SocketMatrix_FPV.r[3]);

	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	//m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CWeapon_Spirit_Hand::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		if (true == m_pSlasher_Huntress->Get_Control()) {
			m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform_FPV->Get_Float4x4_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel_FPV->Get_NumMesh(); ++i) {
				m_pModel_FPV->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				if (FAILED(m_pModel_FPV->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
					m_pModel_FPV->Render(i, m_pShader, 0);
				else
					m_pModel_FPV->Render(i, m_pShader, 8);
			}
		}
		else {
			if (m_pSlasher_Huntress->Get_FM_Slasher()->Get_State() != CFM_Slasher::IDLE_SPIRIT) {
				m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
					m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
					if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
						m_pModel->Render(i, m_pShader, 0);
					else
						m_pModel->Render(i, m_pShader, 8);
				}
			}
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		////m_pCollider->Render(pCamera);
		//m_pCollider_FPV->Render(pCamera);
		break;
	}
	}
	return S_OK;
}
