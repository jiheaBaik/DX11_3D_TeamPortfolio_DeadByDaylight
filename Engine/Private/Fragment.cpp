#include "Fragment.h"
#include "Target_Mgr.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Buffer_Rect.h"
#include "Texture.h"
#include "Game_Instance.h"

IMPLEMENT_SINGLETON(CFragment)

CFragment::CFragment(void)
	: CBase() {
	ZeroMemory(&m_WorldMatrix, sizeof(_float4x4));
	ZeroMemory(&m_ViewMatrix, sizeof(_float4x4));
	ZeroMemory(&m_ProjMatrix, sizeof(_float4x4));
}

void CFragment::Delete(void) {
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader_Light);
	Safe_Release(m_pShader_GodRay);
	Safe_Release(m_pShader_SSAO);
	Safe_Release(m_pShader_Deferred);
	Safe_Release(m_pTarget_Mgr);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const HRESULT CFragment::Init(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);
	m_pContext = pContext;
	Safe_AddRef(m_pContext);
	m_pTarget_Mgr = CTarget_Mgr::Get_Instance();
	Safe_AddRef(m_pTarget_Mgr);

	m_pShader_Deferred = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Deferred.hlsl"), VTXTEX_DECLARATION::Input_Element_Desc, VTXTEX_DECLARATION::iNumElement);
	m_pShader_SSAO = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/SSAO.hlsl"), VTXTEX_DECLARATION::Input_Element_Desc, VTXTEX_DECLARATION::iNumElement);
	m_pShader_GodRay = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/GodRay.hlsl"), VTXTEX_DECLARATION::Input_Element_Desc, VTXTEX_DECLARATION::iNumElement);
	m_pShader_Light = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shader/Light1.hlsl"), VTXTEX_DECLARATION::Input_Element_Desc, VTXTEX_DECLARATION::iNumElement);

	m_pBack0 = m_pTarget_Mgr->Get_Target(TEXT("Back0"));
	m_pWorld = m_pTarget_Mgr->Get_Target(TEXT("World"));
	m_pNormal = m_pTarget_Mgr->Get_Target(TEXT("Normal"));
	m_pGodRay = m_pTarget_Mgr->Get_Target(TEXT("GodRay"));
	m_pLumaSharpen = m_pTarget_Mgr->Get_Target(TEXT("LumaSharpen"));
	m_pHSBC = m_pTarget_Mgr->Get_Target(TEXT("HSBC"));
	m_pBuffer = CBuffer_Rect::Create(m_pDevice, m_pContext);

	D3D11_VIEWPORT ViewPort;
	_uint iNumViewport = 1;
	m_pContext->RSGetViewports(&iNumViewport, &ViewPort);
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewPort.Width;
	m_WorldMatrix._22 = ViewPort.Height;
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f)));

	XMStoreFloat4x4(&m_WorldMatrix_Enviro, XMMatrixIdentity());
	m_WorldMatrix_Enviro._11 = 512;
	m_WorldMatrix_Enviro._22 = 512;
	XMStoreFloat4x4(&m_WorldMatrix_Enviro, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix_Enviro)));
	XMStoreFloat4x4(&m_ViewMatrix_Enviro, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix_Enviro, XMMatrixTranspose(XMMatrixOrthographicLH(512, 512, 0.f, 1.f)));
	return S_OK;
}

void CFragment::Update_Edit(const _int& iPushID) {
	ImGui::PushID(iPushID);
	ImGui::Separator();
	ImGui::Text("Fragment");

	ImGui::Text("GodRay");
	ImGui::DragFloat4("LightShaftValue", (float*)&m_vLightShaftValue, 0.001f);

	ImGui::Text("SSAO");
	ImGui::DragFloat("Sample_rad", &m_fSample_rad, 0.001f);
	ImGui::DragFloat("Intensity", &m_fIntensity, 0.001f);
	ImGui::DragFloat("Scale", &m_fScale, 0.001f);
	ImGui::DragFloat("Bias", &m_fBias, 0.001f);

	ImGui::Separator();
	ImGui::PopID();
}

const HRESULT CFragment::Defferd(void) {
	HRESULT hr = S_OK;
	hr |= m_pShader_Deferred->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));
	hr |= m_pShader_Deferred->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	hr |= m_pShader_Deferred->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	hr |=  m_pHSBC->Set_Resource(m_pShader_Deferred, "g_BackTexture");

	m_pBuffer->Render(m_pShader_Deferred, 0);
	return S_OK;
}

