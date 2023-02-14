#include "pch.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Effect_Blood_Free.h"
#include "Effect_BloodStain.h"
#include "Effect_CamBlood_Free.h"
#include "Effect_FlashLight.h"
#include "Effect_Free3.h"
#include "Effect_Fog.h"
#include "Light_Flash.h"
#include "Slasher_Huntress.h"
#include "Shadow.h"
#include "Inven.h"
#include "Perk.h"
#include "Inter_PlankAnim.h"
#include "Inter_Generator.h"
#include "Setting_BGM.h"
#include "UI_Play_Panel_Second.h"

CCamper_GamePlay* const CCamper_GamePlay::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork) {
	CCamper_GamePlay* pInstnace = new CCamper_GamePlay(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCamper_GamePlay::Clone(void* const& pArg) {
	CCamper_GamePlay* pInstnace = new CCamper_GamePlay(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCamper_GamePlay::CCamper_GamePlay(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CCamper_GamePlay::CCamper_GamePlay(const CCamper_GamePlay& rhs)
	: CObject(rhs) {
}

void CCamper_GamePlay::Delete(void) {
	__super::Delete();
	Safe_Release(m_pNavi);
	Safe_Release(m_pTransform);
	Safe_Release(m_pItemTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pShader2);
	Safe_Release(m_pModel);
	Safe_Release(m_pMediKit_Model);
	Safe_Release(m_pFlashLight_Model);
	Safe_Release(m_pGrassCollider);
	Safe_Release(m_pCollider);
	Safe_Release(m_pOtherCamperCollider);
	Safe_Release(m_pFlashLightCollider);
	Safe_Release(m_pFM);
	Safe_Release(m_pCollider_Foot);
	Safe_Release(m_pCollider_Body);
	Safe_Release(m_pAudio);
}

const HRESULT CCamper_GamePlay::Init_Create(void) {
	return S_OK;
}

const HRESULT CCamper_GamePlay::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelAnim"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));

	CInfo::ECamper eCamper = *reinterpret_cast<CInfo::ECamper*>(pArg);
	switch (eCamper) {
	case CInfo::ECamper::CHERYL:
		m_eCampKind = ECamperKind::CHERYL;
		break;
	case CInfo::ECamper::BILL:
		m_eCampKind = ECamperKind::BILL;
		break;
	case CInfo::ECamper::DWIGHT:
		m_eCampKind = ECamperKind::DWIGHT;
		break;
	case CInfo::ECamper::MEG:
		m_eCampKind = ECamperKind::MEG;
		break;
	}
	//CInfo* pInfo = CInfo::Get_Instance();
	//m_eCampKind = (ECamperKind)pInfo->Get_Camper();
	switch (m_eCampKind)
	{
	case CCamper_GamePlay::ECamperKind::CHERYL:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Cheryl"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
		break;
	case CCamper_GamePlay::ECamperKind::MEG:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Meg"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_StandIdle");
		break;
	case CCamper_GamePlay::ECamperKind::BILL:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Bill"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_StandIdle");
		break;
	case CCamper_GamePlay::ECamperKind::DWIGHT:
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Camper_Dwight"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_StandIdle");
		break;
	}
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	//item
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform2"), reinterpret_cast<CComponent*&>(m_pItemTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader2"), reinterpret_cast<CComponent*&>(m_pShader2));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Inter_ChestBox_MediKit"), TEXT("Model2"), reinterpret_cast<CComponent*&>(m_pMediKit_Model));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Inter_ChestBox_FlashLight"), TEXT("Model3"), reinterpret_cast<CComponent*&>(m_pFlashLight_Model));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider4"), reinterpret_cast<CComponent*&>(m_pFlashLightCollider));
	m_pFlashLightCollider->Set_Center(_float3(0.f, 0.f, 1.59f));
	m_pFlashLightCollider->Set_Extents(_float3(0.16f, 0.06f, 1.6f));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_Sphere"), TEXT("Collider5"), reinterpret_cast<CComponent*&>(m_pGrassCollider));
	//Othercamper
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider3"), reinterpret_cast<CComponent*&>(m_pOtherCamperCollider));
	m_pCollider->Set_Center(_float3(0.f, 0.8f, 0.f));
	//m_pOtherCamperCollider->Set_Extents(_float3(1.f, 1.f, 1.f));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Navi"), TEXT("Navi"), reinterpret_cast<CComponent*&>(m_pNavi));
	//m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");

	m_pFM = new CFM_Camper(m_pDevice, m_pContext, m_pNetwork, this);
	m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
	m_pCollider->Set_Center(_float3(0.f, 0.8f, 0.f));
	m_pCollider->Set_Extents(_float3(0.25f, 0.8f, 0.25f));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_Foot"), reinterpret_cast<CComponent*&>(m_pCollider_Foot));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_Body"), reinterpret_cast<CComponent*&>(m_pCollider_Body));
	m_pCollider_Foot->Set_Center(_float3(0.f, 0.2f, 0.f));
	m_pCollider_Foot->Set_Extents(_float3(0.25f, 0.2f, 0.25f));
	m_pCollider_Body->Set_Center(_float3(0.f, 0.85f, 0.f));
	m_pCollider_Body->Set_Extents(_float3(0.25f, 0.75f, 0.25f));

	m_pTarget_Depth = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Depth"));

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	m_pEffect[0] = dynamic_cast<CEffect_FlashLight*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_FlashLightIdle"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_FlashLightIdle")));
	m_pEffect[1] = dynamic_cast<CEffect_FlashLight*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_FlashLightAttack"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_FlashLightAttack")));
	CLight::SDesc sLight;
	sLight.eType = CLight::EType::SPOT;
	sLight.vDiffuse = _float4{ 2.f, 2.f, 1.f, 1.f };
	sLight.vAmbient = _float4{ 2.f, 2.f, 1.f, 1.f };
	sLight.vSpecular = _float4{ 3.f, 3.f, 3.f, 1.f };
	sLight.fRange = 6.f;
	sLight.fAngle = _float2{ 0.95f, 1.f };
	m_pLight = dynamic_cast<CLight_Flash*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Flash"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Light_Flash"), &sLight));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));
	//m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] = 1;//test
	//m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] = 1;//test
	//m_pAudio->Play_Sound(TEXT("sfx_survivor_heartbeat_03"), 0, m_fVolume, FMOD_2D | FMOD_LOOP_NORMAL);
	return hr;
}

