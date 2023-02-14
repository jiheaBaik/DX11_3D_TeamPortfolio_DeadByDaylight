#include "pch.h"
#include "FS_Camper_HookOut.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_HookOut::CFS_Camper_HookOut(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HookOut::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookOut::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	if (m_pCamper_GamePlay->m_bControl) {
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_long"), 5, -1, 0.2f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_long"), 6, -1, 0.2f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_long"), 8, -1, 0.2f, FMOD_2D);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_long"), 5, -1, 0.2f, FMOD_2D);
			break;
		}

		//CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		//dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, -1);
	}
		
	else {
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_long"), 5, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_long"), 6, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_long"), 8, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_long"), 5, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
			break;
		}
	}
		
	
	return S_OK;
}

const _ubyte  CFS_Camper_HookOut::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (nullptr != m_pHook) {
		CModel* pHookModel = dynamic_cast<CModel*>(m_pHook->Get_Component(TEXT("Model_Hook")));
		CTransform* pHookTransform = dynamic_cast<CTransform*>(m_pHook->Get_Component(TEXT("Transform")));
		_matrix SocketMatrix = pHookModel->Get_BoneMatrix("joint_SurvivorHook") * pHookTransform->Get_Matrix();
		SocketMatrix.r[0] *= -1.f;
		SocketMatrix.r[2] *= -1.f;

		_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
		_vector vLerpPos = SocketMatrix.r[3] * 0.2f + vPos * 0.8f;
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

		_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
		_vector vLockerQaut = XMQuaternionRotationMatrix(SocketMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vLockerQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		m_pCamper_GamePlay->m_pTransform->Set_Matrix(SocketMatrix);
	}

	if (m_pCamper_GamePlay->m_bControl) {

		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_SPACE)) {
			m_pHook->Set_State(CInter_Hook::ESTATE::TRY_ESCAPE);
			return CFM_Camper::EState::HOOKSTRUGGLE;
		}

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			if (m_pCamper_GamePlay->m_fSpiderRound > 1)
				m_pHook->Set_State(CInter_Hook::ESTATE::IDLE2);
			else
				m_pHook->Set_State(CInter_Hook::ESTATE::IDLE);

			return CFM_Camper::EState::HOOKIDLE;
		}
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_HookedIdle01");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_HookedIdle01");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_HookedIdle01");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_HookedIdle01");
				break;
			}
		}
	}

	return CFM_Camper::EState::END;
}