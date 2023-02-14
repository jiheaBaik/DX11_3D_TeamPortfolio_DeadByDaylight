#include "pch.h"
#include "Camera_Spectator.h"
#include "Camper_GamePlay.h"

CCamera_Spectator* const CCamera_Spectator::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCamera_Spectator* pInstnace = new CCamera_Spectator(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCamera_Spectator::Clone(void* const& pArg) {
	CCamera_Spectator* pInstnace = new CCamera_Spectator(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCamera_Spectator::CCamera_Spectator(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CCamera(pDevice, pContext, pNetwork) {
}

CCamera_Spectator::CCamera_Spectator(const CCamera_Spectator& rhs)
	: CCamera(rhs) {
}

void CCamera_Spectator::Delete(void) {
	__super::Delete();
	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform_Orbit);
	Safe_Release(m_pAudio);
}

const HRESULT CCamera_Spectator::Init_Create(void) {
	return S_OK;
}

const HRESULT CCamera_Spectator::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Rot"), reinterpret_cast<CComponent*&>(m_pTransform_Orbit));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));
	s_vecCamera.emplace_back(this);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_uint>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
	for (auto iter : *listCamper) {
		if (false == iter->Get_Control()) {
			m_vecCamper.emplace_back(iter);
		}
	}
	return hr;
}

const _ubyte CCamera_Spectator::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_vecCamper.empty())
		OBJ_ERASE;
	CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_vecCamper[m_iCamper]);

	m_pAudio->Set_Listener(m_pTransform, dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform"))));

	//if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::FlashLight)
	//	return OBJ_NOEVENT;
	//if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::Spider_IN ||
	//	pCamper->m_pFM->Get_State() == CFM_Camper::EState::Spider_KillIN)
	//	eCamState = ECamState::HOOK_EVENT;
	if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::HOOKIN)
		m_faddY = 0.5f;
	if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::HIT_IDLE)
		m_faddY = 0.f;

	CTransform* pTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
	_vector vPlayerPos = pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0, 1.2f, 0, 1);
	_float3 fPlayerPos;	XMStoreFloat3(&fPlayerPos, vPlayerPos);
	m_pTransform_Orbit->Set_Row((CTransform::ERow::POS), vPlayerPos + XMVectorSet(0.f, m_faddY, 0.f, 1.f));

	_long MouseMove = 0;
	if ((MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X)))
		m_pTransform_Orbit->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * 0.07f, dTimeDelta);
	if ((MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::Y))) {
		_vector vLook = m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK);
		vLook = XMVectorSet(abs(XMVectorGetX(vLook)), XMVectorGetY(vLook), abs(XMVectorGetZ(vLook)), 0.f);
		_vector vLookY = vLook * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vLookY));
		_float fCross = XMVectorGetX(XMVector3Cross(vLookY, vLook));

		if (MouseMove < 0.f) {
			if (fCross < 0.f) {
				if (fDot > 0.6f)
					m_pTransform_Orbit->Rotate(m_pTransform_Orbit->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.1f, dTimeDelta);
			}
			else
				m_pTransform_Orbit->Rotate(m_pTransform_Orbit->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.1f, dTimeDelta);
		}
		else {
			if (fCross > 0.f) {
				if (fDot > 0.6f)
					m_pTransform_Orbit->Rotate(m_pTransform_Orbit->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.1f, dTimeDelta);
			}
			else
				m_pTransform_Orbit->Rotate(m_pTransform_Orbit->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.1f, dTimeDelta);
		}
	}

	_vector vLook = XMVector3Normalize(m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK));
	_float3 fLook; XMStoreFloat3(&fLook, vLook);

	_float fDist = m_fDist;

	_vector vSour = m_pTransform_Orbit->Get_Row(CTransform::ERow::POS);
	_vector vDest = m_pTransform_Orbit->Get_Row(CTransform::ERow::POS) - m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK) * fDist;
	_float3 vSour2; XMStoreFloat3(&vSour2, vSour);
	_float3 vDest2; XMStoreFloat3(&vDest2, vDest);

	for (auto& iter : m_pNavi->Get_DivCell(vSour2, vDest2)) {
		if (CCell::EType::NOCAMERAWALL == iter->Get_Type())
			continue;
		_float3* pPoint = iter->Get_Point();
		_float fDist2 = 0.f;
		if (true == TriangleTests::Intersects(m_pTransform_Orbit->Get_Row(CTransform::ERow::POS), -m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK),
			XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2]), fDist2)) {
			if (fDist > fDist2)
				fDist = fDist2;
		}
	}

	_vector vGoal = m_pTransform_Orbit->Get_Row(CTransform::ERow::POS) - m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK) * fDist;
	m_pTransform->Set_Row(CTransform::ERow::POS, (m_pTransform->Get_Row(CTransform::ERow::POS)) * 0.f + vGoal * 1.f);

	m_pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f);
	m_pTransform->LookAt(m_pTransform_Orbit->Get_Row(CTransform::ERow::POS));

	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CCamera_Spectator::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CCamera_Spectator::Late_Update(const _double& dTimeDelta) {
	Set_ViewMatrix();
	//m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CCamera_Spectator::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

void CCamera_Spectator::Shift_Camper(const _bool& m_bLeftRight) {
	if (m_bLeftRight) {
		if (0 == m_iCamper)
			m_iCamper -= 1;
		else
			m_iCamper = static_cast<_uint>(m_vecCamper.size()) - 1;
	}
	else {
		if (static_cast<_uint>(m_vecCamper.size()) - 1 == m_iCamper)
			m_iCamper = 0;
		else
			m_iCamper += 1;
	}
}
