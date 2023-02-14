#include "pch.h"
#include "FS_Camper_FlashLight.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Camera_Camper.h"
CFS_Camper_FlashLight::CFS_Camper_FlashLight(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_FlashLight::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_FlashLight::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);

	if (m_pCamper_GamePlay->m_bControl) {
		m_DifFov = 0.f;
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CObject* pObj = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Camera_Camper"));
		CCamera_Camper* pCam = dynamic_cast<CCamera_Camper*>(pObj);
		CTransform* pCamTransform = dynamic_cast<CTransform*>(pCam->Get_Component(TEXT("Transform")));
		m_pCamper_GamePlay->m_pTransform->LookAt(XMVectorSetY(pCamTransform->Get_Row(CTransform::ERow::POS), XMVectorGetY(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS))));
		m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f), 1.f);
		m_bDefault = false;
		m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("sfx_flashlight_on"), -1, 0.5f, FMOD_2D);
	}
	else
		m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("sfx_flashlight_on"), -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 3.f });


	return S_OK;
}

const _ubyte  CFS_Camper_FlashLight::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl){
	
		m_DifFov += 0.8f + (m_DifFov * 0.05f);
		if (m_DifFov >= 10.f)
			m_DifFov = 10.f;

		if (!m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT]) {
			if (m_pCamper_GamePlay->m_iHP == 2)
				return CFM_Camper::EState::IDLE;
			else if (m_pCamper_GamePlay->m_iHP == 1)
				return CFM_Camper::EState::HIT_IDLE;
		}

		CObject* pObj = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Camera_Camper"));
		CCamera_Camper* pCam = dynamic_cast<CCamera_Camper*>(pObj);

		CTransform* pCamTransform = dynamic_cast<CTransform*>(pCam->Get_Component(TEXT("Transform")));
		_vector vCamPos = pCamTransform->Get_Row(CTransform::ERow::POS);

		_vector vCampPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
		_float3 fPlayerPos; XMStoreFloat3(&fPlayerPos, vCampPos);

		_float4x4 fov; XMStoreFloat4x4(&fov, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f - m_DifFov), static_cast<_float>(g_iWinSizeX) / g_iWinSizeY, 0.2f, 500.f));
		pCam->Set_Fov(fov);

		_vector vLook = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) * -1;
		_vector vRight = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::RIGHT);
		_float3 vRight2; XMStoreFloat3(&vRight2, vRight);

		if (!m_bDefault) {
			_vector VPos = vCampPos + (vRight * 0.5f + vLook * 0.5f) + XMVectorSet(0.f, 1.25f, 0.f, 1.f);
			_vector vLerpPos = VPos * 0.3f + vCamPos * 0.7f;
			pCamTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

			XMMATRIX CampMatrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
			_vector vQuat = XMQuaternionRotationMatrix(pCamTransform->Get_Matrix());
			_vector vCampQaut = XMQuaternionRotationMatrix(CampMatrix);
			_vector vResultQuat = XMQuaternionSlerp(vQuat, vCampQaut, 0.2f);
			_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);
			pCamTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
			pCamTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
			pCamTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

			_vector vDot = XMVector3Dot(XMVector3Normalize(VPos), XMVector3Normalize(vLerpPos));
			_vector vDot2 = XMVector3Dot(XMVector3Normalize(vCampQaut), XMVector3Normalize(vResultQuat)); 
			_float fDot = acos(XMVectorGetX(vDot));
			_float fDot2 = acos(XMVectorGetX(vDot2));

			if (fDot == 0.f && fDot2 == 0.f)
				m_bDefault = true;
		}
		else {
			_vector VPos = vCampPos + (vRight * 0.5f + vLook * 0.5f) + XMVectorSet(0.f, 1.25f, 0.f, 1.f);
			pCamTransform->Set_Row(CTransform::ERow::POS, VPos);
			pCamTransform->Set_Row(CTransform::ERow::RIGHT, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			pCamTransform->Set_Row(CTransform::ERow::UP, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::UP));
			pCamTransform->Set_Row(CTransform::ERow::LOOK, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK));
		}

		_long MouseMove = 0;
		if ((MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X))) {
			m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * 0.007f, dTimeDelta);
			m_pCamper_GamePlay->m_vDotDir = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::LOOK);
			m_pCamper_GamePlay->Get_FM_Camper()->Update_Network("Set_Dir");
		}

		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::R)) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CObject* pObj = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Camera_Camper"));
			CCamera_Camper* pCam = dynamic_cast<CCamera_Camper*>(pObj);
			_float4x4 pro; XMStoreFloat4x4(&pro, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), static_cast<_float>(g_iWinSizeX) / g_iWinSizeY, 0.2f, 500.f));
			pCam->Set_Fov(pro);
			CTransform* pCamTransform = dynamic_cast<CTransform*>(pCam->Get_Component(TEXT("Transform_Rot")));
			pCamTransform->Set_Row(CTransform::ERow::LOOK, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK));
			pCamTransform->Set_Row(CTransform::ERow::RIGHT, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			pCamTransform->Set_Row(CTransform::ERow::UP, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::UP));
			
			m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("sfx_flashlight_off"), -1, 0.5f, FMOD_2D);

			if (m_pCamper_GamePlay->m_iHP == 2)
				return CFM_Camper::EState::IDLE;
			else if (m_pCamper_GamePlay->m_iHP == 1)
				return CFM_Camper::EState::HIT_IDLE;
		}
	}
	else 
		m_pCamper_GamePlay->m_pTransform->LookAt(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&m_pCamper_GamePlay->m_vDotDir));

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}