const HRESULT CFragment::SSAO(void) {
	HRESULT hr = S_OK;
	hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("SSAO"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);

	hr |= m_pShader_SSAO->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));
	hr |= m_pShader_SSAO->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	hr |= m_pShader_SSAO->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	hr |= m_pShader_SSAO->Set_RawValue("g_sample_rad", &m_fSample_rad, sizeof(_float));
	hr |= m_pShader_SSAO->Set_RawValue("g_intensity", &m_fIntensity, sizeof(_float));
	hr |= m_pShader_SSAO->Set_RawValue("g_scale", &m_fScale, sizeof(_float));
	hr |= m_pShader_SSAO->Set_RawValue("g_bias", &m_fBias, sizeof(_float));

	hr |= m_pWorld->Set_Resource(m_pShader_SSAO, "g_WorldTexture");
	hr |= m_pNormal->Set_Resource(m_pShader_SSAO, "g_NormalTexture");
	CTexture* pRandom = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->Get_Component_Prototype(0, TEXT("Texture_Random")));
	ID3D11ShaderResourceView* pRandomSRV = pRandom->Get_ResourceView()[0];
	m_pShader_SSAO->Set_Resource("g_RandomTexture", pRandomSRV);
	m_pBuffer->Render(m_pShader_SSAO, 0);

	hr |= m_pTarget_Mgr->End_MultiTarget();
	return hr;
}

const HRESULT CFragment::GodRay(void) {
	HRESULT hr = S_OK;
	hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("GodRay0"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);
	hr |= m_pShader_GodRay->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));
	hr |= m_pShader_GodRay->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	hr |= m_pShader_GodRay->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	hr |= m_pShader_GodRay->Set_RawValue("g_vLightShaftValue", &m_vLightShaftValue, sizeof(_float4));

	hr |=m_pGodRay->Set_Resource(m_pShader_GodRay, "g_BackTexture");

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera) {
		_matrix WorldMatrix = XMMatrixInverse(nullptr, pCamera->Get_ViewMatrix());
		_matrix ViewMatrix = pCamera->Get_ViewMatrix();
		_matrix ProjMatrix = pCamera->Get_ProjMatrix();
	
		_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_float fDist = 9999.f;
		for (auto& iter : m_listGodRayPos) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			if (pGame_Instance->IsIn_Frustum_World(XMLoadFloat3(&iter), 0.1f)) {
				_vector vPos2 = XMVector3TransformCoord(XMLoadFloat3(&iter), ViewMatrix);
				vPos2 = XMVector3TransformCoord(vPos2, ProjMatrix);

				_float fDist2 = XMVectorGetX(XMVector3Length(WorldMatrix.r[3] - XMLoadFloat3(&iter)));
				if (fDist > fDist2) {
					vPos = vPos2;
					fDist = fDist2;
				}
			}
		}

		_float3 vPos2; XMStoreFloat3(&vPos2, vPos);
		vPos2.x = vPos2.x * 0.5f + 0.5f;
		vPos2.y = 1.f - (vPos2.y * 0.5f + 0.5f);

		m_pShader_GodRay->Set_RawValue("g_vScreenLightPos", &vPos2, sizeof(_float2));
		m_pBuffer->Render(m_pShader_GodRay, 0);
	}
	m_listGodRayPos.clear();
	hr |= m_pTarget_Mgr->End_MultiTarget();
	return hr;
}

const HRESULT CFragment::Light(void) {
	HRESULT hr = S_OK;
	hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("LightAcc"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);
	hr |= m_pShader_Light->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));
	hr |= m_pShader_Light->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	hr |= m_pShader_Light->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	hr |= m_pTarget_Mgr->Get_Target(TEXT("Normal"))->Set_Resource(m_pShader_Light, "g_NormalTexture");
	hr |= m_pTarget_Mgr->Get_Target(TEXT("World"))->Set_Resource(m_pShader_Light, "g_WorldTexture");
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera) {
		hr |= m_pShader_Light->Set_RawValue("g_vCameraPos", &pCamera->Get_Pos(), sizeof(_float3));
		vector<CLight*> vecLight = CLight::Get_VecLight();
		for (auto& iter : vecLight) {
			if (false == iter->Get_OnOff())
				continue;
			iter->Set_RawValue(m_pShader_Light);
			switch (iter->Get_Type()) {
			case CLight::EType::DIR:
				m_pBuffer->Render(m_pShader_Light, 0);
				break;
			case CLight::EType::POINT:
				m_pBuffer->Render(m_pShader_Light, 1);
				break;
			case CLight::EType::SPOT:
				m_pBuffer->Render(m_pShader_Light, 2);
				break;
			}
		}
		hr |= m_pTarget_Mgr->End_MultiTarget();
	}
	else
		hr |= m_pTarget_Mgr->End_MultiTarget();
	return hr;
}

