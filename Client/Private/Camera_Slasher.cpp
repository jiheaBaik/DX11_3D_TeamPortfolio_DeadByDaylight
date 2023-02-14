#include "pch.h"
#include "Camera_Slasher.h"
#include "Slasher_Huntress.h"
#include "UI_Image.h"

CCamera_Slasher* const CCamera_Slasher::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCamera_Slasher* pInstnace = new CCamera_Slasher(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCamera_Slasher::Clone(void* const& pArg) {
	CCamera_Slasher* pInstnace = new CCamera_Slasher(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCamera_Slasher::CCamera_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CCamera(pDevice, pContext, pNetwork) {
}

CCamera_Slasher::CCamera_Slasher(const CCamera_Slasher& rhs)
	: CCamera(rhs) {
}

void CCamera_Slasher::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform_Intro);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pAudio);
}

const HRESULT CCamera_Slasher::Init_Create(void) {
	return S_OK;
}

const HRESULT CCamera_Slasher::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Intro"), reinterpret_cast<CComponent*&>(m_pTransform_Intro));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	CTransform* pSlasherTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));

	s_vecCamera.emplace_back(this);
	return hr;
}

const _ubyte CCamera_Slasher::Update(const _double& dTimeDelta) {

	return OBJ_NOEVENT;
}

void CCamera_Slasher::Update_Edit(void) {

}