const _ubyte CCamper_GamePlay::Update(const _double& dTimeDelta) {

	//m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] = 1;//test
	//m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] = 1;//test
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	// blood stain effect
	m_fDist += static_cast<_float>(dTimeDelta);
	switch (m_pFM->Get_State())
	{
	case CFM_Camper::EState::HIT_CRAWL_F:
	case CFM_Camper::EState::HIT_CRAWL_B:
	case CFM_Camper::EState::HIT_CRAWL_L:
	case CFM_Camper::EState::HIT_CRAWL_R:
	case CFM_Camper::EState::CRAWL_IDLE:
	case CFM_Camper::EState::CRAWL_WALK:
	case CFM_Camper::EState::HITRUNTO_IDLE:
	case CFM_Camper::EState::HITWALKTO_IDLE:
	case CFM_Camper::EState::HIT_F:
	case CFM_Camper::EState::HIT_B:
	case CFM_Camper::EState::HIT_IDLE:
	case CFM_Camper::EState::HIT_RUN:
	case CFM_Camper::EState::HIT_WALK:
	case CFM_Camper::EState::HIT_SITWALK:
	case CFM_Camper::EState::SITTO_HIT_IDLE:
	case CFM_Camper::EState::HIT_SIT: {
		if (m_fDist > 3.f) {
			CEffect_BloodStain* pBloodStain = dynamic_cast<CEffect_BloodStain*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_BloodStain"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_BloodStain")));
			CTransform* pTransform = dynamic_cast<CTransform*>(pBloodStain->Get_Component(TEXT("Transform")));
			pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS));
			m_fDist = 0.f;
		}
		break;
	}
	default:
		break;
	}

	if (nullptr == m_pSlasher) {
		m_pSlasher = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), L"Slasher_Huntress");

		if (nullptr != m_pSlasher)
			m_pSlasherTransform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
	}
	if (true == m_bControl) {
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_9)) {
			m_bMouse = !m_bMouse;
			//ShowCursor(m_bMouse);
		}
		if (true == m_bMouse) {
			RECT	rc;
			POINT	pt;
			GetClientRect(g_hWnd, &rc);
			pt.x = (rc.left + rc.right) / (_long)2.0;
			pt.y = (rc.bottom + rc.top) / (_long)2.0;
			ClientToScreen(g_hWnd, &pt);
			SetCursorPos(pt.x, pt.y);
		}

		m_fNetworkTime += static_cast<_float>(dTimeDelta);
		if (m_fNetworkTime >= 0.2f) {
			m_fNetworkTime = 0.f;
			Update_Network("WorldMatrix");
		}

		////hwang////////////////
		//Check_Perk(dTimeDelta);
		///////////////////

		m_dSoundTime += dTimeDelta;
		if (m_dSoundTime >= 2.) {
			if (m_pFM->Get_State() != CFM_Camper::EState::MORI && m_pFM->Get_State() != CFM_Camper::EState::ENDGAME &&
				m_pFM->Get_State() != CFM_Camper::EState::ENDING_RUN && m_pFM->Get_State() != CFM_Camper::EState::HATCH_OUT
				&& m_pFM->Get_State() != CFM_Camper::EState::SPIDER_KILLOUT) {
				if (m_iHP == 2 && m_pFM->Get_State() != CFM_Camper::EState::RUN && m_pFM->Get_State() != CFM_Camper::EState::MORI && m_pFM->Get_State() != m_pFM->HATCH_OUT
					&& m_eCollisionState == ECollisionState::NONE)
				{
					switch (m_eCampKind)
					{
					case CCamper_GamePlay::ECamperKind::CHERYL:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_normal"), 16, -1, 0.2f, FMOD_2D);
						break;
					case CCamper_GamePlay::ECamperKind::MEG:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_normal"), 7, -1, 0.2f, FMOD_2D);
						break;
					case CCamper_GamePlay::ECamperKind::BILL:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_normal"), 15, -1, 0.2f, FMOD_2D);
						break;
					case CCamper_GamePlay::ECamperKind::DWIGHT:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_normal"), 15, -1, 0.2f, FMOD_2D);
						break;
					}
				}
				else if (m_iHP == 1 && m_pFM->Get_State() != CFM_Camper::EState::FALL && m_pFM->Get_State() != CFM_Camper::EState::RUN
					&& m_pFM->Get_State() != CFM_Camper::EState::MORI)
				{
					switch (m_eCampKind)
					{
					case CCamper_GamePlay::ECamperKind::CHERYL:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_injured"), 31, -1, 0.3f, FMOD_2D);
						break;
					case CCamper_GamePlay::ECamperKind::MEG:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_injured_breath"), 7, -1, 0.3f, FMOD_2D);
						break;
					case CCamper_GamePlay::ECamperKind::BILL:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_injured"), 7, -1, 0.2f, FMOD_2D);
						break;
					case CCamper_GamePlay::ECamperKind::DWIGHT:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_injured"), 12, -1, 0.2f, FMOD_2D);
						break;
					}
				}
				else if (m_iHP == 0) {
					if (m_eCollisionState != ECollisionState::HOOK && m_pFM->Get_State() != CFM_Camper::EState::CARRY_IDLE &&
						m_pFM->Get_State() != CFM_Camper::EState::CARRY_WIGGLE && m_pFM->Get_State() != CFM_Camper::EState::MORI) {

						switch (m_eCampKind)
						{
						case CCamper_GamePlay::ECamperKind::CHERYL:
							m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_dying"), 37, -1, 0.3f, FMOD_2D);
							break;
						case CCamper_GamePlay::ECamperKind::MEG:
							m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_dying"), 33, -1, 0.3f, FMOD_2D);
							break;
						case CCamper_GamePlay::ECamperKind::BILL:
							m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_dying"), 26, -1, 0.3f, FMOD_2D);
							break;
						case CCamper_GamePlay::ECamperKind::DWIGHT:
							m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_dying"), 32, -1, 0.3f, FMOD_2D);
							break;
						}
					}
				}
			}

			m_dSoundTime = 0.;
		}

		// effect _yeon
		if (m_pFM->Get_State() == m_pFM->HOOKIDLE && m_bHookFirst) {
			m_pSideSmokeEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_CamperHookSideSmoke"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_CamperHookSideSmoke")));
			m_bHookFirst = false;
		}
		else if (m_pFM->Get_State() != m_pFM->HOOKIDLE && !m_bHookFirst) {
			if(m_pSideSmokeEffect != nullptr)
				m_pSideSmokeEffect->Set_RepeatEnd();
		}



		/*if (nullptr == m_pSlasher) {
			m_pSlasher = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), L"Slasher_Huntress");

			if (nullptr != m_pSlasher)
				m_pSlasherTransform = dynamic_cast<CTransform*>(m_pSlasher->Get_Component(TEXT("Transform")));
		}*/
		if (nullptr != m_pSlasher) {
			_float fDist = XMVectorGetX(XMVector3Length(m_pTransform->Get_Row(CTransform::ERow::POS) - m_pSlasherTransform->Get_Row(CTransform::ERow::POS)));
			if (fDist <= 15.f) {
				/*	if(fDist <= 5.f)
						m_pAudio->Set_Volume(0, 1.f);
					else*/
				m_pAudio->Set_Volume(0, (15.f - fDist) * 0.1f);
			}

			if (m_bPlayBGM) {
				CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::STATIC), TEXT("Setting_BGM")));
				switch (m_pFM->Get_State()) {
				case CFM_Camper::EState::ENDGAME:
					break;
				case CFM_Camper::EState::HIT_CRAWL_F:
				case CFM_Camper::EState::HIT_CRAWL_B:
				case CFM_Camper::EState::HIT_CRAWL_L:
				case CFM_Camper::EState::HIT_CRAWL_R:
				case CFM_Camper::EState::CRAWL_IDLE:
				case CFM_Camper::EState::CRAWL_WALK:
					pBGM->Play_BGM(TEXT("mu_survivor_dying_01"), 0.05f, 2.f);
					break;
				case CFM_Camper::EState::CRAWL_PICK1:
				case CFM_Camper::EState::CRAWL_PICK2:
				case CFM_Camper::EState::CARRY_IDLE:
				case CFM_Camper::EState::CARRY_WIGGLE:
					pBGM->Play_BGM(TEXT("mu_survivor_injured_01"), 0.15f, 2.f);
					break;
				case CFM_Camper::EState::HOOKIDLE:
				case CFM_Camper::EState::HOOKIN:
				case CFM_Camper::EState::HOOKOUT:
				case CFM_Camper::EState::HOOKSTRUGGLE:
				case CFM_Camper::EState::HOOKFREE:
					pBGM->Play_BGM(TEXT("mu_survivor_hooked_part1_02"), 0.05f, 2.f);
					break;
				case CFM_Camper::EState::SPIDER_IN:
				case CFM_Camper::EState::SPIDER_LOOP:
				case CFM_Camper::EState::SPIDER_OUT:
				case CFM_Camper::EState::SPIDER_STRUGGLE:
					pBGM->Play_BGM(TEXT("mu_survivor_hooked_part1_03"), 0.05f, 2.f);
					break;
				case CFM_Camper::EState::SPIDER_KILLIN:
				case CFM_Camper::EState::SPIDER_KILLLOOP:
				case CFM_Camper::EState::SPIDER_KILLOUT:
					break;
				default: {
					if (fDist > 20.f)
						pBGM->Play_BGM(TEXT("mu_survivor_normal_02"), 0.05f, 2.f);
					else if (fDist > 15.f)
						pBGM->Play_BGM(TEXT("mu_terrorradius_default_01_layer_01"), 0.05f, 2.f);
					else if (fDist > 10.f)
						pBGM->Play_BGM(TEXT("mu_terrorradius_default_01_layer_02"), 0.05f, 2.f);
					else if (fDist > 5.f)
						pBGM->Play_BGM(TEXT("mu_terrorradius_default_01_layer_03"), 0.05f, 2.f);
					else
						pBGM->Play_BGM(TEXT("mu_terrorradius_default_01_layer_04"), 0.05f, 2.f);
					break;
				}
				}
			}
		}
	}
	else {

		m_dSoundTime += dTimeDelta;
		if (m_dSoundTime >= 2.) {

			if (m_iHP == 2 && m_pFM->Get_State() != CFM_Camper::EState::RUN && m_pFM->Get_State() != CFM_Camper::EState::MORI && m_pFM->Get_State() != m_pFM->HATCH_OUT
				&& m_eCollisionState == ECollisionState::NONE)
			{
				switch (m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_normal"), 16, -1, 0.2f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 2.f });
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_normal"), 7, -1, 0.2f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 2.f });
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_normal"), 15, -1, 0.2f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 2.f });
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_normal"), 15, -1, 0.2f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 2.f });
					break;
				}
			}
			else if (m_iHP == 1 && m_pFM->Get_State() != CFM_Camper::EState::FALL && m_pFM->Get_State() != CFM_Camper::EState::RUN
				&& m_pFM->Get_State() != CFM_Camper::EState::MORI)
			{
				switch (m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_injured"), 31, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_injured_breath"), 10, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_injured"), 7, -1, 0.2f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_injured"), 12, -1, 0.2f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
					break;
				}
			}
			else if (m_iHP == 0) {
				if (m_eCollisionState != ECollisionState::HOOK && m_pFM->Get_State() != CFM_Camper::EState::CARRY_IDLE &&
					m_pFM->Get_State() != CFM_Camper::EState::CARRY_WIGGLE && m_pFM->Get_State() != CFM_Camper::EState::MORI) {
					switch (m_eCampKind)
					{
					case CCamper_GamePlay::ECamperKind::CHERYL:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_dying"), 37, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
						break;
					case CCamper_GamePlay::ECamperKind::MEG:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_dying"), 33, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
						break;
					case CCamper_GamePlay::ECamperKind::BILL:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_dying"), 26, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
						break;
					case CCamper_GamePlay::ECamperKind::DWIGHT:
						m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_dying"), 32, -1, 0.3f, FMOD_3D, m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
						break;
					}
				}
			}
			m_dSoundTime = 0.;
		}

		//Collider
		CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_CloneControl(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay")));
		if (nullptr != pCamper) {
			CCollider* pCamperCollider = dynamic_cast<CCollider*>(pCamper->Get_Component(TEXT("Collider")));
			if (m_pCollider->Is_Collision(pCamperCollider)) {
				pCamper->Set_ColObject(this);
				pCamper->Set_CollisionState(CCamper_GamePlay::ECollisionState::CAMPER);
				if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::HOOKOUT ||
					pCamper->m_pFM->Get_State() == CFM_Camper::EState::HOOKIDLE) {
					//구출UI
					//dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
					//	Set_HudOn(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::RESCUE), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
					//dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Gauge_Inter")))->
					//	Set_Render(CUI_Play_Gauge_Inter::EInter::RESQUE, true, false);
				}
			}
			else {
				if (pCamper->Get_ColObject() == this) {
					//dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
					//	Set_HudOff(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::RESCUE), true);
					//dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Gauge_Inter")))->
					//	Set_Render(CUI_Play_Gauge_Inter::EInter::RESQUE, false, false);

					pCamper->Set_ColObject(nullptr);
					pCamper->Set_CollisionState(CCamper_GamePlay::ECollisionState::NONE);
				}
			}
		}
	}

	m_bCollider_Move = false;
	list<CObject*>* plistPlank = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_PlankAnim"));
	if (nullptr != plistPlank) {
		for (auto& iter : *plistPlank) {
			CInter_PlankAnim* pPlank = dynamic_cast<CInter_PlankAnim*>(iter);
			CCollider* pPlankCollider = dynamic_cast<CCollider*>(iter->Get_Component(TEXT("Collider")));
			if (CInter_PlankAnim::ESTATE::IDLE != pPlank->Get_State()) {
				if (m_pCollider_Body->Is_Collision(pPlankCollider)) {
					m_bCollider_Move = true;
					XMStoreFloat3(&m_vCollider_Move, XMVector3Normalize(XMLoadFloat3(&pPlankCollider->Get_Center()) - m_pTransform->Get_Row(CTransform::ERow::POS)));
					break;
				}
			}
		}
	}
	CInter_ExitDoor* pExit = dynamic_cast<CInter_ExitDoor*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_ExitDoor")));
	if (nullptr != pExit) {
		CCollider* pExitCollider = dynamic_cast<CCollider*>(pExit->Get_Component(TEXT("Collider_Door")));
		if (CInter_ExitDoor::ESTATE::OPENING != pExit->Get_State()) {
			if (m_pCollider_Body->Is_Collision(pExitCollider)) {
				m_bCollider_Move = true;
				XMStoreFloat3(&m_vCollider_Move, XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&pExitCollider->Get_Center()), 0.f) - m_pTransform->Get_Row(CTransform::ERow::POS)));
			}
		}
	}
	Update_Network();
	m_pFM->Update_Network();
	m_pFM->Update(dTimeDelta);
	m_pCollider->Update(m_pTransform->Get_Matrix());
	m_pCollider_Foot->Update(m_pTransform->Get_Matrix());
	m_pCollider_Body->Update(m_pTransform->Get_Matrix());
	m_pOtherCamperCollider->Update(m_pTransform->Get_Matrix());
	//CTransform* pGrabTarnsform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("GrapTransform")));

	if (false == m_bFlag && m_pFM->Get_State() != CFM_Camper::EState::SPIDER_KILLOUT) {
		m_vAccel.y += -9.8f * static_cast<_float>(dTimeDelta);
		_vector vResist = XMVector3Normalize(XMVectorSetY(-XMLoadFloat3(&m_vAccel), 0.f));
		vResist *= 5.f * static_cast<_float>(dTimeDelta);
		XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vResist);
		m_pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&m_vAccel) * static_cast<_float>(dTimeDelta));
		m_pCollider->Update(m_pTransform->Get_Matrix());
		m_pCollider_Foot->Update(m_pTransform->Get_Matrix());
		m_pCollider_Body->Update(m_pTransform->Get_Matrix());

		//collider
		list<CCell*> listCell = m_pNavi->Get_DivCell(m_pTransform);
		_float3 vPos; XMStoreFloat3(&vPos, m_pTransform->Get_Row(CTransform::ERow::POS));
		listCell.sort([&](CCell* pSour, CCell* pDest) {
			_float3 vSourCross; XMStoreFloat3(&vSourCross, XMVector3Normalize(XMVectorSetY(pSour->Get_Cross(), 0.f)));
			_float4 vSourPlane;
			_float3* pSourPoint = pSour->Get_Point();
			XMStoreFloat4(&vSourPlane, XMPlaneFromPoints(XMLoadFloat3(&pSourPoint[0]), XMLoadFloat3(&pSourPoint[1]), XMLoadFloat3(&pSourPoint[2])));
			_float fSourDist = abs((vSourPlane.x * vPos.x + vSourPlane.y * vPos.y + vSourPlane.z * vPos.z + vSourPlane.w) / sqrt(vSourPlane.x * vSourCross.x + vSourPlane.y * vSourCross.y + vSourPlane.z * vSourCross.z));

			_float3 vDestCross; XMStoreFloat3(&vDestCross, XMVector3Normalize(XMVectorSetY(pDest->Get_Cross(), 0.f)));
			_float4 vDestPlane;
			_float3* pDestPoint = pDest->Get_Point();
			XMStoreFloat4(&vDestPlane, XMPlaneFromPoints(XMLoadFloat3(&pDestPoint[0]), XMLoadFloat3(&pDestPoint[1]), XMLoadFloat3(&pDestPoint[2])));
			_float fDestDist = abs((vDestPlane.x * vPos.x + vDestPlane.y * vPos.y + vDestPlane.z * vPos.z + vDestPlane.w) / sqrt(vDestPlane.x * vDestCross.x + vDestPlane.y * vDestCross.y + vDestPlane.z * vDestCross.z));

			return fSourDist > fDestDist;
			});
		m_bIsCollider_Foot = false;
		_float fDist = 0.f;
		for (auto& iter : listCell) {
			switch (iter->Get_Type()) {
			case CCell::EType::FLOOR: {
				if (!m_pCollider_Foot->Is_Collision(iter))
					break;

				_float4 vPlane;
				_float3* pPoint = iter->Get_Point();
				XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2])));
				m_bIsCollider_Foot = true;
				if (!TriangleTests::Intersects(m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 100.f, 0.f, 0.f), XMVectorSet(0.f, -1.f, 0.f, 0.f), XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2]), fDist))
					break;
				_float3 vPos; XMStoreFloat3(&vPos, m_pTransform->Get_Row(CTransform::ERow::POS));
				vPos.y = -((vPlane.x * vPos.x + vPlane.z * vPos.z + vPlane.w) / vPlane.y);

				m_pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos));
				m_pCollider->Update(m_pTransform->Get_Matrix());
				m_pCollider_Foot->Update(m_pTransform->Get_Matrix());
				m_pCollider_Body->Update(m_pTransform->Get_Matrix());
				m_vAccel.y = 0.f;
			}
									break;
			case CCell::EType::WALL:
			case CCell::EType::NOCAMERAWALL: {
				if (!m_pCollider_Body->Is_Collision(iter))
					break;
				_float4 vPlane;
				_float3* pPoint = iter->Get_Point();
				XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2])));

				_float3 vPos; XMStoreFloat3(&vPos, m_pTransform->Get_Row(CTransform::ERow::POS));

				_vector vCross = XMVector3Normalize(XMVectorSetY(iter->Get_Cross(), 0.f));
				_float3 vCross2; XMStoreFloat3(&vCross2, vCross);

				_float fDist = abs((vPlane.x * vPos.x + vPlane.y * vPos.y + vPlane.z * vPos.z + vPlane.w) / sqrt(vPlane.x * vCross2.x + vPlane.y * vCross2.y + vPlane.z * vCross2.z));

				//-------------------------------------------
				_float3 vExtents = m_pCollider->Get_Extents();
				_float fDiagonal = sqrt(vExtents.x * vExtents.x + vExtents.z * vExtents.z);

				_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTransform->Get_Row(CTransform::ERow::LOOK), 0.f));
				_float3 vLook2; XMStoreFloat3(&vLook2, vLook);

				_float fDot = abs(XMVectorGetX(XMVector3Dot(vLook, vCross)));

				_matrix Matrix = XMMatrixRotationNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f) - acos(fDot));
				_vector vDiagonal = XMVector3TransformNormal(vLook, Matrix);

				fDiagonal = XMVectorGetX(XMVector3Dot(vDiagonal * fDiagonal, vLook));
				//-------------------------------------------
				m_pTransform->Translate(vCross, abs(fDist - fDiagonal), 1);

				m_pCollider->Update(m_pTransform->Get_Matrix());
				m_pCollider_Foot->Update(m_pTransform->Get_Matrix());
				m_pCollider_Body->Update(m_pTransform->Get_Matrix());
			}
										   break;
										   /*case CCell::EType::CEILING: {
											   if (!m_pCollider->Is_Collision(iter))
												   break;
											   _float4 vPlane;
											   _float3* pPoint = iter->Get_Point();
											   XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2])));
											   _float3 vPos;
											   XMStoreFloat3(&vPos, m_pTransform->Get_Row(CTransform::ERow::POS));
											   vPos.y = -((vPlane.x * vPos.x + vPlane.z * vPos.z + vPlane.w) / vPlane.y) - m_pCollider->Get_Extents().y * 2.f;
											   m_pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos));
											   m_pCollider->Update(m_pTransform->Get_Matrix());
										   }
																	 break;*/
			}
		}
	}
	_float3 vPos2 = {};
	XMStoreFloat3(&vPos2, m_pTransform->Get_Row(CTransform::ERow::POS));

	if (vPos2.y <= 0.f) {
		m_bIsCollider_Foot = true;

		vPos2.y = 0.f;
		m_pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos2));
		m_vAccel.y = 0.f;
	}


	m_pGrassCollider->Update(m_pTransform->Get_Matrix());	//hwang-grassCollider
	// hwang
	//FindCamper Perk
	if (m_bFindPerkRender == true) {
		m_dFindPerkTime -= dTimeDelta;
		if (m_dFindPerkTime <= 0.0) {
			m_dFindPerkTime = 5.0;
			m_bFindPerkRender = false;
		}
	}

	// fog effect_yeon
	//m_dFogCreateTime += dTimeDelta;
	//if (static_cast<_float>(m_dFogCreateTime) > static_cast<_float>((10 - rand() % 3))) {
	//	for (_uint i = 0; i < 15; i++) {
	//		if (m_pFogEffect[i] == nullptr) {
	//			m_pFogEffect[i] = dynamic_cast<CEffect_Fog*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Fog"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Fog")));
	//			CTransform* pTransform = m_pFogEffect[i]->Get_Transform();
	//			//_float posX = static_cast<_float>(rand() % 10 - rand() % 10) * static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX);
	//			//_float posZ = static_cast<_float>(rand() % 10 - rand() % 10) * static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX);
	//
	//			//posX = -10 + (float)(rand()) / ((float)(RAND_MAX / (10 - -10)));
	//			//posZ = -10 + (float)(rand()) / ((float)(RAND_MAX / (10 - -10)));
	//			_float posX = -10.f + ((float)rand() * (10.f - -10.f)) / (float)(RAND_MAX);
	//			_float posZ = -10.f + ((float)rand() * (10.f - -10.f)) / (float)(RAND_MAX);
	//
	//			_vector pos = XMVectorSet(posX + XMVectorGetX(m_pTransform->Get_Row(CTransform::ERow::POS)), 0.f, posZ + XMVectorGetZ(m_pTransform->Get_Row(CTransform::ERow::POS)), 1.f);
	//			pTransform->Set_Row(CTransform::ERow::POS, pos);
	//			break;
	//		}
	//	}
	//	m_dFogCreateTime = 0.;
	//}
	//
	//for (_uint i = 0; i < 15; i++) {
	//	if (m_pFogEffect[i] != nullptr && m_pFogEffect[i]->Get_Time() == 0.) {
	//		CTransform* pTransform = m_pFogEffect[i]->Get_Transform();
	//		//_float posX = static_cast<_float>(rand() % 10 - rand() % 10) * static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX);
	//		//_float posZ = static_cast<_float>(rand() % 10 - rand() % 10) * static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX);
	//
	//		_float posX = -10.f + ((float)rand() * (10.f - -10.f)) / (float)(RAND_MAX);
	//		_float posZ = -10.f + ((float)rand() * (10.f - -10.f)) / (float)(RAND_MAX);
	//
	//		_vector pos = XMVectorSet(posX + XMVectorGetX(m_pTransform->Get_Row(CTransform::ERow::POS)), 0.f, posZ + XMVectorGetZ(m_pTransform->Get_Row(CTransform::ERow::POS)), 1.f);
	//		pTransform->Set_Row(CTransform::ERow::POS, pos);
	//	}
	//}
	//
	//for (_uint i = 0; i < 15; i++) {
	//	if (m_pFogEffect[i] != nullptr) {
	//		CTransform* pTransform = nullptr;
	//		pTransform = m_pFogEffect[i]->Get_Transform();
	//
	//		if (pTransform != nullptr && m_pFogEffect[i]->Get_Time() < 8.) {
	//			_vector pos = pTransform->Get_Row(CTransform::ERow::POS);
	//			pos += 0.7f * pTransform->Get_Row(CTransform::ERow::RIGHT) * static_cast<_float>(dTimeDelta);
	//			pTransform->Set_Row(CTransform::ERow::POS, pos);
	//		}
	//	}
	//}

	return OBJ_NOEVENT;
}

