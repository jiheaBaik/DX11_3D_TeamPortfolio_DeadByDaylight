#include "pch.h"
#include "Terrain.h"

CTerrain* const CTerrain::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CTerrain* pInstnace = new CTerrain(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CTerrain::Clone(void* const& pArg) {
	CTerrain* pInstnace = new CTerrain(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CTerrain::CTerrain(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CObject(rhs) {
}

void CTerrain::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexture[0]);
	Safe_Release(m_pTexture[1]);
	Safe_Release(m_pTexture[2]);
	Safe_Release(m_pTexture[3]);
	Safe_Release(m_pTexture[4]);
	Safe_Release(m_pTexture_Normal[0]);
	Safe_Release(m_pTexture_Normal[1]);
	Safe_Release(m_pTexture_Normal[2]);
	Safe_Release(m_pTexture_Normal[3]);
	Safe_Release(m_pTexture_Normal[4]);
}

const HRESULT CTerrain::Init_Create(void) {
	return S_OK;
}

const HRESULT CTerrain::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_TexNor"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Terrain"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GroundCornFieldMud_A_BC_2"), TEXT("Texture0"), reinterpret_cast<CComponent*&>(m_pTexture[0]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GroundCornField_A_BC"), TEXT("Texture1"), reinterpret_cast<CComponent*&>(m_pTexture[1]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GrassDirt_A_BC"), TEXT("Texture2"), reinterpret_cast<CComponent*&>(m_pTexture[2]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GroundMud_A_BC"), TEXT("Texture3"), reinterpret_cast<CComponent*&>(m_pTexture[3]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_PavementSquareTiles_A_BC"), TEXT("Texture4"), reinterpret_cast<CComponent*&>(m_pTexture[4]));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GroundCornFieldMud_A_N"), TEXT("Texture_Normal0"), reinterpret_cast<CComponent*&>(m_pTexture_Normal[0]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GroundCornField_A_N"), TEXT("Texture_Normal1"), reinterpret_cast<CComponent*&>(m_pTexture_Normal[1]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GrassDirt01_A_N"), TEXT("Texture_Normal2"), reinterpret_cast<CComponent*&>(m_pTexture_Normal[2]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_GroundMud_A_N"), TEXT("Texture_Normal3"), reinterpret_cast<CComponent*&>(m_pTexture_Normal[3]));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_Com_PavementSquareTiles_A_N"), TEXT("Texture_Normal4"), reinterpret_cast<CComponent*&>(m_pTexture_Normal[4]));
	return hr;
}

const _ubyte CTerrain::Update(const _double& dTimeDelta) {
	return OBJ_NOEVENT;
}

void CTerrain::Update_Edit(void) {
	m_pTransform->Update_Edit(1);
	m_pBuffer->Update_Edit(0);
	ImGui::DragFloat("Roughness", &m_fRoughness, 0.001f);
	ImGui::DragFloat("Refract", &m_fRefract, 0.001f);
	ImGui::DragFloat("Metalness", &m_fMetalness, 0.001f);
	ImGui::DragFloat("Metalness2", &m_fMetalness2, 0.001f);
}

void CTerrain::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (nullptr != m_pBuffer->Get_Index())
		m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
}

const HRESULT CTerrain::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
		m_pShader->Set_RawValue("g_fRefract", &m_fRefract, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness2", &m_fMetalness2, sizeof(_float));

		m_pBuffer->Set_Resource(m_pShader, "g_BrushTexture");
		m_pTexture[0]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		m_pTexture[1]->Set_Resource(m_pShader, "g_DiffuseTexture0", 0);
		m_pTexture[2]->Set_Resource(m_pShader, "g_DiffuseTexture1", 0);
		m_pTexture[3]->Set_Resource(m_pShader, "g_DiffuseTexture2", 0);
		m_pTexture[4]->Set_Resource(m_pShader, "g_DiffuseTexture3", 0);
		m_pTexture_Normal[0]->Set_Resource(m_pShader, "g_NormalTexture", 0);
		m_pTexture_Normal[1]->Set_Resource(m_pShader, "g_NormalTexture0", 0);
		m_pTexture_Normal[2]->Set_Resource(m_pShader, "g_NormalTexture1", 0);
		m_pTexture_Normal[3]->Set_Resource(m_pShader, "g_NormalTexture2", 0);
		m_pTexture_Normal[4]->Set_Resource(m_pShader, "g_NormalTexture3", 0);
		m_pBuffer->Render(m_pShader, 0);
	}
										 break;
	}
	return S_OK;
}
const HRESULT CTerrain::Save_Data(Json::Value& root) {
	m_pBuffer->Save_Data(root["Terrain"]);
	return S_OK;
}

void CTerrain::Load_Data(Json::Value& root) {
	m_pBuffer->Load_Data(root["Terrain"]);
}