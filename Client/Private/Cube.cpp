#include "pch.h"
#include "Cube.h"

CCube* const CCube::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCube* pInstnace = new CCube(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCube::Clone(void* const& pArg) {
	CCube* pInstnace = new CCube(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCube::CCube(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CCube::CCube(const CCube& rhs)
	: CObject(rhs) {
}

void CCube::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDiffuseTex);
	Safe_Release(m_pBuffer);
}

const HRESULT CCube::Init_Create(void) {
	return S_OK;
}

const HRESULT CCube::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_TexCube"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_numcube"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pDiffuseTex));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Cube"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));
	return hr;
}

const _ubyte CCube::Update(const _double& dTimeDelta) {
	return OBJ_NOEVENT;
}

void CCube::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	ImGui::InputText("Diffuse", m_szSearchKey, IM_ARRAYSIZE(m_szSearchKey)); ImGui::SameLine();
	if (ImGui::Button("Change_dif")) {
		if (m_pDiffuseTex != nullptr) {
			Safe_Release(m_pDiffuseTex);
			string str;
			wstring wstr;
			str = m_szSearchKey;
			wstr.assign(str.begin(), str.end());
			wstr = TEXT("Texture_") + wstr;
			strcpy_s(m_szDiffuse, m_szSearchKey);
			m_pDiffuseTex = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), wstr.c_str(), nullptr));
		}
	}
	ImGui::DragFloat4("Color", (_float*)&m_fColor, 0.05f);
	const char* iTexture[] = { "Pass_Cube", "Pass_SSD" };
	_int pass = static_cast<_int>(m_iShaderPass);
	ImGui::Combo("TexturePass", &pass, iTexture, IM_ARRAYSIZE(iTexture));
	m_iShaderPass = static_cast<_uint>(pass);
}

void CCube::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::DECAL, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::GLOW, this);
}

const HRESULT CCube::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::DECAL: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_InvWorldMatrix", &XMMatrixTranspose(m_pTransform->Get_Matrix_Inverse()), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_fColor", &m_fColor, sizeof(_float4));
		CTarget_Mgr::Get_Instance()->Get_Target(TEXT("World"))->Set_Resource(m_pShader, "g_WorldTexture");
		CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Normal"))->Set_Resource(m_pShader, "g_NormalTexture");
		m_pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);

		m_pBuffer->Render(m_pShader, m_iShaderPass);
		break;
	}
	case CRenderer::EOrder::GLOW: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_InvWorldMatrix", &XMMatrixTranspose(m_pTransform->Get_Matrix_Inverse()), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_fColor", &m_fColor, sizeof(_float4));
		CTarget_Mgr::Get_Instance()->Get_Target(TEXT("World"))->Set_Resource(m_pShader, "g_WorldTexture");
		CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Normal"))->Set_Resource(m_pShader, "g_NormalTexture");
		m_pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);

		m_pBuffer->Render(m_pShader, m_iShaderPass);
		break;
	}
	}
	return S_OK;
}
