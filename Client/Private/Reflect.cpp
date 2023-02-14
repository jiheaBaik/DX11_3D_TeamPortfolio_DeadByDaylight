#include "Reflect.h"
#include "pch.h"
#include "Reflect.h"
#include "Sky.h"

CReflect* const CReflect::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CReflect* pInstnace = new CReflect(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CReflect::Clone(void* const& pArg) {
	CReflect* pInstnace = new CReflect(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CReflect::CReflect(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CEnviro(pDevice, pContext, pNetwork) {
}

CReflect::CReflect(const CReflect& rhs)
	: CEnviro(rhs) {
}

void CReflect::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
	Safe_Release(m_pShader);
	Safe_Release(m_pBuffer);
	for (_uint i = 0; i < 6; ++i) {
		Safe_Release(m_pSRV[i]);
		Safe_Release(m_pTexture[i]);
	}
	Safe_Release(m_pTextureCube);
	Safe_Release(m_pCubeSRV);
}

const HRESULT CReflect::Init_Create(void) {
	return S_OK;
}

const HRESULT CReflect::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_TexCube"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Cube"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));

	for (_uint i = 0; i < 6; ++i) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CCamera::SDesc sCamera;
		ZeroMemory(&sCamera, sizeof(CCamera::SDesc));
		sCamera.fFovy = XMConvertToRadians(90.f);
		sCamera.fAspect = 512.f / 512;
		sCamera.fNear = 0.2f;
		sCamera.fFar = 500.f;
		m_pCamera[i] = dynamic_cast<CCamera*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Camera_Reflect"), pGame_Instance->Get_LoadingScene(), TEXT("Camera_Reflect"), &sCamera));
	}

	CTransform* pCameraTransform0 = dynamic_cast<CTransform*>(m_pCamera[0]->Get_Component(TEXT("Transform")));
	CTransform* pCameraTransform1 = dynamic_cast<CTransform*>(m_pCamera[1]->Get_Component(TEXT("Transform")));
	CTransform* pCameraTransform2 = dynamic_cast<CTransform*>(m_pCamera[2]->Get_Component(TEXT("Transform")));
	CTransform* pCameraTransform3 = dynamic_cast<CTransform*>(m_pCamera[3]->Get_Component(TEXT("Transform")));
	CTransform* pCameraTransform4 = dynamic_cast<CTransform*>(m_pCamera[4]->Get_Component(TEXT("Transform")));
	CTransform* pCameraTransform5 = dynamic_cast<CTransform*>(m_pCamera[5]->Get_Component(TEXT("Transform")));
	pCameraTransform0->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	pCameraTransform1->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	pCameraTransform2->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	pCameraTransform3->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	pCameraTransform4->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(0.f));
	pCameraTransform5->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	return hr;
}

const _ubyte CReflect::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CReflect::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	float my_tex_w = 300.f;
	float my_tex_h = 300.f;
	for (_uint i = 0; i < 6; ++i)
		ImGui::Image(m_pSRV[i], ImVec2(my_tex_w, my_tex_h), ImVec2{ 0.f, 0.f }, ImVec2{ 1.f, 1.f }, ImVec4{ 1.f, 1.f, 1.f, 1.f }, ImVec4{ 1.f, 1.f, 1.f, 0.5f });

}

void CReflect::Late_Update(const _double& dTimeDelta) {
	//m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
	if (!m_bRender) {
		m_bRender = true;
		m_pRenderer->Add_Object(CRenderer::EOrder::ENVIRO, this);
	}
}

const HRESULT CReflect::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::ENVIRO: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		for (_uint i = 0; i < 6; ++i) {
			m_pCamera[i]->Set_Camera(static_cast<_uint>(0));
			CTransform* pCameraTransform = dynamic_cast<CTransform*>(m_pCamera[i]->Get_Component(TEXT("Transform")));
			pCameraTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS));
			m_pCamera[i]->Set_ViewMatrix();

			CSky* pSky = dynamic_cast<CSky*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Sky")));
			pSky->Late_Update(1.f);

			m_pRenderer->Render_Enviro();

			CTarget_Mgr* pTarget_Mgr = CTarget_Mgr::Get_Instance();
			CTarget* pBack0 = pTarget_Mgr->Get_Target(TEXT("Back0_Enviro"));
			ID3D11Texture2D* pBack0Texture = pBack0->Get_Texture();

			D3D11_TEXTURE2D_DESC pBack0Desc = pBack0->Get_Texture2d_Desc();
			D3D11_TEXTURE2D_DESC Desc;
			ZeroMemory(&Desc, sizeof(D3D11_TEXTURE2D_DESC));
			Desc.Width = pBack0Desc.Width;
			Desc.Height = pBack0Desc.Height;
			Desc.MipLevels = pBack0Desc.MipLevels;
			Desc.ArraySize = pBack0Desc.ArraySize;
			Desc.Format = pBack0Desc.Format;
			Desc.SampleDesc = pBack0Desc.SampleDesc;
			Desc.Usage = D3D11_USAGE_STAGING;
			Desc.BindFlags = 0;
			Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			Desc.MiscFlags = 0;
			m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pTexture[i]);
			m_pContext->CopyResource(m_pTexture[i], pBack0Texture);

			//DirectX::SaveDDSTextureToFile(m_pContext, pBack0Texture, TEXT("../Bin/Test.dds"));
			//CreateDDSTextureFromFile(m_pDevice, TEXT("../Bin/Test.dds"), nullptr, &m_pSRV[i]);
		}

		D3D11_TEXTURE2D_DESC texDesc1 = {};
		D3D11_TEXTURE2D_DESC texDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};

		m_pTexture[0]->GetDesc(&texDesc1);

		texDesc.Width = 512;
		texDesc.Height = 512;
		texDesc.MipLevels = texDesc1.MipLevels;
		texDesc.ArraySize = 6;
		texDesc.Format = texDesc1.Format;
		texDesc.CPUAccessFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		SMViewDesc.Format = texDesc.Format;
		SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
		SMViewDesc.TextureCube.MostDetailedMip = 0;

		m_pDevice->CreateTexture2D(&texDesc, NULL, &m_pTextureCube);
		for (_int i = 0; i < 6; i++) {
			for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel) {
				D3D11_MAPPED_SUBRESOURCE mappedTex2D;
				m_pContext->Map(m_pTexture[i], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
				m_pContext->UpdateSubresource(m_pTextureCube, D3D11CalcSubresource(mipLevel, i, texDesc.MipLevels), 0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);
				m_pContext->Unmap(m_pTexture[i], mipLevel);
			}
		}
		m_pDevice->CreateShaderResourceView(m_pTextureCube, &SMViewDesc, &m_pCubeSRV);

		pCamera->Set_Camera(static_cast<_uint>(0));
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pCollider->Render(pCamera);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pShader->Set_Resource("g_DiffuseTexture", m_pCubeSRV);
		m_pBuffer->Render(m_pShader, 0);
		break;
	}
	}
	return S_OK;
}

const HRESULT CReflect::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CReflect::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
}