void CCamera_Slasher::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	CTransform* pSlasherTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));

	if (s_vecCamera.front() == this)
		m_pAudio->Set_Listener(m_pTransform, pSlasherTransform);

	if (pGame_Instance->Get_Input_KeyboardKey(DIK_X)) {
		Set_Camera(static_cast<_uint>(0));
		/*CObject* pEditCam = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"Camera_Edit");
		pGame_Instance->Delete_Object_Clone(pEditCam);


		CUI_Image* pTemp = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), L"UI_Play_Panel_Second"));
		if (!pTemp->Get_Active())
			pTemp->Set_Active(true);

		m_eCamState = ECamState::PLAY;
		m_bisIntroFlag = true;
		pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::IDLE_SLASH);

		CModel* pModel = dynamic_cast<CModel*>(pSlasher->Get_Component(TEXT("Model_FPV")));
		_matrix SlasherBoneMatrix = pModel->Get_BoneMatrix("joint_Cam_01");
		_matrix SocketMatrix;
		SocketMatrix = SlasherBoneMatrix * pSlasherTransform->Get_Matrix();

		m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
		m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
		m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
		m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);*/
	}

	switch (m_eCamState) {
	case ECamState::INTRO: {
		m_dIntroTime += dTimeDelta;
		if (m_dIntroTime < 5.0) {
			if (false == m_bIntroBGM) {
				m_pAudio->Play_Sound(TEXT("am_t03_farm_intro"), -1, 0.5f, FMOD_2D);
				m_bIntroBGM = true;
			}
			_vector vLook = XMVector3Normalize(m_pTransform_Intro->Get_Row(CTransform::ERow::LOOK)) * 2.5f;
			_vector vUp = XMVector3Normalize(m_pTransform_Intro->Get_Row(CTransform::ERow::UP)) * 2.0f;

			m_pTransform_Intro->Set_Row(CTransform::ERow::POS, pSlasherTransform->Get_Row(CTransform::ERow::POS));
			m_pTransform_Intro->LookAt(pSlasherTransform->Get_Row(CTransform::ERow::POS));

			m_pTransform->Set_Row(CTransform::ERow::POS, m_pTransform_Intro->Get_Row(CTransform::ERow::POS) + vLook + vUp);
			m_pTransform->LookAt(m_pTransform_Intro->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 1.f, 0.f, 1.f));
		}
		else if (m_dIntroTime < 11.0) {
			m_pTransform_Intro->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f / 6.), dTimeDelta);

			_vector vLook = XMVector3Normalize(m_pTransform_Intro->Get_Row(CTransform::ERow::LOOK)) * 2.5f;
			_vector vUp = XMVector3Normalize(m_pTransform_Intro->Get_Row(CTransform::ERow::UP)) * 2.0f;

			m_pTransform->Set_Row(CTransform::ERow::POS, m_pTransform_Intro->Get_Row(CTransform::ERow::POS) + vLook + vUp);
			m_pTransform->Set_Row(CTransform::ERow::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
			m_pTransform->Set_Row(CTransform::ERow::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
			m_pTransform->Set_Row(CTransform::ERow::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
			m_pTransform->LookAt(m_pTransform_Intro->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 1.f, 0.f, 1.f));

			if (m_dIntroTime > 8.f) {
				m_bAlmostFin = true;
			}
		}
		else if (m_dIntroTime < 11.4f) {
			if (false == m_bPreSet) {
				m_bPreSet = true;
				XMStoreFloat3(&m_vPreLook, m_pTransform->Get_Row(CTransform::ERow::LOOK));
				XMStoreFloat3(&m_vPrePos, m_pTransform->Get_Row(CTransform::ERow::POS));
			}
			//0~1로 바꾸는 코드
			//타임 공식 = (현재시간 - 시작시간) / (종료시간 - 시작시간) 
			_float fTime = ((_float)m_dIntroTime - 11.f) / (11.4f - 11.f);
			CModel* pModel = dynamic_cast<CModel*>(pSlasher->Get_Component(TEXT("Model_FPV")));
			_matrix SlasherBoneMatrix = pModel->Get_BoneMatrix("joint_Cam_01");
			_matrix SocketMatrix;
			SocketMatrix = SlasherBoneMatrix * pSlasherTransform->Get_Matrix();

			m_pTransform->Set_Row(CTransform::ERow::LOOK, XMVectorLerp(XMLoadFloat3(&m_vPreLook), (-SocketMatrix.r[2]), fTime));
			m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorLerp(XMLoadFloat3(&m_vPrePos), SocketMatrix.r[3], fTime));
		}
		else {
			m_eCamState = ECamState::PLAY;
			m_bisIntroFlag = true;
			pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::IDLE_SLASH);

			CModel* pModel = dynamic_cast<CModel*>(pSlasher->Get_Component(TEXT("Model_FPV")));
			_matrix SlasherBoneMatrix = pModel->Get_BoneMatrix("joint_Cam_01");
			_matrix SocketMatrix;
			SocketMatrix = SlasherBoneMatrix * pSlasherTransform->Get_Matrix();

			m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
			m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
			m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
			m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);
		}
		break;
	}
	case ECamState::PLAY: {
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
		CTransform* pSlasherTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));
		CModel* pModel = dynamic_cast<CModel*>(pSlasher->Get_Component(TEXT("Model_FPV")));
		_matrix SlasherBoneMatrix = pModel->Get_BoneMatrix("joint_Cam_01");
		_matrix SocketMatrix;
		SocketMatrix = SlasherBoneMatrix * pSlasherTransform->Get_Matrix();
		CFM_Slasher::EState eState = pSlasher->Get_State();

		_vector vLook = m_pTransform->Get_Row(CTransform::ERow::LOOK);
		vLook = XMVectorSet(abs(XMVectorGetX(vLook)), XMVectorGetY(vLook), abs(XMVectorGetZ(vLook)), 0.f);
		_vector vLookY = vLook * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vLookY));
		_float fCross = XMVectorGetX(XMVector3Cross(vLookY, vLook));

		if (CFM_Slasher::EState::PARKOUR != eState && CFM_Slasher::EState::PARKOUR_VAULT != eState && CFM_Slasher::EState::DESTROY_HATCH != eState
			&& CFM_Slasher::EState::DESTROY_GENE != eState && CFM_Slasher::EState::DESTROY_PLANK != eState && CFM_Slasher::EState::DESTROY_TOTEM != eState
			&& CFM_Slasher::EState::HOOK != eState && CFM_Slasher::EState::PICKUP_IN != eState && CFM_Slasher::EState::PICKUP != eState
			&& CFM_Slasher::EState::LOCKER_GRAB != eState && CFM_Slasher::EState::LOCKER_RELOAD != eState && CFM_Slasher::EState::LOCKER_SEARCH != eState
			&& CFM_Slasher::EState::STUN != eState && CFM_Slasher::EState::STUN_DROP_IN != eState && CFM_Slasher::EState::STUN_DROP_OUT != eState
			&& CFM_Slasher::EState::DROP != eState && CFM_Slasher::EState::MORI != eState && CFM_Slasher::EState::SPIRIT_CHARGE != eState) {
			_long MouseMove = 0;
			if (MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X)) {
				m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * 0.04f, dTimeDelta);
				pSlasherTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * 0.04f, dTimeDelta);
				//m_pTransform->Set_Row(CTransform::ERow::LOOK, pSlasherTransform->Get_Row(CTransform::ERow::LOOK));
			}
			if (MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::Y)) {
				if (MouseMove < 0.f) {
					if (fCross < 0.f) {
						if (fDot > 0.5f) {
							m_pTransform->Rotate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.07f, dTimeDelta);
						}
					}
					else
						m_pTransform->Rotate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.07f, dTimeDelta);
				}
				else {
					if (fCross > 0.f) {
						if (fDot > 0.8f)
							m_pTransform->Rotate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.07f, dTimeDelta);
					}
					else
						m_pTransform->Rotate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.07f, dTimeDelta);
				}
			}
		}
		/*m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
		m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
		m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);*/
		//SocketMatrix.r[3] = XMVectorLerp(SocketMatrix.r[3], m_pTransform->Get_Row(CTransform::ERow::POS), 0.2f
		//m_pTransform->Set_Row(CTransform::ERow::POS, vMyPos * 0.8f + XMLoadFloat3(&vPos) * 0.2f););
		//??
		m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3] * 0.8f + m_pTransform->Get_Row(CTransform::ERow::POS) * 0.2f);
		//m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);

		if (/*CFM_Slasher::EState::ATTACK_SLASH_IN == eState || CFM_Slasher::EState::ATTACK_SLASH_SWING == eState || CFM_Slasher::EState::ATTACK_SLASH_MISS == eState
			|| CFM_Slasher::EState::ATTACK_SLASH_WIPE == eState || */CFM_Slasher::EState::HOOK == eState || CFM_Slasher::EState::LOCKER_GRAB == eState
			|| CFM_Slasher::EState::LOCKER_RELOAD == eState || CFM_Slasher::EState::LOCKER_SEARCH == eState || CFM_Slasher::EState::PARKOUR == eState
			|| CFM_Slasher::EState::PARKOUR_VAULT == eState || CFM_Slasher::EState::DESTROY_GENE == eState || CFM_Slasher::EState::DESTROY_HATCH == eState
			|| CFM_Slasher::EState::DESTROY_PLANK == eState || CFM_Slasher::EState::DESTROY_TOTEM == eState || CFM_Slasher::EState::PICKUP == eState
			|| CFM_Slasher::EState::PICKUP_IN == eState || CFM_Slasher::EState::STUN == eState || CFM_Slasher::EState::DROP == eState
			|| CFM_Slasher::EState::STUN_DROP_IN == eState || CFM_Slasher::EState::STUN_DROP_OUT == eState || CFM_Slasher::EState::MORI == eState || CFM_Slasher::EState::SPIRIT_CHARGE == eState) {
			m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0] * 0.6f + m_pTransform->Get_Row(CTransform::ERow::RIGHT) * 0.4f);
			m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1] * 0.6f + m_pTransform->Get_Row(CTransform::ERow::UP) * 0.4f);
			m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2] * 0.6f + m_pTransform->Get_Row(CTransform::ERow::LOOK) * 0.4f);
			m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3] * 0.8f + m_pTransform->Get_Row(CTransform::ERow::POS) * 0.2f);
		}
		break;
	}
	case ECamState::ENDING: {

		break;
	}
	}

	Set_ViewMatrix();
	//m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CCamera_Slasher::Render(const CRenderer::EOrder& eOrder) {
	/*switch (eOrder) {
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		break;
	}
	}*/
	return S_OK;
}

const CCamera_Slasher::ECamState& CCamera_Slasher::Get_CamState(void) {
	return m_eCamState;
}
