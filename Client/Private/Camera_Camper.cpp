#include "pch.h"
#include "Camera_Camper.h"
#include "Camper_GamePlay.h"
#include "Slasher_Huntress.h"
#include "UI_Image.h"

CCamera_Camper* const CCamera_Camper::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCamera_Camper* pInstnace = new CCamera_Camper(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCamera_Camper::Clone(void* const& pArg) {
	CCamera_Camper* pInstnace = new CCamera_Camper(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCamera_Camper::CCamera_Camper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CCamera(pDevice, pContext, pNetwork) {
}

CCamera_Camper::CCamera_Camper(const CCamera_Camper& rhs)
	: CCamera(rhs) {
}

void CCamera_Camper::Delete(void) {
	__super::Delete();
	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform_Orbit);
	Safe_Release(m_pNavi);
	Safe_Release(m_pAudio);

	Safe_Release(m_pTransform_Ending_Sour);
	Safe_Release(m_pTransform_Ending_Dest);
}

const HRESULT CCamera_Camper::Init_Create(void) {
	return S_OK;
}

const HRESULT CCamera_Camper::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Rot"), reinterpret_cast<CComponent*&>(m_pTransform_Orbit));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));

	m_pTransform_Orbit->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-270));
	s_vecCamera.emplace_back(this);

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Ending_Sour"), reinterpret_cast<CComponent*&>(m_pTransform_Ending_Sour));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Ending_Dest"), reinterpret_cast<CComponent*&>(m_pTransform_Ending_Dest));
	m_pTransform_Ending_Dest->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f), 1.f);
	m_pTransform_Ending_Dest->Rotate(m_pTransform_Ending_Dest->Get_Row(CTransform::ERow::RIGHT), XMConvertToRadians(10.f), 1.f);
	return hr;
}