const HRESULT CFragment::Defferd_Enviro(void) {
	HRESULT hr = S_OK;
	hr |= m_pShader_Deferred->Set_RawValue("g_WorldMatrix", &m_WorldMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pShader_Deferred->Set_RawValue("g_ViewMatrix", &m_ViewMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pShader_Deferred->Set_RawValue("g_ProjMatrix", &m_ProjMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pTarget_Mgr->Get_Target(TEXT("Back0"))->Set_Resource(m_pShader_Deferred, "g_BackTexture");
	m_pBuffer->Render(m_pShader_Deferred, 0);
	return hr;
}

const HRESULT CFragment::SSAO_Enviro(void) {
	HRESULT hr = S_OK;
	hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("SSAO_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), true);

	hr |= m_pShader_SSAO->Set_RawValue("g_WorldMatrix", &m_WorldMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pShader_SSAO->Set_RawValue("g_ViewMatrix", &m_ViewMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pShader_SSAO->Set_RawValue("g_ProjMatrix", &m_ProjMatrix_Enviro, sizeof(_float4x4));

	hr |= m_pShader_SSAO->Set_RawValue("g_sample_rad", &m_fSample_rad, sizeof(_float));
	hr |= m_pShader_SSAO->Set_RawValue("g_intensity", &m_fIntensity, sizeof(_float));
	hr |= m_pShader_SSAO->Set_RawValue("g_scale", &m_fScale, sizeof(_float));
	hr |= m_pShader_SSAO->Set_RawValue("g_bias", &m_fBias, sizeof(_float));

	hr |= m_pTarget_Mgr->Get_Target(TEXT("World_Enviro"))->Set_Resource(m_pShader_SSAO, "g_WorldTexture");
	hr |= m_pTarget_Mgr->Get_Target(TEXT("Normal_Enviro"))->Set_Resource(m_pShader_SSAO, "g_NormalTexture");
	CTexture* pRandom = dynamic_cast<CTexture*>(CGame_Instance::Get_Instance()->Get_Component_Prototype(0, TEXT("Texture_Random")));
	ID3D11ShaderResourceView* pRandomSRV = pRandom->Get_ResourceView()[0];
	m_pShader_SSAO->Set_Resource("g_RandomTexture", pRandomSRV);
	m_pBuffer->Render(m_pShader_SSAO, 0);

	hr |= m_pTarget_Mgr->End_MultiTarget();
	return hr;
}

const HRESULT CFragment::GodRay_Enviro(void) {
	HRESULT hr = S_OK;
	hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("GodRay0_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), true);
	hr |= m_pShader_GodRay->Set_RawValue("g_WorldMatrix", &m_WorldMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pShader_GodRay->Set_RawValue("g_ViewMatrix", &m_ViewMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pShader_GodRay->Set_RawValue("g_ProjMatrix", &m_ProjMatrix_Enviro, sizeof(_float4x4));
	hr |= m_pShader_GodRay->Set_RawValue("g_vLightShaftValue", &m_vLightShaftValue, sizeof(_float4));

	hr |= m_pTarget_Mgr->Get_Target(TEXT("GodRay_Enviro"))->Set_Resource(m_pShader_GodRay, "g_BackTexture");

	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	if (nullptr != pCamera) {
		_matrix WorldMatrix = XMMatrixInverse(nullptr, pCamera->Get_ViewMatrix());
		_matrix ViewMatrix = pCamera->Get_ViewMatrix();
		_matrix ProjMatrix = pCamera->Get_ProjMatrix();

		_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_float fDist = 9999.f;
		for (auto& iter : m_listGodRayPos) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			if (pGame_Instance->IsIn_Frustum_World(XMLoadFloat3(&iter), 0.1f)) {
				_vector vPos2 = XMVector3TransformCoord(XMLoadFloat3(&iter), ViewMatrix);
				vPos2 = XMVector3TransformCoord(vPos2, ProjMatrix);

				_float fDist2 = XMVectorGetX(XMVector3Length(WorldMatrix.r[3] - XMLoadFloat3(&iter)));
				if (fDist > fDist2) {
					vPos = vPos2;
					fDist = fDist2;
				}
			}
		}

		_float3 vPos2; XMStoreFloat3(&vPos2, vPos);
		vPos2.x = vPos2.x * 0.5f + 0.5f;
		vPos2.y = 1.f - (vPos2.y * 0.5f + 0.5f);

		m_pShader_GodRay->Set_RawValue("g_vScreenLightPos", &vPos2, sizeof(_float2));
		m_pBuffer->Render(m_pShader_GodRay, 0);
	}
	m_listGodRayPos.clear();
	hr |= m_pTarget_Mgr->End_MultiTarget();
	return hr;
}

void CFragment::Set_GodRayPos(const _float3& vGodRayPos) {
	m_listGodRayPos.emplace_back(vGodRayPos);
}

void CFragment::Set_GodRayPos(_fvector vGodRayPos) {
	_float3 vPos; XMStoreFloat3(&vPos, vGodRayPos);
	m_listGodRayPos.emplace_back(vPos);
}
