#include "pch.h"
#include "Ending_Camper.h"
#include "Shadow.h"

CEnding_Camper* const CEnding_Camper::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEnding_Camper* pInstnace = new CEnding_Camper(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEnding_Camper::Clone(void* const& pArg) {
	CEnding_Camper* pInstnace = new CEnding_Camper(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEnding_Camper::CEnding_Camper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEnding_Camper::CEnding_Camper(const CEnding_Camper& rhs)
	: CObject(rhs) {
}

void CEnding_Camper::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
}

const HRESULT CEnding_Camper::Init_Create(void) {
	return S_OK;
}

const HRESULT CEnding_Camper::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelAnim"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));

	CInfo::ECamper eCamper = *reinterpret_cast<CInfo::ECamper*>(pArg);
	switch (eCamper) {
	case CInfo::ECamper::CHERYL:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Cheryl"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandRun", 0.f);
		break;
	case CInfo::ECamper::BILL:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Bill"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_StandRun", 0.f);
		break;
	case CInfo::ECamper::DWIGHT:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Dwight"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_StandRun", 0.f);
		break;
	case CInfo::ECamper::MEG:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Meg"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_StandRun", 0.f);
		break;
	}
	return hr;
}

const _ubyte CEnding_Camper::Update(const _double& dTimeDelta) {
	m_pModel->Update_Anim(dTimeDelta, true);
	return OBJ_NOEVENT;
}

void CEnding_Camper::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CEnding_Camper::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
}

const HRESULT CEnding_Camper::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
		m_pShader->Set_RawValue("g_fRefract", &m_fRefract, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));

		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
				m_pModel->Render(i, m_pShader, 0);
			else
				m_pModel->Render(i, m_pShader, 7);
		}
		break;
	}
	case CRenderer::EOrder::SHADOW: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CShadow* pShadow = dynamic_cast<CShadow*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow")));
		pShadow->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			m_pModel->Render(i, m_pShader, 1);
		}
		break;
	}
	}
	return S_OK;
}