void CCamper_GamePlay::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	m_pCollider->Update_Edit(1);
	m_pCollider_Foot->Update_Edit(2);
	m_pCollider_Body->Update_Edit(3);
	m_pOtherCamperCollider->Update_Edit(4);
	m_pFlashLightCollider->Update_Edit(5);


	ImGui::Text("Oren-Nayar");
	ImGui::DragFloat("Roughness", &m_fRoughness, 0.001f);
	ImGui::DragFloat("Refract", &m_fRefract, 0.001f);
	ImGui::DragFloat("Metalness", &m_fMetalness, 0.001f);
	ImGui::DragFloat("Metalness2", &m_fMetalness2, 0.001f);
}

void CCamper_GamePlay::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	switch (m_pFM->Get_State()) {
	case CFM_Camper::EState::CARRY_IDLE: {
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(m_pTarObject);
		if (nullptr != pSlasher) {
			CModel* pHuntressModel = dynamic_cast<CModel*>(pSlasher->Get_Component(TEXT("Model_FPV")));
			CTransform* pHuntressTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));
			_matrix SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_Char") * pHuntressTransform->Get_Matrix();
			m_pTransform->Set_Matrix(SocketMatrix);
		}
		break;
	}
	case CFM_Camper::EState::CARRY_WIGGLE: {
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(m_pTarObject);
		if (nullptr != pSlasher) {
			CModel* pHuntressModel = dynamic_cast<CModel*>(pSlasher->Get_Component(TEXT("Model_FPV")));
			CTransform* pHuntressTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));
			_matrix SocketMatrix;
			switch (pSlasher->Get_Slasher()) {
			case false:
				SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_Char") * pHuntressTransform->Get_Matrix();
				break;
			case true:
				SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_CamperAttach_01") * pHuntressTransform->Get_Matrix();
				break;
			}
			m_pTransform->Set_Matrix(SocketMatrix);
		}
		break;
	}
	}

	if (true == m_bControl) {
		//hwang////////////////
		Check_Perk(dTimeDelta);
		/////////////////
	}

	_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(180.0f));
	_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
	m_pItemTransform->Set_Matrix(SocketMatrix);

	if (m_eCampKind == ECamperKind::CHERYL || m_eCampKind == ECamperKind::MEG) {

		// effect flash light---------------------------------------------------------------------------------------------------------------------
		if (Get_FM_Camper()->Get_State() != CFM_Camper::EState::FLASHLIGHT) {
			m_pEffect[0]->LightOff();
			m_pEffect[1]->LightOff();
			m_pLight->Set_OnOff(false);
		}
		// effect flash light---------------------------------------------------------------------------------------------------------------------

		if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT]) {
			_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(190.0f)) * XMMatrixRotationY(XMConvertToRadians(170.0f));
			_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
			m_pItemTransform->Set_Matrix(SocketMatrix);
			m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.04f);
			switch (Get_FM_Camper()->Get_State())
			{
			case CFM_Camper::EState::IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(210.0f)) * XMMatrixRotationY(XMConvertToRadians(170.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.04f + SocketMatrix.r[2] * 0.04f);
				break;
			}
			case CFM_Camper::EState::WALK_F:
			case CFM_Camper::EState::RUN:
			case CFM_Camper::EState::SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(190.0f)) * XMMatrixRotationY(XMConvertToRadians(170.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.08f - SocketMatrix.r[0] * 0.02f + SocketMatrix.r[2] * 0.04f);
				break;
			}
			case CFM_Camper::EState::SIT_WALK_F: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(190.0f)) * XMMatrixRotationY(XMConvertToRadians(170.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.08f + SocketMatrix.r[0] * 0.02f + SocketMatrix.r[2] * 0.02f);
				break;
			}
			case CFM_Camper::EState::HIT_RUN: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(190.0f)) * XMMatrixRotationY(XMConvertToRadians(170.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.08f - SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::HITWALKTO_IDLE:
			case CFM_Camper::EState::HIT_WALK: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(190.0f)) * XMMatrixRotationY(XMConvertToRadians(170.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.08f - SocketMatrix.r[0] * 0.04f);
				break;
			}
			case CFM_Camper::EState::HIT_IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(200.0f)) * XMMatrixRotationY(XMConvertToRadians(170.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.3f - SocketMatrix.r[0] * 0.04f + SocketMatrix.r[2] * 0.1f);
				break;
			}
			case CFM_Camper::EState::HIT_SIT:
			case CFM_Camper::EState::HIT_SITWALK: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(190.0f)) * XMMatrixRotationY(XMConvertToRadians(190.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.08f - SocketMatrix.r[0] * 0.04f);
				break;
			}

			}
		}
		else if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT]) {

			switch (Get_FM_Camper()->Get_State())
			{

			case CFM_Camper::EState::IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.038f, 0.02f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-70.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			case CFM_Camper::EState::HIT_IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.04f, 0.18f, 0.1f) * XMMatrixRotationX(XMConvertToRadians(-50.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			case CFM_Camper::EState::HIT_WALK: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.03f, 0.08f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-17.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			case CFM_Camper::EState::HIT_RUN: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.02f, 0.1f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-17.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			case CFM_Camper::EState::HIT_SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.04f, 0.1f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-17.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			case CFM_Camper::EState::SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.04f, 0.1f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-20.f)) * XMMatrixRotationY(XMConvertToRadians(-10.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.04f - SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::FLASHLIGHT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.05f, 0.02f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-70.f)) * XMMatrixRotationZ(XMConvertToRadians(-8.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				// effect flash light---------------------------------------------------------------------------------------------------------------------
				CTransform* pLightTransform = dynamic_cast<CTransform*>(m_pLight->Get_Component(TEXT("Transform")));
				pLightTransform->Set_Matrix(SocketMatrix);
				m_pLight->Set_OnOff(true);

				CTransform* pTransform0 = dynamic_cast<CTransform*>(m_pEffect[0]->Get_Component(TEXT("Transform")));
				CTransform* pTransform1 = dynamic_cast<CTransform*>(m_pEffect[1]->Get_Component(TEXT("Transform")));
				pTransform0->Set_Matrix(SocketMatrix);
				pTransform1->Set_Matrix(SocketMatrix);

				CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
				if (pSlasher != nullptr) {
					CCollider* pSlasherCollider = dynamic_cast<CCollider*>(pSlasher->Get_Component(TEXT("Collider_Eyes")));

					
					if (m_pFlashLightCollider->Is_Collision(pSlasherCollider)) {
						if (!m_isAttack) {
							// set hud
							if (m_bControl == true) {
								dynamic_cast<CUI_Play_Icon_Passive*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("UI_Play_Icon_Passive")))->Pop_Slot();

								if (pSlasher->Get_Type() == CSlasher_Huntress::EType::CARRY) {

									pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_FLASH_IN);
								}
								else
									pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_FLASH);

								pSlasher->Get_FM_Slasher()->Update_Network("Set_State");
							}
							m_bisGetItem = false;
							m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] = 0;
							m_isAttack = true;
							//hwang - score
							CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
							if (m_bControl == true) {
								CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
								if (pScore != nullptr) {
									pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::BOLDNESS_KILLER_BLIND));
									pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::BOLDNESS), 250);
								}
							}
						}

						if (m_bLightFirstAtk && !m_pEffect[1]->Get_OnOff()) {
							m_pEffect[1]->LightOn();
							m_pLight->Set_Attack();
							m_bLightFirstAtk = false;
						}
						if (m_pEffect[0]->Get_OnOff())
							m_pEffect[0]->LightOff();
					}
					else {
						m_isAttack = false;
						if (m_bLightFirstAtk)
							m_pEffect[0]->LightOn();
						else
							m_pEffect[0]->LightOff();
						if (m_pEffect[1]->Get_OnOff())
							m_pEffect[1]->LightOff();
					}
				}
				else {
					_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.038f, 0.02f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-70.f));
					CTransform* pTransform0 = dynamic_cast<CTransform*>(m_pEffect[0]->Get_Component(TEXT("Transform")));
					pTransform0->Set_Matrix(SocketMatrix);
					pTransform0->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3] + SocketMatrix.r[2] * 0.316f);

					if (!m_pEffect[0]->Get_OnOff())
						m_pEffect[0]->LightOn();
				}
				// effect flash light---------------------------------------------------------------------------------------------------------------------------------------
				break;
			}
			default: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.01f, 0.03f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-50.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			}
		}
	}
	else if (m_eCampKind == ECamperKind::BILL) {
		// effect flash light---------------------------------------------------------------------------------------------------------------------
		if (Get_FM_Camper()->Get_State() != CFM_Camper::EState::FLASHLIGHT) {
			m_pEffect[0]->LightOff();
			m_pEffect[1]->LightOff();
			m_pLight->Set_OnOff(false);
		}
		if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT]) {
			_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(20.0f)) * XMMatrixRotationZ(XMConvertToRadians(180.0f));
			_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
			m_pItemTransform->Set_Matrix(SocketMatrix);
			m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f + SocketMatrix.r[0] * 0.0f + SocketMatrix.r[2] * 0.05f);
			switch (Get_FM_Camper()->Get_State()) {
			case CFM_Camper::EState::SIT_WALK_F:
			case CFM_Camper::EState::HIT_RUN:
			case CFM_Camper::EState::HIT_WALK:
			case CFM_Camper::EState::HIT_IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(20.0f)) * XMMatrixRotationZ(XMConvertToRadians(180.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.02f + SocketMatrix.r[2] * 0.05f);
				break;
			}
			case CFM_Camper::EState::HIT_SIT:
			case CFM_Camper::EState::HIT_SITWALK:
			case CFM_Camper::EState::SITTO_HIT_IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(30.0f)) * XMMatrixRotationZ(XMConvertToRadians(180.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.1f - SocketMatrix.r[0] * 0.06f + SocketMatrix.r[2] * 0.05f);
				break;
			}
			case CFM_Camper::EState::SIT:
			case CFM_Camper::EState::SITWALKTO_SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(20.0f)) * XMMatrixRotationZ(XMConvertToRadians(180.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.02f + SocketMatrix.r[2] * 0.05f);
				break;
			}
			}
		}
		else if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT]) {

			// effect flash light---------------------------------------------------------------------------------------------------------------------
			if (Get_FM_Camper()->Get_State() != CFM_Camper::EState::FLASHLIGHT) {
				m_pEffect[0]->LightOff();
				m_pEffect[1]->LightOff();
				m_pLight->Set_OnOff(false);
			}
			// effect flash light---------------------------------------------------------------------------------------------------------------------

			switch (Get_FM_Camper()->Get_State())
			{
			case CFM_Camper::EState::WALKTO_IDLE:
			case CFM_Camper::EState::IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-20.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f - SocketMatrix.r[0] * 0.f);
				break;
			}
			case CFM_Camper::EState::WALK_F: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-20.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f + SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::RUNTO_IDLE:
			case CFM_Camper::EState::RUN: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(0.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f + SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::HIT_IDLE:
			case CFM_Camper::EState::HIT_WALK:
			case CFM_Camper::EState::SIT_WALK_F:
			case CFM_Camper::EState::HIT_RUN:
			case CFM_Camper::EState::SIT:
			case CFM_Camper::EState::SIT_WALK_B:
			case CFM_Camper::EState::SITWALKTO_SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-20.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f + SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::HIT_SITWALK:
			case CFM_Camper::EState::SITTO_HIT_IDLE:
			case CFM_Camper::EState::HIT_SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-20.f)) * XMMatrixRotationX(XMConvertToRadians(-20.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[1] * 0.1f - SocketMatrix.r[2] * 0.1f + SocketMatrix.r[0] * 0.06f);
				break;
			}
			case CFM_Camper::EState::FLASHLIGHT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(-90.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[1] * 0.04f - SocketMatrix.r[2] * 0.1f + SocketMatrix.r[0] * 0.02f);
				// effect flash light---------------------------------------------------------------------------------------------------------------------
				CTransform* pLightTransform = dynamic_cast<CTransform*>(m_pLight->Get_Component(TEXT("Transform")));
				pLightTransform->Set_Matrix(SocketMatrix);
				pLightTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[2] * 0.315f);
				m_pLight->Set_OnOff(true);

				CTransform* pTransform0 = dynamic_cast<CTransform*>(m_pEffect[0]->Get_Component(TEXT("Transform")));
				CTransform* pTransform1 = dynamic_cast<CTransform*>(m_pEffect[1]->Get_Component(TEXT("Transform")));
				pTransform0->Set_Matrix(SocketMatrix);
				pTransform1->Set_Matrix(SocketMatrix);
				pTransform0->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[2] * 0.315f);
				pTransform1->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[2] * 0.315f);

				CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
				if (pSlasher != nullptr) {
					CCollider* pSlasherCollider = dynamic_cast<CCollider*>(pSlasher->Get_Component(TEXT("Collider_Eyes")));

					if (m_pFlashLightCollider->Is_Collision(pSlasherCollider)) {
						if (!m_isAttack) {
							// set hud
							if (m_bControl == true) {
								dynamic_cast<CUI_Play_Icon_Passive*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("UI_Play_Icon_Passive")))->Pop_Slot();

								if (pSlasher->Get_Type() == CSlasher_Huntress::EType::CARRY) {

									pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_FLASH_IN);
								}
								else
									pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_FLASH);

								pSlasher->Get_FM_Slasher()->Update_Network("Set_State");
							}
							m_bisGetItem = false;
							m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] = 0;
							m_isAttack = true;
							//hwang - score
							CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
							if (m_bControl == true) {
								CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
								if (pScore != nullptr) {
									pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::BOLDNESS_KILLER_BLIND));
									pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::BOLDNESS), 250);
								}
							}
						}

						if (m_bLightFirstAtk && !m_pEffect[1]->Get_OnOff()) {
							m_pEffect[1]->LightOn();
							m_pLight->Set_Attack();
							m_bLightFirstAtk = false;
						}
						if (m_pEffect[0]->Get_OnOff() && m_bLightFirstAtk)
							m_pEffect[0]->LightOff();
					}
					else {
						m_isAttack = false;
						m_pEffect[0]->LightOn();
						if (m_pEffect[1]->Get_OnOff())
							m_pEffect[1]->LightOff();
					}
				}
				else {
					//_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.038f, 0.02f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-70.f));
					//CTransform* pTransform0 = dynamic_cast<CTransform*>(m_pEffect[0]->Get_Component(TEXT("Transform")));
					//pTransform0->Set_Matrix(SocketMatrix);
					//pTransform0->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3] + SocketMatrix.r[2] * 0.316f);

					if (!m_pEffect[0]->Get_OnOff())
						m_pEffect[0]->LightOn();
				}
				// effect flash light---------------------------------------------------------------------------------------------------------------------------------------
				break;
			}
			default: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.01f, 0.03f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-50.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			}
		}
	}
	else {
		// effect flash light---------------------------------------------------------------------------------------------------------------------
		if (Get_FM_Camper()->Get_State() != CFM_Camper::EState::FLASHLIGHT) {
			m_pEffect[0]->LightOff();
			m_pEffect[1]->LightOff();
			m_pLight->Set_OnOff(false);
		}
		if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT]) {
			_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.0f));
			_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
			m_pItemTransform->Set_Matrix(SocketMatrix);
			m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.0f);
			switch (Get_FM_Camper()->Get_State()) {
			case CFM_Camper::EState::WALK_F:
			case CFM_Camper::EState::RUN:
			case CFM_Camper::EState::SIT:
			case CFM_Camper::EState::SIT_WALK_F: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.02f);
				break;

			}
			case CFM_Camper::EState::HIT_IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-80.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::HIT_WALK: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[0] * 0.04f);
				break;
			}
			case CFM_Camper::EState::HIT_SIT:
			case CFM_Camper::EState::HIT_SITWALK: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.0f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.08f - SocketMatrix.r[0] * 0.04f);
				break;
			}
			}
		}
		else if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT]) {
			// effect flash light---------------------------------------------------------------------------------------------------------------------
			if (Get_FM_Camper()->Get_State() != CFM_Camper::EState::FLASHLIGHT) {
				m_pEffect[0]->LightOff();
				m_pEffect[1]->LightOff();
				m_pLight->Set_OnOff(false);
			}

			switch (Get_FM_Camper()->Get_State())
			{

			case CFM_Camper::EState::IDLE: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f - SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::RUN:
			{
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f - SocketMatrix.r[0] * 0.05f);
				break;
			}
			case CFM_Camper::EState::RUNTO_IDLE:
			case CFM_Camper::EState::WALK_F:
			case CFM_Camper::EState::WALKTO_IDLE:
			case CFM_Camper::EState::SIT:
			case CFM_Camper::EState::SIT_WALK_F:
			case CFM_Camper::EState::SIT_WALK_B:
			case CFM_Camper::EState::SITWALKTO_SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f - SocketMatrix.r[0] * 0.02f);
				break;
			}
			case CFM_Camper::EState::HIT_IDLE:
			case CFM_Camper::EState::HIT_WALK:
			case CFM_Camper::EState::HIT_RUN: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f - SocketMatrix.r[0] * 0.04f);
				break;
			}
			case CFM_Camper::EState::HIT_SITWALK:
			case CFM_Camper::EState::SITTO_HIT_IDLE:
			case CFM_Camper::EState::HIT_SIT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationY(XMConvertToRadians(-70.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.12f - SocketMatrix.r[2] * 0.1f - SocketMatrix.r[0] * 0.06f);
				break;
			}
			case CFM_Camper::EState::FLASHLIGHT: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixRotationX(XMConvertToRadians(80.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				m_pItemTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) - SocketMatrix.r[1] * 0.04f - SocketMatrix.r[2] * 0.1f + SocketMatrix.r[0] * 0.05f);
				// effect flash light---------------------------------------------------------------------------------------------------------------------
				CTransform* pLightTransform = dynamic_cast<CTransform*>(m_pLight->Get_Component(TEXT("Transform")));
				pLightTransform->Set_Matrix(SocketMatrix);
				pLightTransform->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[2] * 0.315f);
				m_pLight->Set_OnOff(true);

				CTransform* pTransform0 = dynamic_cast<CTransform*>(m_pEffect[0]->Get_Component(TEXT("Transform")));
				CTransform* pTransform1 = dynamic_cast<CTransform*>(m_pEffect[1]->Get_Component(TEXT("Transform")));
				pTransform0->Set_Matrix(SocketMatrix);
				pTransform1->Set_Matrix(SocketMatrix);
				pTransform0->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[2] * 0.315f);
				pTransform1->Set_Row(CTransform::ERow::POS, m_pItemTransform->Get_Row(CTransform::ERow::POS) + SocketMatrix.r[2] * 0.315f);

				CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
				if (pSlasher != nullptr) {
					CCollider* pSlasherCollider = dynamic_cast<CCollider*>(pSlasher->Get_Component(TEXT("Collider_Eyes")));

					if (m_pFlashLightCollider->Is_Collision(pSlasherCollider)) {
						if (!m_isAttack) {
							if (m_bControl == true) {

								dynamic_cast<CUI_Play_Icon_Passive*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("UI_Play_Icon_Passive")))->Pop_Slot();

								if (pSlasher->Get_Type() == CSlasher_Huntress::EType::CARRY) {

									pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_FLASH_IN);
								}
								else
									pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_FLASH);

								pSlasher->Get_FM_Slasher()->Update_Network("Set_State");
							}
							m_bisGetItem = false;
							m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] = 0;
							m_isAttack = true;
							//hwang - score
							CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
							if (m_bControl == true) {
								CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
								if (pScore != nullptr) {
									pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::BOLDNESS_KILLER_BLIND));
									pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::BOLDNESS), 250);
								}
							}
						}

						if (m_bLightFirstAtk && !m_pEffect[1]->Get_OnOff()) {
							m_pEffect[1]->LightOn();
							m_pLight->Set_Attack();
							m_bLightFirstAtk = false;
						}
						if (m_pEffect[0]->Get_OnOff())
							m_pEffect[0]->LightOff();
					}
					else {
						m_isAttack = false;
						if (m_bLightFirstAtk)
							m_pEffect[0]->LightOn();
						if (m_pEffect[1]->Get_OnOff())
							m_pEffect[1]->LightOff();
					}
				}
				else {
					//_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.038f, 0.02f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-70.f));
					//CTransform* pTransform0 = dynamic_cast<CTransform*>(m_pEffect[0]->Get_Component(TEXT("Transform")));
					//pTransform0->Set_Matrix(SocketMatrix);
					//pTransform0->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3] + SocketMatrix.r[2] * 0.316f);

					if (!m_pEffect[0]->Get_OnOff())
						m_pEffect[0]->LightOn();
				}
				// effect flash light---------------------------------------------------------------------------------------------------------------------------------------
				break;
			}
			default: {
				_matrix	PivotMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(0.01f, 0.03f, -0.08f) * XMMatrixRotationX(XMConvertToRadians(-50.f));
				_matrix SocketMatrix = PivotMatrix * m_pModel->Get_BoneMatrix("joint_HandRoll01RT_01") * m_pTransform->Get_Matrix();
				m_pItemTransform->Set_Matrix(SocketMatrix);
				break;
			}
			}
		}
	}

	if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT])
		m_pFlashLightCollider->Update(m_pItemTransform->Get_Matrix());

	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::SHADOW, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
	if (m_bFindPerkRender == true)
		m_pRenderer->Add_Object(CRenderer::EOrder::PENETRATE, this);
}

