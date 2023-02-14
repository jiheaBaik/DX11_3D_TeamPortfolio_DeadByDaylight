#include "pch.h"
#include "Ending_Spon.h"
#include "Ending_Ground.h"
#include "Ending_Grass.h"

CEnding_Spon* const CEnding_Spon::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CEnding_Spon* pInstnace = new CEnding_Spon(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CEnding_Spon::Clone(void* const& pArg) {
	CEnding_Spon* pInstnace = new CEnding_Spon(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CEnding_Spon::CEnding_Spon(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEnding_Spon::CEnding_Spon(const CEnding_Spon& rhs)
	: CObject(rhs) {
}

void CEnding_Spon::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}

const HRESULT CEnding_Spon::Init_Create(void) {
	return S_OK;
}

const HRESULT CEnding_Spon::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	return hr;
}

const _ubyte CEnding_Spon::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (nullptr == m_pGround) {
		m_pGround = dynamic_cast<CEnding_Ground*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Ending_Ground"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_Ground")));
		CTransform* pGroundTransform = dynamic_cast<CTransform*>(m_pGround->Get_Component(TEXT("Transform")));
		pGroundTransform->Set_Matrix(m_pTransform->Get_Matrix());
		m_listGround.emplace_back(m_pGround);
	}
	else {
		CTransform* pGroundTransform = dynamic_cast<CTransform*>(m_pGround->Get_Component(TEXT("Transform")));
		_float fGround_Dist = m_pTransform->Get_Dist(pGroundTransform->Get_Row(CTransform::ERow::POS));
		if (fGround_Dist > m_fGround_Dist) {
			CEnding_Ground* pGround = nullptr;
			for (auto& iter : m_listGround) {
				if (false == iter->Get_Active()) {
					pGround = iter;
					break;
				}
			}
			if (nullptr != pGround) {
				m_pGround = pGround;
				pGroundTransform = dynamic_cast<CTransform*>(m_pGround->Get_Component(TEXT("Transform")));
				pGroundTransform->Set_Matrix(m_pTransform->Get_Matrix());
				m_pGround->Set_Active(true);
			}
			else {
				m_pGround = dynamic_cast<CEnding_Ground*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Ending_Ground"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_Ground")));
				pGroundTransform = dynamic_cast<CTransform*>(m_pGround->Get_Component(TEXT("Transform")));
				pGroundTransform->Set_Matrix(m_pTransform->Get_Matrix());
				m_listGround.emplace_back(m_pGround);
			}
		}
	}
	m_fGrass_Time += static_cast<_float>(dTimeDelta);
	if (m_fGrass_Time > m_fGrass_TimeMax) {
		m_fGrass_Time = 0.f;
		CEnding_Grass* pGrass = nullptr;
		for (auto& iter : m_listGrass) {
			if (false == iter->Get_Active()) {
				pGrass = iter;
				break;
			}
		}
		if (nullptr != pGrass) {
			CTransform* pGrassTransform = dynamic_cast<CTransform*>(pGrass->Get_Component(TEXT("Transform")));
			pGrassTransform->Set_Matrix(m_pTransform->Get_Matrix());
			pGrass->Set_Active(true);
		}
		else {
			pGrass = dynamic_cast<CEnding_Grass*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Ending_Grass"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Ending_Grass")));
			CTransform* pGrassTransform = dynamic_cast<CTransform*>(pGrass->Get_Component(TEXT("Transform")));
			pGrassTransform->Set_Matrix(m_pTransform->Get_Matrix());
			m_listGrass.emplace_back(pGrass);
		}
	}
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CEnding_Spon::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	ImGui::DragFloat("Ground_Dist", &m_fGround_Dist);
}

void CEnding_Spon::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CEnding_Spon::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CEnding_Spon::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CEnding_Spon::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
}
