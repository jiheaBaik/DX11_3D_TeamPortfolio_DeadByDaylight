#include "Renderer.h"
#include "Shader.h"
#include "Buffer_Rect.h"
#include "Object.h"
#include "Target_Mgr.h"
#include "Target.h"
#include "Camera.h"
#include "Light.h"
#include "UI.h"
#include "Effect.h"
#include "Game_Instance.h"
#include "Target.h"
#include "Compute.h"
#include "Fragment.h"

CRenderer* const CRenderer::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext) {
	CRenderer* pInstance = new CRenderer(pDevice, pContext);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CRenderer::Clone(void* const& pArg) {
	AddRef();
	return this;
}

CRenderer::CRenderer(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext),
	m_pTarget_Mgr(CTarget_Mgr::Get_Instance()),
	m_pFragment(CFragment::Get_Instance()),
	m_pCompute(CCompute::Get_Instance()) {
	Safe_AddRef(m_pTarget_Mgr);
	Safe_AddRef(m_pCompute);
	Safe_AddRef(m_pFragment);
}

void CRenderer::Delete(void) {
	__super::Delete();
	Safe_Release(m_pFragment);
	Safe_Release(m_pCompute);
	Safe_Release(m_pTarget_Mgr);
}

const HRESULT CRenderer::Init_Create(void) {
	if (nullptr == m_pTarget_Mgr)
		return E_FAIL;
	return S_OK;
}

const HRESULT CRenderer::Render(void) {
	if (nullptr == m_pTarget_Mgr)
		return E_FAIL;

	if (!m_listObject[static_cast<_uint>(EOrder::ENVIRO)].empty()) {
		for (auto& iter : m_listObject[static_cast<_uint>(EOrder::ENVIRO)]) {
			iter->Render(EOrder::ENVIRO);
			Safe_Release(iter);
		}
		m_listObject[static_cast<_uint>(EOrder::ENVIRO)].clear();
	}

	HRESULT hr = S_OK;
	hr |= m_pTarget_Mgr->End_MultiTarget();
	m_pTarget_Mgr->Get_Target(TEXT("Back0"))->Clear_RenderTargetView();
	m_pTarget_Mgr->Get_Target(TEXT("Fog1"))->Clear_RenderTargetView();
	m_pTarget_Mgr->Clear_DepthStencilView(TEXT("DepthStencilView_Back"));
	m_pTarget_Mgr->Clear_DepthStencilView(TEXT("DepthStencilView_Shadow"));
	m_pTarget_Mgr->Clear_DepthStencilView(TEXT("DepthStencilView_Shadow_Bake"));

	hr |= Render_Object(EOrder::PRIORITY);
	hr |= Render_Object(EOrder::NONALPHABLEND);
	hr |= Render_Object(EOrder::SHADOW);
	if (true == m_bBake) {
		m_bBake = false;
		hr |= Render_Object(EOrder::SHADOW_BAKE);
	}
	hr |= Render_Object(EOrder::DECAL);

	hr |= m_pFragment->SSAO();
	hr |= m_pCompute->SSAO();
	hr |= m_pCompute->Light();
	hr |= m_pCompute->Blend();
	hr |= m_pCompute->Shadow();
	hr |= m_pCompute->Back();

	hr |= Render_Object(EOrder::GLOW);
	hr |= Render_Object(EOrder::DISTORTION);
	hr |= Render_Object(EOrder::NONLIGHT);
	hr |= Render_Object(EOrder::GODRAY);

	hr |= m_pCompute->Glow();
	hr |= m_pCompute->Distortion();
	//hr |= m_pCompute->DOF();
	hr |= m_pFragment->GodRay();

	hr |= m_pCompute->Fog0();
	hr |= m_pCompute->Fog1();

	hr |= Render_Object(EOrder::ALPHABLEND);

	hr |= m_pCompute->HDR();
	hr |= m_pCompute->Back1();
	hr |= m_pCompute->Bloom();

	hr |= Render_Object(EOrder::PENETRATE);
	hr |= m_pCompute->Back2();

	//hr |= m_pCompute->MotionBlur();
	hr |= Render_Object(EOrder::EFFECT_UI);	//Sample
	hr |= Render_Object(EOrder::UI);
	hr |= m_pCompute->LumaSharpen();
	hr |= m_pCompute->HSBC();
	hr |= m_pFragment->Defferd();
	return hr;
}

const HRESULT CRenderer::Render_Enviro(void) {
	if (nullptr == m_pTarget_Mgr)
		return E_FAIL;
	HRESULT hr = S_OK;
	hr |= m_pTarget_Mgr->End_MultiTarget();
	m_pTarget_Mgr->Get_Target(TEXT("Back0_Enviro"))->Clear_RenderTargetView();
	m_pTarget_Mgr->Get_Target(TEXT("Fog1_Enviro"))->Clear_RenderTargetView();
	m_pTarget_Mgr->Clear_DepthStencilView(TEXT("DepthStencilView_Back_Enviro"));
	m_pTarget_Mgr->Clear_DepthStencilView(TEXT("DepthStencilView_Shadow_Enviro"));
	m_pTarget_Mgr->Clear_DepthStencilView(TEXT("DepthStencilView_Shadow_Bake_Enviro"));

	hr |= Render_Enviro(EOrder::PRIORITY);
	hr |= Render_Enviro(EOrder::NONALPHABLEND);
	hr |= Render_Enviro(EOrder::SHADOW);
	if (true == m_bBake) {
		hr |= Render_Enviro(EOrder::SHADOW_BAKE);
	}
	hr |= Render_Enviro(EOrder::DECAL);

	hr |= m_pFragment->SSAO_Enviro();
	hr |= m_pCompute->SSAO_Enviro();
	hr |= m_pCompute->Light_Enviro();
	hr |= m_pCompute->Blend_Enviro();
	hr |= m_pCompute->Shadow_Enviro();
	hr |= m_pCompute->Back_Enviro();

	hr |= Render_Enviro(EOrder::GLOW);
	hr |= Render_Enviro(EOrder::DISTORTION);
	hr |= Render_Enviro(EOrder::NONLIGHT);
	hr |= Render_Enviro(EOrder::GODRAY);

	hr |= m_pCompute->Glow_Enviro();
	hr |= m_pCompute->Distortion_Enviro();
	hr |= m_pFragment->GodRay_Enviro();

	hr |= m_pCompute->Fog0_Enviro();
	hr |= m_pCompute->Fog1_Enviro();

	hr |= Render_Enviro(EOrder::ALPHABLEND);

	hr |= m_pCompute->HDR_Enviro();
	hr |= m_pCompute->Back1_Enviro();
	hr |= m_pCompute->Bloom_Enviro();

	hr |= Render_Enviro(EOrder::PENETRATE);
	hr |= m_pCompute->Back2_Enviro();
	return hr;
}

const HRESULT CRenderer::Add_Object(const EOrder& eOrder, CObject* const& pObject) {
	if (eOrder == EOrder::SHADOW_BAKE && m_bBake == false)
		return S_OK;
	if (nullptr == pObject)
		return E_FAIL;
	m_listObject[static_cast<_ubyte>(eOrder)].emplace_back(pObject);
	Safe_AddRef(pObject);
	return S_OK;
}

const HRESULT CRenderer::Render_Object(const EOrder& eOrder) {
	HRESULT hr = S_OK;
	switch (eOrder) {
	default:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Back0"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), false);
		break;
	case EOrder::PRIORITY:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);
		break;
	case EOrder::NONALPHABLEND:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), false);
		break;
	case EOrder::DECAL:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred2"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), false);
		break;
	case EOrder::SHADOW:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Shadow"), TEXT("DepthStencilView_Shadow"), TEXT("Viewport_Shadow"), true);
		break;
	case EOrder::SHADOW_BAKE:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Shadow_Bake"), TEXT("DepthStencilView_Shadow_Bake"), TEXT("Viewport_Shadow_Bake"), true);
		break;
	case EOrder::DISTORTION:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Distortion"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);
		break;
	case EOrder::GLOW:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Glow"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);
		break;
	case EOrder::NONLIGHT:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred3"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), false);
		break;
	case EOrder::GODRAY:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("GodRay"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);
		break;
	case EOrder::ALPHABLEND: {
		_vector vCamerapos = {0.f,0.f,0.f,0.f};
		if(CCamera::Get_Camera(static_cast<_uint>(0)) != nullptr)
			vCamerapos = CCamera::Get_Camera(static_cast<_uint>(0))->Get_Pos();
		m_listObject[static_cast<_ubyte>(eOrder)].sort([&](CObject* pSour, CObject* pDest) {
			CTransform* pSourTransform = reinterpret_cast<CEffect*>(pSour)->Get_EffectTransform();
			CTransform* pDestTransform = reinterpret_cast<CEffect*>(pDest)->Get_EffectTransform();
			if (nullptr == pSourTransform)
				pSourTransform = dynamic_cast<CTransform*>(pSour->Get_Component(TEXT("Transform")));
			if (nullptr == pDestTransform)
				pDestTransform = dynamic_cast<CTransform*>(pDest->Get_Component(TEXT("Transform")));
			//CTransform* pSourTransform = dynamic_cast<CTransform*>(pSour->Get_Component(TEXT("Transform"))); //reinterpret_cast<CEffect*>(pSour)->Get_EffectTransform();
			//CTransform* pDestTransform = dynamic_cast<CTransform*>(pDest->Get_Component(TEXT("Transform"))); //reinterpret_cast<CEffect*>(pDest)->Get_EffectTransform();
			_float vSourLength = pSourTransform->Get_Dist(vCamerapos);
			_float vDestLength = pDestTransform->Get_Dist(vCamerapos);
			return vSourLength > vDestLength;
		});
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Fog1"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), false);
		break;
	}
	case EOrder::PENETRATE:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Penetrate"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), true);
		break;
	case EOrder::UI:
		m_listObject[static_cast<_ubyte>(eOrder)].sort([&](CObject* pSour, CObject* pDest) {
			return reinterpret_cast<CUI*>(pSour)->Get_Depth() < reinterpret_cast<CUI*>(pDest)->Get_Depth();
			});
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Back0"), TEXT("DepthStencilView_Back"), TEXT("Viewport_Back"), false);
		break;
	}

	for (auto& iter : m_listObject[static_cast<_ubyte>(eOrder)]) {
		if (nullptr != iter)
			hr |= iter->Render(eOrder);
		Safe_Release(iter);
	}
	m_listObject[static_cast<_ubyte>(eOrder)].clear();

	hr |= m_pTarget_Mgr->End_MultiTarget();
	return hr;
}

