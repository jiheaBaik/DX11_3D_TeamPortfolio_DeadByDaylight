#include "pch.h"
#include "Cube_Free.h"

CCube_Free* const CCube_Free::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCube_Free* pInstnace = new CCube_Free(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCube_Free::Clone(void* const& pArg) {
	CCube_Free* pInstnace = new CCube_Free(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCube_Free::CCube_Free(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CCube_Free::CCube_Free(const CCube_Free& rhs)
	: CObject(rhs),
	m_fColor(rhs.m_fColor) {
}

void CCube_Free::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDiffuseTex);
	Safe_Release(m_pBuffer);
}

const HRESULT CCube_Free::Init_Create(void) {
	m_fColor = _float4{ 0.45f, 0.f, 0.f, 1.f };
	return S_OK;
}

const HRESULT CCube_Free::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_TexCube"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_footstepsCube"), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pDiffuseTex));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Cube"), TEXT("Buffer"), reinterpret_cast<CComponent*&>(m_pBuffer));

	m_pTarget_World = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("World"));
	m_pTarget_Normal = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Normal"));
	m_dDurationTime = 0.;
	m_fGlow = { 1.f,1.f };
	m_pTransform->Set_Scale(XMVectorSet(4.f,4.f,4.f,0.f));
	return hr;
}

const _ubyte CCube_Free::Update(const _double& dTimeDelta) {
	m_dDurationTime += dTimeDelta;
	if (m_dDurationTime >= 0. && m_dDurationTime < 4.) {
		_float fRatio = static_cast<_float>(m_dDurationTime) / 2.f;
		if (fRatio > 1.f)
			fRatio = 1.f;
		m_fColor.w = fRatio;
	}
	if (m_dDurationTime >= 4.) {
		_float fRatio = (static_cast<_float>(m_dDurationTime) - 4.f) / 2.f;
		if (fRatio > 1.f)
			fRatio = 1.f;

		m_fColor.w = (1 - fRatio);
		if (m_fColor.w <= 0.f)
			return OBJ_ERASE;	
	}
	return OBJ_NOEVENT;
}

void CCube_Free::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CCube_Free::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::DECAL, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::GLOW, this);
}

const HRESULT CCube_Free::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::DECAL: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_InvWorldMatrix", &XMMatrixTranspose(m_pTransform->Get_Matrix_Inverse()), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_fColor", &m_fColor, sizeof(_float4));
		_float2 fWinSize = _float2{ static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY) };
		m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
		m_pShader->Set_RawValue("g_fColor", &m_fColor, sizeof(_float4));
		m_pTarget_World->Set_Resource(m_pShader, "g_WorldTexture");
		m_pTarget_Normal->Set_Resource(m_pShader, "g_NormalTexture");
		m_pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);

		m_pBuffer->Render(m_pShader, 1);
		break;
	}
	case CRenderer::EOrder::GLOW: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_InvWorldMatrix", &XMMatrixTranspose(m_pTransform->Get_Matrix_Inverse()), sizeof(_float4x4));
		_float2 fWinSize = _float2{ static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY) };
		m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
		m_pShader->Set_RawValue("g_fColor", &m_fColor, sizeof(_float4));
		m_pShader->Set_RawValue("g_fGlow_Buffer", &m_fGlow, sizeof(_float2));
		m_pTarget_World->Set_Resource(m_pShader, "g_WorldTexture");
		m_pTarget_Normal->Set_Resource(m_pShader, "g_NormalTexture");
		m_pDiffuseTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);

		m_pBuffer->Render(m_pShader, 1);
		break;
	}
	}
	return S_OK;
}

void CCube_Free::Set_Pos(_fvector pos){
	m_pTransform->Set_Row(CTransform::ERow::POS, pos);
}

const _float CCube_Free::Get_Dist(){
	return m_pTransform->Get_Scale().x / 2;
}
