#include "pch.h"
#include "Ending_Slasher.h"
#include "Shadow.h"

CEnding_Slasher* const CEnding_Slasher::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEnding_Slasher* pInstnace = new CEnding_Slasher(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEnding_Slasher::Clone(void* const& pArg) {
	CEnding_Slasher* pInstnace = new CEnding_Slasher(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEnding_Slasher::CEnding_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEnding_Slasher::CEnding_Slasher(const CEnding_Slasher& rhs)
	: CObject(rhs) {
}

void CEnding_Slasher::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pShader_Weapon);
	Safe_Release(m_pModel);
	Safe_Release(m_pTransform_Weapon);
	Safe_Release(m_pModel_Weapon);
}

const HRESULT CEnding_Slasher::Init_Create(void) {
	return S_OK;
}

const HRESULT CEnding_Slasher::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelAnim"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader_Weapon"), reinterpret_cast<CComponent*&>(m_pShader_Weapon));
	
	CInfo::ESlasher eSlasher = *reinterpret_cast<CInfo::ESlasher*>(pArg);
	switch (eSlasher) {
	case CInfo::ESlasher::HUNTRESS:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Huntress"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Weapon"), reinterpret_cast<CComponent*&>(m_pTransform_Weapon));
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Weapon_Slash"), TEXT("Model_Weapon"), reinterpret_cast<CComponent*&>(m_pModel_Weapon));
		m_pModel->Set_Anim(31, 0.f);
		break;
	case CInfo::ESlasher::SPIRIT:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Spirit"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim(38, 0.f);
		break;
	}

	return hr;
}

const _ubyte CEnding_Slasher::Update(const _double& dTimeDelta) {
	m_pModel->Update_Anim(dTimeDelta, true);

	if (nullptr != m_pModel_Weapon) {
		_matrix BoneMatrix = m_pModel->Get_BoneMatrix("joint_WeaponRT_01") * m_pTransform->Get_Matrix();
		m_pTransform_Weapon->Set_Row(CTransform::ERow::RIGHT, -BoneMatrix.r[0]);
		m_pTransform_Weapon->Set_Row(CTransform::ERow::UP, BoneMatrix.r[1]);
		m_pTransform_Weapon->Set_Row(CTransform::ERow::LOOK, -BoneMatrix.r[2]);
		m_pTransform_Weapon->Set_Row(CTransform::ERow::POS, BoneMatrix.r[3]);
	}
	return OBJ_NOEVENT;
}

void CEnding_Slasher::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CEnding_Slasher::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
}

const HRESULT CEnding_Slasher::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		pCamera->Set_RawValue(m_pShader_Weapon);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
				m_pModel->Render(i, m_pShader, 0);
			else
				m_pModel->Render(i, m_pShader, 7);
		}

		if (nullptr != m_pModel_Weapon) {
			m_pShader_Weapon->Set_RawValue("g_WorldMatrix", &m_pTransform_Weapon->Get_Float4x4_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel_Weapon->Get_NumMesh(); ++i) {
				m_pModel_Weapon->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader_Weapon, "g_DiffuseTexture");
				if (FAILED(m_pModel_Weapon->Set_Resource(i, aiTextureType_NORMALS, m_pShader_Weapon, "g_NormalTexture")))
					m_pModel_Weapon->Render(i, m_pShader_Weapon, 0);
				else
					m_pModel_Weapon->Render(i, m_pShader_Weapon, 8);
			}
		}
		break;
	}
										 break;
	case CRenderer::EOrder::SHADOW: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CShadow* pShadow = dynamic_cast<CShadow*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow")));
		pShadow->Set_RawValue(m_pShader);
		pShadow->Set_RawValue(m_pShader_Weapon);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			m_pModel->Render(i, m_pShader, 1);
		}

		if (nullptr != m_pModel_Weapon) {
			m_pShader_Weapon->Set_RawValue("g_WorldMatrix", &m_pTransform_Weapon->Get_Float4x4_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel_Weapon->Get_NumMesh(); ++i) {
				m_pModel_Weapon->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader_Weapon, "g_DiffuseTexture");
				m_pModel_Weapon->Render(i, m_pShader_Weapon, 3);
			}
		}
	}
								  break;
	}
	return S_OK;
}