const HRESULT CCamper_GamePlay::Render(const CRenderer::EOrder& eOrder) {

	//Item
	CModel* m_pItem_Model = nullptr;
	if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT])
		m_pItem_Model = m_pMediKit_Model;
	else if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT])
		m_pItem_Model = m_pFlashLight_Model;
	else {}


	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
		m_pShader->Set_RawValue("g_fRefract", &m_fRefract, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness2", &m_fMetalness2, sizeof(_float));
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));

		if (m_bisRender) {
			for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
				m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
					m_pModel->Render(i, m_pShader, 0);
				else
					m_pModel->Render(i, m_pShader, 7);
			}
		}
		if (nullptr != m_pItem_Model /*&& m_bisGetItem*/) {
			m_pShader2->Set_RawValue("g_WorldMatrix", &m_pItemTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pItem_Model->Get_NumMesh(); ++i) {
				m_pItem_Model->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader2, "g_DiffuseTexture");
				if (FAILED(m_pItem_Model->Set_Resource(i, aiTextureType_NORMALS, m_pShader2, "g_NormalTexture")))
					m_pItem_Model->Render(i, m_pShader2, 0);
				else
					m_pItem_Model->Render(i, m_pShader2, 8);
			}

		}

		break;
	}
	case CRenderer::EOrder::SHADOW: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CShadow* pShadow = dynamic_cast<CShadow*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow")));
		pShadow->Set_RawValue(m_pShader);
		pShadow->Set_RawValue(m_pShader2);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			m_pModel->Render(i, m_pShader, 1);
		}
		if (nullptr != m_pItem_Model) {
			m_pShader2->Set_RawValue("g_WorldMatrix", &m_pItemTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pItem_Model->Get_NumMesh(); ++i) {
				m_pItem_Model->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader2, "g_DiffuseTexture");
				m_pItem_Model->Render(i, m_pShader2, 3);
			}
		}
		break;
	}
								  //hwang
	case CRenderer::EOrder::PENETRATE: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		ID3D11ShaderResourceView* pDepthSRV = m_pTarget_Depth->Get_ShaderResourceView();
		m_pShader->Set_Resource("g_DepthTexture", pDepthSRV);
		_float2 fWinSize = _float2{ static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY) };
		m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
		_float4 vPenetrateColor = _float4{ 1.f, 0.f, 0.f, 1.f };
		m_pShader->Set_RawValue("g_vPenetrateColor", &vPenetrateColor, sizeof(_float4));
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			m_pModel->Render(i, m_pShader, 3);
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		//m_pCollider_Foot->Render(pCamera);
		//m_pCollider_Body->Render(pCamera);
		//m_pOtherCamperCollider->Render(pCamera);
		//if (m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT])
			//m_pFlashLightCollider->Render(pCamera);

		break;
	}
	}
	return S_OK;
}


