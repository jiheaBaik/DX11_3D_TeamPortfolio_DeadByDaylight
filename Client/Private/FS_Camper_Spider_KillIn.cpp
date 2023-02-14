#include "pch.h"
#include "FS_Camper_Spider_KillIn.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Hook.h"
#include "Effect_CamperHookDeath.h"
#include "Effect_Blood_Free.h"
#include "UI_Play_Icon_SkillCheck.h"

CFS_Camper_Spider_KillIn::CFS_Camper_Spider_KillIn(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Spider_KillIn::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Spider_KillIn::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_Active(false);
	}

	CEffect_CamperHookDeath* pEffect = dynamic_cast<CEffect_CamperHookDeath*>(CGame_Instance::Get_Instance()->CreateGet_Object_Clone(TEXT("Effect_CamperHookDeath"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_CamperHookDeath")));
	CTransform* pTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
	pTransform->Set_Row(CTransform::ERow::POS, dynamic_cast<CTransform*>(m_pHook->Get_Component(TEXT("Transform")))->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 25.f, 0.f, 0.f) + 3.5f * dynamic_cast<CTransform*>(m_pHook->Get_Component(TEXT("Transform")))->Get_Row(CTransform::ERow::LOOK));
	return S_OK;
}

const _ubyte  CFS_Camper_Spider_KillIn::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(20)) {
		_matrix vEffectPos;
		vEffectPos = XMLoadFloat4x4(&m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char")) * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_TorsoB_01") * (m_pCamper_GamePlay->m_pTransform->Get_Matrix());
		CEffect_Blood_Free* pBloodEffect = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
		CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
		pTransform->Set_Row(CTransform::ERow::POS, vEffectPos.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_blood_splash_screen"), 4, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
	}

	if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(55)) {
		_matrix vEffectPos;
		vEffectPos = XMLoadFloat4x4(&m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char")) * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_TorsoB_01") * (m_pCamper_GamePlay->m_pTransform->Get_Matrix());
		CEffect_Blood_Free* pBloodEffect = dynamic_cast<CEffect_Blood_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
		CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
		pTransform->Set_Row(CTransform::ERow::POS, vEffectPos.r[3] + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_blood_splash_screen"), 4, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
	}

	if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(17))
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("TheEntity_Spider_Attack_Bounce"), 6, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

	if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(52))
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("TheEntity_Spider_Attack_Bounce"), 6, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

	if (m_pCamper_GamePlay->m_bControl) {

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			return CFM_Camper::EState::SPIDER_KILLLOOP;
		}

		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(20)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 0.7f, FMOD_2D);
				break;
			}
		}
			
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(55)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 0.7f, FMOD_2D);
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 0.7f, FMOD_2D);
				break;
			}
		}
			

		
	}
	else
	{

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_BeingKilledBySpiderLoop");
		}

		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(20)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			}
		}
			
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(55)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_grunt_land_hard"), 10, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_grunt_land_hard"), 7, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_grunt_land_hard"), 8, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_grunt_land_hard"), 7, -1, 0.7f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			}
		}
			
		
	}
	return CFM_Camper::EState::END;
}
