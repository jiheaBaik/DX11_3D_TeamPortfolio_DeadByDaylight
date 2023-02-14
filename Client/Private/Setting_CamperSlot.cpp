#include "pch.h"
#include "Setting_CamperSlot.h"
#include "Effect_Camper_Lobby.h"

CSetting_CamperSlot* const CSetting_CamperSlot::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CSetting_CamperSlot* pInstnace = new CSetting_CamperSlot(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CSetting_CamperSlot::Clone(void* const& pArg) {
	CSetting_CamperSlot* pInstnace = new CSetting_CamperSlot(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CSetting_CamperSlot::CSetting_CamperSlot(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CSetting_CamperSlot::CSetting_CamperSlot(const CSetting_CamperSlot& rhs)
	: CObject(rhs) {
}

void CSetting_CamperSlot::Delete(void) {
	__super::Delete();
	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
}

const HRESULT CSetting_CamperSlot::Init_Create(void) {
	return S_OK;
}

const HRESULT CSetting_CamperSlot::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	m_pCollider->Set_Extents(_float3{ 0.1f, 0.1f, 0.1f });

	m_pEffect[0] = dynamic_cast<CEffect_Camper_Lobby*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_LobbyOtherPlayerZone"), static_cast<_ubyte>(EScene::LOBBY), TEXT("Effect_LobbyOtherPlayerZone")));
	m_pEffect[1] = dynamic_cast<CEffect_Camper_Lobby*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_LobbyOtherPlayerZoneBack"), static_cast<_ubyte>(EScene::LOBBY), TEXT("Effect_LobbyOtherPlayerZoneBack")));
	m_pEffect[2] = dynamic_cast<CEffect_Camper_Lobby*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_LobbyOtherPlayerZoneParticle0"), static_cast<_ubyte>(EScene::LOBBY), TEXT("Effect_LobbyOtherPlayerZoneParticle0")));
	m_pEffect[3] = dynamic_cast<CEffect_Camper_Lobby*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_LobbyOtherPlayerZoneParticle1"), static_cast<_ubyte>(EScene::LOBBY), TEXT("Effect_LobbyOtherPlayerZoneParticle2")));

	return hr;
}

const _ubyte CSetting_CamperSlot::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	Update_Network();
	return OBJ_NOEVENT;
}

void CSetting_CamperSlot::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CSetting_CamperSlot::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CSetting_CamperSlot::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CSetting_CamperSlot::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CSetting_CamperSlot::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
	_vector vPos = m_pTransform->Get_Row(CTransform::ERow::POS);
	for (_uint i = 0; i < 4; ++i) {
		CTransform* pEffectTransform = dynamic_cast<CTransform*>(m_pEffect[i]->Get_Component(TEXT("Transform")));
		if (i == 0) {
			pEffectTransform->Rotation(pEffectTransform->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(90.f));
			XMVectorSetY(vPos, 0.01f);
		}
		pEffectTransform->Set_Row(CTransform::ERow::POS, vPos);

		if(i < 2)
			pEffectTransform->Set_Scale(_float3{ 1.f,1.f,1.f });
		else
			pEffectTransform->Set_Scale(_float3{ 0.5f,0.5f,0.5f });
	}
}

void CSetting_CamperSlot::Set_EffectRender(const _bool bRender) {
	m_bRender = bRender;
	for (_uint i = 0; i < 4; ++i) {
		m_pEffect[i]->Set_Render(m_bRender);
	}
}