void CCamper_GamePlay::Update_Network(const string& pKey) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	Json::Value root;
	if ("WorldMatrix" == pKey) {
		_float3 vDir = m_pTransform->Get_Float3(CTransform::ERow::LOOK);
		_float3 vPos = m_pTransform->Get_Float3(CTransform::ERow::POS);
		root["Dir"]["x"] = vDir.x;
		root["Dir"]["y"] = vDir.y;
		root["Dir"]["z"] = vDir.z;
		root["Pos"]["x"] = vPos.x;
		root["Pos"]["y"] = vPos.y;
		root["Pos"]["z"] = vPos.z;
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_TarObjID" == pKey) {
		root["TarObjID"] = static_cast<_int>(m_pTarObject->Get_Id());
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_TarObjDir" == pKey) {
		root["TarObjDir"] = static_cast<_int>(m_iTarObjDir);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_SpeedCamper" == pKey) {
		root["SpeedCamper"] = static_cast<_float>(m_fPerkSpeed);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_SpiderTime" == pKey) {
		root["SpiderTime"] = m_fSpiderTime;
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
}

void CCamper_GamePlay::Update_Network(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<Json::Value>& recv = m_pNetwork->Get_Recv(m_iId);
	for (auto iter = recv.begin(); iter != recv.end();) {
		Json::Value root = (*iter);
		string key = root["Key"].asString();
		if ("WorldMatrix" == key) {
			_float3 vPos = {};
			_float3 vDir = {};
			vDir.x = root["Dir"]["x"].asFloat();
			vDir.y = root["Dir"]["y"].asFloat();
			vDir.z = root["Dir"]["z"].asFloat();
			vPos.x = root["Pos"]["x"].asFloat();
			vPos.y = root["Pos"]["y"].asFloat();
			vPos.z = root["Pos"]["z"].asFloat();

			m_vDir = vDir;
			m_vPos = vPos;

			_vector vPrePos = m_pTransform->Get_Row(CTransform::ERow::POS);

			m_pTransform->Set_Row(CTransform::ERow::POS, vPrePos * 0.8f + XMLoadFloat3(&m_vPos) * 0.2f);

			_vector vPrelook = m_pTransform->Get_Row(CTransform::ERow::LOOK);
			_vector vLook = XMVector3Normalize(vPrelook * 0.8f + XMLoadFloat3(&m_vDir) * 0.2f);

			m_pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f);
			m_pTransform->LookAt(m_pTransform->Get_Row(CTransform::ERow::POS) + vLook);

			iter = recv.erase(iter);
		}
		else if ("Set_TarObjID" == key) {
			m_iTarObjID = root["TarObjID"].asInt();
			iter = recv.erase(iter);
		}
		else if ("Set_TarObjDir" == key) {
			m_iTarObjDir = root["TarObjDir"].asInt();
			iter = recv.erase(iter);
		}
		else if ("Set_SpeedCamper" == key) {
			m_fPerkSpeed = root["SpeedCamper"].asFloat();
			iter = recv.erase(iter);
		}
		else if ("Set_SpiderTIme" == key) {
			m_fSpiderTime = root["SpiderTime"].asFloat();
			iter = recv.erase(iter);
		}
		else
			++iter;
	}
}