const HRESULT CRenderer::Render_Enviro(const EOrder& eOrder) {
	HRESULT hr = S_OK;
	switch (eOrder) {
	default:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Back0_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), false);
		break;
	case EOrder::PRIORITY:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), true);
		break;
	case EOrder::NONALPHABLEND:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), false);
		break;
	case EOrder::DECAL:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred2_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), false);
		break;
	case EOrder::SHADOW:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Shadow_Enviro"), TEXT("DepthStencilView_Shadow_Enviro"), TEXT("Viewport_Shadow_Enviro"), true);
		break;
	case EOrder::SHADOW_BAKE:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Shadow_Bake_Enviro"), TEXT("DepthStencilView_Shadow_Bake_Enviro"), TEXT("Viewport_Shadow_Bake_Enviro"), true);
		break;
	case EOrder::DISTORTION:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Distortion_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), true);
		break;
	case EOrder::GLOW:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Glow_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), true);
		break;
	case EOrder::NONLIGHT:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Deferred3_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), false);
		break;
	case EOrder::GODRAY:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("GodRay_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), true);
		break;
	case EOrder::ALPHABLEND:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Fog1_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), false);
		break;
	case EOrder::PENETRATE:
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Penetrate_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), true);
		break;
	case EOrder::UI:
		m_listObject[static_cast<_ubyte>(eOrder)].sort([&](CObject* pSour, CObject* pDest) {
			return reinterpret_cast<CUI*>(pSour)->Get_Depth() < reinterpret_cast<CUI*>(pDest)->Get_Depth();
			});
		hr |= m_pTarget_Mgr->Begin_MultiTarget(TEXT("Back0_Enviro"), TEXT("DepthStencilView_Back_Enviro"), TEXT("Viewport_Back_Enviro"), false);
		break;
	}

	for (auto& iter : m_listObject[static_cast<_ubyte>(eOrder)]) {
		if (nullptr != iter)
			hr |= iter->Render(eOrder);
	}

	hr |= m_pTarget_Mgr->End_MultiTarget();
	return hr;
}

void CRenderer::Set_Bake(const _bool& bBake) {
	m_bBake = bBake;
}