const _ubyte CCamera_Camper::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_CloneControl(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay")));
	if (nullptr == pCamper)
		return OBJ_NOEVENT;

	//#ifdef _DEBUG
	//	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_X)) {
	//		CObject* pEditCam = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"Camera_Edit");
	//		pGame_Instance->Delete_Object_Clone(pEditCam);
	//		CUI_Image* pTemp = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), L"UI_Play_Panel_Second"));
	//		if (!pTemp->Get_Active())
	//			pTemp->Set_Active(true);
	//
	//		m_pTransform_Orbit->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
	//		m_bisIntroFlag = true;
	//		eCamState = ECamState::PLAY;
	//	}
	//#endif // _DEBUG

	if (s_vecCamera.front() == this)
		m_pAudio->Set_Listener(m_pTransform, dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform"))));

	switch (eCamState) {
	case ECamState::INTRO: {
		if (!m_bisIntroFlag) {
			m_fdecTime += static_cast<_float>(dTimeDelta);

			if (m_fdecTime < 5.f) {

			}
			else if (m_fdecTime < 10.f) {
				if (m_bIntroBgm) {
					m_pAudio->Play_Sound(TEXT("am_t03_farm_intro"), -1, 0.5f, FMOD_2D);
					m_bIntroBgm = false;
				}
				m_pTransform_Orbit->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f / 5.f), dTimeDelta);
				if (m_fdecTime > 9.f)
					m_bAlmostFin = true;
			}
			else {
				m_bisIntroFlag = true;
				eCamState = ECamState::PLAY;
			}

			CTransform* pTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
			_vector vPos = pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0, 1.2f, 0, 1);
			m_pTransform_Orbit->Set_Row((CTransform::ERow::POS), vPos);
			_vector vGoal = m_pTransform_Orbit->Get_Row(CTransform::ERow::POS) - m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK) * m_fDist;
			m_pTransform->Set_Row(CTransform::ERow::POS, vGoal);

			m_pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f);
			m_pTransform->LookAt(m_pTransform_Orbit->Get_Row(CTransform::ERow::POS)/* + XMVectorSet(0.f, 0.8f, 0.f, 0.f)*/);
		}
	}
						 break;
	case Client::CCamera_Camper::ECamState::PLAY: {
		if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::FLASHLIGHT)
			return OBJ_NOEVENT;

		if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::SPIDER_IN ||
			pCamper->m_pFM->Get_State() == CFM_Camper::EState::SPIDER_KILLIN)
			eCamState = ECamState::HOOK_EVENT;


		if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::HOOKIN)
			m_faddY = 0.5f;
		if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::HIT_IDLE)
			m_faddY = 0.f;

		CTransform* pTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
		_vector vPlayerPos = pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0, 1.2f, 0, 1);
		_float3 fPlayerPos;	XMStoreFloat3(&fPlayerPos, vPlayerPos);
		m_pTransform_Orbit->Set_Row((CTransform::ERow::POS), vPlayerPos + XMVectorSet(0.f, m_faddY, 0.f, 1.f));

		_long MouseMove = 0;
		if ((MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X)) && m_bisIntroFlag)
			m_pTransform_Orbit->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * 0.07f, dTimeDelta);
		if ((MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::Y)) && m_bisIntroFlag) {
			_vector vLook = m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK);
			vLook = XMVectorSet(abs(XMVectorGetX(vLook)), XMVectorGetY(vLook), abs(XMVectorGetZ(vLook)), 0.f);
			_vector vLookY = vLook * XMVectorSet(1.f, 0.f, 1.f, 0.f);
			_float fDot = XMVectorGetX(XMVector3Dot(vLook, vLookY));
			_float fCross = XMVectorGetX(XMVector3Cross(vLookY, vLook));

			if (MouseMove < 0.f) {
				if (fCross < 0.f) {
					if (fDot > 0.6f) {
						m_pTransform_Orbit->Rotate(m_pTransform_Orbit->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.1f, dTimeDelta);
					}
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
			//m_pTransform_Orbit->Rotate(m_pTransform_Orbit->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.1f, dTimeDelta);
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
		m_pTransform->Set_Row(CTransform::ERow::POS, (m_pTransform->Get_Row(CTransform::ERow::POS)) * 0.2f + vGoal * 0.8f);

		m_pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f);
		m_pTransform->LookAt(m_pTransform_Orbit->Get_Row(CTransform::ERow::POS));

		break;
	}
	case Client::CCamera_Camper::ECamState::HOOK_EVENT: {
		if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::SPIDER_STRUGGLE)
			eCamState = ECamState::PLAY;

		m_pTransform->Set_Row(CTransform::ERow::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
		m_pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransform->Set_Row(CTransform::ERow::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		CModel* pCamperModel = dynamic_cast<CModel*>(pCamper->Get_Component(TEXT("Model")));
		CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
		_matrix CurMatrix = pCamperModel->Get_BoneMatrix("joint_Cam_03") * pCamperTransform->Get_Matrix();
		_vector VPos = XMVectorLerp(CurMatrix.r[3], m_pTransform->Get_Row(CTransform::ERow::POS), 0.8f);
		_matrix LookMatrix = pCamperModel->Get_BoneMatrix("joint_TorsoC_01") * pCamperTransform->Get_Matrix();
		m_pTransform->Set_Row((CTransform::ERow::POS), VPos);
		m_pTransform->LookAt(LookMatrix.r[3]);
	}
													  break;

	case Client::CCamera_Camper::ECamState::ENDING_EXIT: {
		CTransform* pTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
		_vector vPlayerPos = pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0, 1.2f, 0, 1);
		_float3 fPlayerPos;	XMStoreFloat3(&fPlayerPos, vPlayerPos);
		m_pTransform_Orbit->Set_Row((CTransform::ERow::POS), vPlayerPos + XMVectorSet(0.f, m_faddY, 0.f, 1.f));

		m_fTimeEnding += static_cast<_float>(dTimeDelta);
		_float fRatio = m_fTimeEnding / 3.f;
		if (fRatio > 1.f)
			fRatio = 1.f;
		_vector SourQuat = XMQuaternionRotationMatrix(m_pTransform_Ending_Sour->Get_Matrix());
		_vector DestQuat = XMQuaternionRotationMatrix(m_pTransform_Ending_Dest->Get_Matrix());
		_vector ResultQuat = XMQuaternionSlerp(SourQuat, DestQuat, fRatio);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(ResultQuat);
		m_pTransform_Orbit->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pTransform_Orbit->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pTransform_Orbit->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

		_vector vLook = XMVector3Normalize(m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK));
		_float3 fLook; XMStoreFloat3(&fLook, vLook);

		_float fDist = m_fDist;

		_vector vSour = m_pTransform_Orbit->Get_Row(CTransform::ERow::POS);
		_vector vDest = m_pTransform_Orbit->Get_Row(CTransform::ERow::POS) - m_pTransform_Orbit->Get_Row(CTransform::ERow::LOOK) * fDist;
		_float3 vSour2; XMStoreFloat3(&vSour2, vSour);
		_float3 vDest2; XMStoreFloat3(&vDest2, vDest);

		for (auto& iter : m_pNavi->Get_DivCell(vSour2, vDest2)) {
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
	}
													   break;
	case Client::CCamera_Camper::ECamState::MORI:
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
		CTransform* pSlasherTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));
		CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
		CModel* pCamperModel = dynamic_cast<CModel*>(pCamper->Get_Component(TEXT("Model")));
		_matrix CamperBoneMatrix = pCamperModel->Get_BoneMatrix("joint_Cam_03");
		_matrix SocketMatrix;
		SocketMatrix = CamperBoneMatrix * pCamperTransform->Get_Matrix();

		m_pTransform->Set_Row(CTransform::ERow::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
		m_pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		m_pTransform->Set_Row(CTransform::ERow::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		if (pSlasher->Get_Slasher())
			m_pTransform->LookAt(pSlasherTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
		else
			m_pTransform->LookAt(pSlasherTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);
		break;
	}

	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CCamera_Camper::Update_Edit(void) {
}

void CCamera_Camper::Late_Update(const _double& dTimeDelta) {
	Set_ViewMatrix();
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CCamera_Camper::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//	m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}
