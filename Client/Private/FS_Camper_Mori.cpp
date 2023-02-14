#include "pch.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "FS_Camper_Mori.h"
#include "Camera_Camper.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Panel_Manager.h"
#include "UI_Play_Gauge_Inter.h"
#include "Effect_Blood_Free.h"
#include "Scene_GamePlay.h"
#include "Setting_BGM.h"

CFS_Camper_Mori::CFS_Camper_Mori(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Mori::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Mori::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		static_cast<CCamera_Camper*>(pCamera)->Set_Estate(CCamera_Camper::ECamState::MORI);
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), m_pCamper_GamePlay->m_iTarObjID);
		m_pSlasher = static_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pTarObject);
		
		m_pCamper_GamePlay->Set_End(true);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress"), m_pCamper_GamePlay->m_iTarObjID);
		m_pSlasher = static_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pTarObject);

		CScene_GamePlay* pScene = dynamic_cast<CScene_GamePlay*>(pGame_Instance->Get_Scene2());
		pScene->Plus_CamperKillCount();
	}

	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	return S_OK;
}

const _ubyte  CFS_Camper_Mori::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	CTransform* pSlasherTransform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
	CModel* pHuntressModel = dynamic_cast<CModel*>(m_pSlasher->Get_Component(TEXT("Model")));
	_matrix CurSlasherMatrix = pHuntressModel->Get_BoneMatrix("joint_Char") * pSlasherTransform->Get_Matrix();

	// blood effect
	// 65 �����ѹ�(joint_TorsoB_01) �Ӹ��ѹ�(joint_FacialGroup), �Ӹ��� ��� (joint_FacialGroup)
	// 163 �Ӹ��� �޸���, ������
	// 235 ������ ����Ƣ�� ��
	// 265 ������ ~ 316 ���� ������ ��� ��������
	// 316 ����������
	_matrix vEffectPos1, vEffectPos2;

	//if (!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_BE_Mori01") ||
	//	!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_BE_Mori01") ||
	//	!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|M_BE_Mori01") ||
	//	!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_BE_Mori01")) {
	//}
	if (m_pSlasher != nullptr) {
		if (m_pSlasher->Get_Slasher() == true) {
			vEffectPos1 = XMLoadFloat4x4(&m_fRootOffsetMatrix) * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_TorsoB_01") * (m_pCamper_GamePlay->m_pTransform->Get_Matrix());
			vEffectPos2 = XMLoadFloat4x4(&m_fRootOffsetMatrix) * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_FacialGroup") * (m_pCamper_GamePlay->m_pTransform->Get_Matrix());
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(61)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos1.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos2.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(162)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos1.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos2.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(234)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos1.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos2.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(264)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos1.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos2.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(315)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos1.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Row(CTransform::ERow::POS, vEffectPos2.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
		}

		else {
			//95 �ڿ��� Į�� - �ڷ� 2��f
			//100 ���� �ѹ��� �Ǻм� - ������ 2��
			//135���� �� ������
			//181 ���� ��
			//225 �� ��
			//246 �� ��
			vEffectPos1 = XMLoadFloat4x4(&m_fRootOffsetMatrix) * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_TorsoC_01") * (m_pCamper_GamePlay->m_pTransform->Get_Matrix());
			vEffectPos2 = XMLoadFloat4x4(&m_fRootOffsetMatrix) * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_TorsoB_01") * (m_pCamper_GamePlay->m_pTransform->Get_Matrix());
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(95)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos1);
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos1);
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(100)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos1);
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos1);
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(135)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos1);
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos1);
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(181)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos2);
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos2);
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(225)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos2);
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos2);
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(246)) {
				CEffect_Blood_Free* pBloodEffect1 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect1->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos2);
				CEffect_Blood_Free* pBloodEffect2 = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
				pTransform = dynamic_cast<CTransform*>(pBloodEffect2->Get_Component(TEXT("Transform")));
				pTransform->Set_Matrix(vEffectPos2);
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("Blood_Splash_Bounce"), 10, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
		}
	}

	if (!m_bMoriEnd) {
		_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
		_vector vLerpPos = CurSlasherMatrix.r[3] * 0.2f + vPos * 0.8f;
		m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);
		_vector vLOOK = pSlasherTransform->Get_Row(CTransform::ERow::LOOK) * 1.f;
		vLOOK = XMVectorSetY(vLOOK, 0.f);
		m_pCamper_GamePlay->m_pTransform->LookAt(vPos + vLOOK);
	}
	

	if (m_pCamper_GamePlay->m_bControl) {

		
		if (m_pSlasher->Get_Slasher() == true) {
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(50)) {

				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.8f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.8f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.8f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.8f, FMOD_2D);
					break;
				}
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(80)) {
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_survivor_punch"), 2, -1, 1.f, FMOD_2D);
			}
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(85)) {
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_survivor_punch"), 2, -1, 1.f, FMOD_2D);
			}
		}
		else {
			if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(94)) {
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.8f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 7, -1, 0.8f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 8, -1, 0.8f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 6, -1, 0.8f, FMOD_2D);
					break;
				}
			}
		}

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_bMoriEnd = true;

			CCamera_Camper* pCameraCamper = dynamic_cast<CCamera_Camper*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::GAMEPLAY), TEXT("Camera_Camper")));
			pCameraCamper->Set_Estate(CCamera_Camper::ECamState::ENDING_EXIT);

			/*CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
			static_cast<CCamera_Camper*>(pCamera)->Set_Estate(CCamera_Camper::ECamState::PLAY);*/

			if (false == m_bCheck) {
				m_bCheck = true;
				CInfo* pInfo = CInfo::Get_Instance();
				pInfo->Add_CamperCount(-1);

				// Mori Ending UI On
				CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
				dynamic_cast<CUI_End_Panel_First*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_End_Panel_First"))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ECamper::SACRIFICE), true);
			}
			CUI_Play_Panel_Manager* pPanelMgr = dynamic_cast<CUI_Play_Panel_Manager*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Panel_Manager"));
			
			m_dEndingTime += dTimeDelta;
			if (m_dEndingTime < 3.) {
				dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Active(false);
			}
			else if (m_dEndingTime < 6.) {
				if (false == m_bUI_FadeIn) {
					m_bUI_FadeIn = true;
					pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::FADE, true, CUI_Play_Panel_Manager::EPanel::PLAY2);
				}
			}
			else if (m_dEndingTime < 9.) {
				if (false == m_bUI_FadeOut) {
					m_bUI_FadeOut = true;
					CCamera* pCamera = CCamera::Get_Camera(TEXT("Ending"));
					pCamera->Set_Camera(0);
					m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(10.f, 0.f, 10.f, 1.f));
					pPanelMgr->Get_Panel(CUI_Play_Panel_Manager::EPanel::FADE)->Set_Render(false);
				}
			}
			else if (m_dEndingTime < 12.) {
				if (false == m_bUI_First) {
					m_bUI_First = true;
					m_pCamper_GamePlay->m_bPlayBGM = false;
					m_pCamper_GamePlay->m_pAudio->Play_Sound(TEXT("mu_menu_defeated"), -1, 0.05f, FMOD_2D);
					pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END1, true);
				}
			}
			else if (m_dEndingTime < 15.) {
				if (false == m_bUI_Second) {
					m_bUI_Second = true;
					CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Setting_BGM")));
					pBGM->Play_BGM(TEXT("mu_menu_tallyscreen"), 0.05f, 2.f);
					pPanelMgr->Set_Panel(CUI_Play_Panel_Manager::EPanel::END2, true, CUI_Play_Panel_Manager::EPanel::END);
				}
			}
		}
	}
	else {
	if (m_pSlasher->Get_Slasher() == true) {
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(50)) {

			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			}
		}
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(80)) {
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_survivor_punch"), 2, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(85)) {
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_survivor_punch"), 2, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}
	}
	else {
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(94)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_scream_hit"), 10, -1, 0.8f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
				break;
			}
		}
	}
	if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
		
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(10.f, 0.f, 10.f, 0.f));
		}
	}
	return CFM_Camper::EState::END;
}