void CCamper_GamePlay::Set_TarObjID(_int iTarObjID) {
	m_iTarObjID = iTarObjID;
}

void CCamper_GamePlay::Call_BloodEffect(_uint type) {
	//CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//if (type == 0) {
	//	CObject* pCamBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_CamBlood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_CamBlood"));
	//	CObject* pBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood"));
	//	CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
	//	pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 1.5f, 0.f, 0.f));
	//}
	//else if (type == 1) {
	//	CObject* pCamBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_CamBlood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_CamBlood"));
	//	CObject* pBloodEffect = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood"));
	//	CTransform* pTransform = dynamic_cast<CTransform*>(pBloodEffect->Get_Component(TEXT("Transform")));
	//	pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 1.5f, 0.f, 0.f));
	//}
}

void CCamper_GamePlay::Set_Accel(_float3 accel) {
	m_vAccel = accel;
}


void CCamper_GamePlay::Set_FindPerk(_double dDuringTime, _bool bFind) {
	m_dFindPerkTime = dDuringTime;
	m_bFindPerkRender = bFind;
}

const _bool& CCamper_GamePlay::Get_HaveStayCrowPerk() const {
	return m_bHaveStayCrowPerk;
}

void CCamper_GamePlay::Check_Perk(_double dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CInven* pInven = CInven::Get_Instance();
	if (pInven->OkayPerk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER)) == true) {
		pInven->Update(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), dTimeDelta);
		//1.FindCamper
		if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FIND_CAMPER))) {
			list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
			if (nullptr != listCamper) {
				for (auto& iter : *listCamper) {
					CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
					if (pCamper != this)
						pCamper->Set_FindPerk(3.0, true);
				}
			}
		}
		//2.SELF_HEAL -> FS_Camper_HealingSelf
		//3.RunAway ------------------- UI ----- m_bRunAway ->>> false?��?���????��(죽기?���???) -> true?��?�� 
		if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::RUNAWAY))) {
			CUI_Play_Icon_Hud* pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"));
			if (!m_bRunAway && (m_pFM->Get_State() == CFM_Camper::EState::HIT_RUN || m_pFM->Get_State() == CFM_Camper::EState::HIT_WALK) && m_eCollisionState == ECollisionState::NONE) {
				pHud->Set_HudOn(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::PERK1), CUI_Play_Icon_Hud::EKey::EKEY, CUI_Play_Icon_Hud::EDiv::CENTER, CUI_Play_Icon_Hud::EDiv::LEFT);
			}
			if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_E)) {
				if (m_bRunAway == false && (m_pFM->Get_State() == CFM_Camper::EState::HIT_RUN || m_pFM->Get_State() == CFM_Camper::EState::HIT_WALK))
					if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::RUNAWAY))) {
						pHud->Set_HudOff(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::PERK1), true);

						m_fPerkSpeed += 2.5f;
						m_bRunAway = true;
						Update_Network("Set_SpeedCamper");
					}
			}
			CPerk* pPerk = pInven->Get_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::RUNAWAY));
			if (nullptr != pPerk)
				if (m_bRunAway == true && pPerk->Get_DuringTIme() <= 0 && m_bStopRunAway == false) {
					m_fPerkSpeed -= 2.5f;
					m_bStopRunAway = true;
					Update_Network("Set_SpeedCamper");
				}

		}
		//4.ESCAPE_CARRY -> FS_Camper_Carry_Idle -----------  UI --- m_bEscapeCarry ->>> false(결정?��?�� ?���???) -> true 
		if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::ESCAPE_CARRY))) {
			if (!m_bEscapeCarry) {
				// perk R
			}

			else {
				// perk R

			}

		}
		//5.LOCKER -> FS_Camper_Closet_Idle
		//6.STAY_CROW -> m_bHaveStaryCrowPerk
		if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::STAY_CROW))) {
			m_bHaveStayCrowPerk = true;
		}
		//7.END_GENERATOR 
		if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::END_GENERATOR))) {
			CInter_ExitDoor* pExitDoor = dynamic_cast<CInter_ExitDoor*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_ExitDoor")));
			if (pExitDoor->Get_State() == CInter_ExitDoor::ESTATE::CLOSED && m_bEndGene == false) {
				m_fPerkSpeed += 1.f;
				m_bEndGene = true;
				Update_Network("Set_SpeedCamper");
			}
		}
		//8.FIND_SLASHER -> FS_Camper_Generator
		//9.FAST -> FS_Camper_plank,FS_Camper_Fall,FS_Camper_ParkourVault_Jump
		if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FAST))) {
			if (m_bFast == true) {
				m_fPerkSpeed += 1.f;
				m_bFast = false;
				Update_Network("Set_SpeedCamper");
			}
			CPerk* pPerk = pInven->Get_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FAST));
			if (nullptr != pPerk) {
				if (pPerk->Get_DuringTIme() <= 0.0 && m_bStopFast == true) {
					m_fPerkSpeed -= 1.f;
					m_bStopFast = false;
					Update_Network("Set_SpeedCamper");
				}
			}
		}
		//10.NEWBIE -> Inter_Locker, Inter_PlankAnim
		//11.PURIFICATION -> FS_Camper_TotemOut
		//12.FIND_GENERATOR
		if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FIND_GENERATOR))) {
			if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FIND_GENERATOR))) {
				list<CObject*>* listGenerator = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Generator"));
				if (nullptr != listGenerator) {
					for (auto& iter : *listGenerator) {
						CInter_Generator* pGenerator = dynamic_cast<CInter_Generator*>(iter);
						if (pGenerator->Get_State() != CInter_Generator::EState::ON) {
							pGenerator->Set_PerkRender(true);
						}
					}
				}
			}
			CPerk* pPerk = pInven->Get_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FIND_GENERATOR));
			if (nullptr != pPerk) {
				if (pPerk->Get_DuringTIme() <= 0) {
					list<CObject*>* listGenerator = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Generator"));
					if (nullptr != listGenerator) {
						for (auto& iter : *listGenerator) {
							CInter_Generator* pGenerator = dynamic_cast<CInter_Generator*>(iter);
							pGenerator->Set_PerkRender(false);
						}
					}
				}
			}
		}
	}
}

