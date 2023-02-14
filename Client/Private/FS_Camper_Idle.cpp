#include "pch.h"
#include "FS_Camper_Idle.h"
#include "Camper_GamePlay.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_ChestBox.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"
#include "Inter_ExitDoor.h"
#include "Inter_Parkour.h"
#include "Inter_Totem.h"
#include "Camera_Camper.h"


CFS_Camper_Idle::CFS_Camper_Idle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Idle::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Camper_Idle::Init(_char* pModelName) {

	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);

	if (CFM_Camper::EState::SIT == m_pCamper_GamePlay->Get_FM_Camper()->m_eState ||
		CFM_Camper::EState::HIT_SIT == m_pCamper_GamePlay->Get_FM_Camper()->m_eState) {
		m_pCamper_GamePlay->m_pCollider->Set_Center(_float3(0.f, 0.55f, 0.f));
		m_pCamper_GamePlay->m_pCollider->Set_Extents(_float3(0.25f, 0.55f, 0.25f));
	}
	else {
		m_pCamper_GamePlay->m_pCollider->Set_Center(_float3(0.f, 0.8f, 0.f));
		m_pCamper_GamePlay->m_pCollider->Set_Extents(_float3(0.25f, 0.8f, 0.25f));
	}

	if (m_pCamper_GamePlay->m_bControl)
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_walk"), 10, -1, 0.2f, FMOD_2D);
	else
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_walk"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });

	m_pCamper_GamePlay->m_bFlag = false;
	return S_OK;
}

const HRESULT CFS_Camper_Idle::Init(_char* pModelName, _double dLerpTime)
{

	if (CFM_Camper::EState::SIT == m_pCamper_GamePlay->Get_FM_Camper()->m_eState ||
		CFM_Camper::EState::HIT_SIT == m_pCamper_GamePlay->Get_FM_Camper()->m_eState) {
		m_pCamper_GamePlay->m_pCollider->Set_Center(_float3(0.f, 0.55f, 0.f));
		m_pCamper_GamePlay->m_pCollider->Set_Extents(_float3(0.25f, 0.55f, 0.25f));
	}
	else {
		m_pCamper_GamePlay->m_pCollider->Set_Center(_float3(0.f, 0.8f, 0.f));
		m_pCamper_GamePlay->m_pCollider->Set_Extents(_float3(0.25f, 0.8f, 0.25f));
	}

	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName, dLerpTime);
	m_pCamper_GamePlay->m_bFlag = false;
	return S_OK;
}


const _ubyte  CFS_Camper_Idle::Update(const _double& dTimeDelta) {
	if (true == m_pCamper_GamePlay->m_bControl) {
		if (!m_pCamper_GamePlay->m_bIsCollider_Foot && XMVectorGetY(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS)) > 0.5f)
			m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::FALL);

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CObject* pCamera = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Camera_Camper"));
		CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Transform")));
		_vector vLook = pCameraTransform->Get_Row(CTransform::ERow::LOOK) * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_vector vRight = pCameraTransform->Get_Row(CTransform::ERow::RIGHT) * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_bool bDir = false;
		//Input
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W))
			vDir += vLook;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S))
			vDir -= vLook;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A))
			vDir -= vRight;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D))
			vDir += vRight;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W) ||
			pGame_Instance->Get_Input_KeyboardKey(DIK_S) ||
			pGame_Instance->Get_Input_KeyboardKey(DIK_A) ||
			pGame_Instance->Get_Input_KeyboardKey(DIK_D)) {
			vDir = XMVector3Normalize(vDir);
			XMStoreFloat3(&m_pCamper_GamePlay->m_vDotDir, vDir);
			m_pCamper_GamePlay->Get_FM_Camper()->Update_Network("Set_Dir");
			return CFM_Camper::EState::WALK_F;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F8))
			return CFM_Camper::EState::HIT_CRAWL_F;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W))
			return CFM_Camper::EState::WALK_F;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S))
			return CFM_Camper::EState::WALK_F;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A))
			return CFM_Camper::EState::WALK_F;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D))
			return CFM_Camper::EState::WALK_F;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_LCONTROL))
			return CFM_Camper::EState::SIT;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_F1))
			return CFM_Camper::EState::HERE;
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_F2))
			return CFM_Camper::EState::POINTTO;
		/*if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F9))
			return CFM_Camper::EState::Hit_F;*/
		//if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F9))
		//	return CFM_Camper::EState::Hit_Crawl_F;

		if (m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT]) {

			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R)) {
				return CFM_Camper::EState::FLASHLIGHT;
			}
		}
		if (pGame_Instance->Get_Input_MouseButton(CInput::EMouseButton::L)) {

			switch (m_pCamper_GamePlay->m_eCollisionState) {
			case CCamper_GamePlay::ECollisionState::CAMPER: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				CCamper_GamePlay* pCamper = static_cast<CCamper_GamePlay*>(m_pCamper_GamePlay->m_pTarObject);
				if (CFM_Camper::EState::HOOKOUT == pCamper->m_pFM->m_eState ||
					CFM_Camper::EState::HOOKIDLE == pCamper->m_pFM->m_eState || 
					CFM_Camper::EState::SPIDER_IN == pCamper->m_pFM->m_eState ||
					CFM_Camper::EState::SPIDER_LOOP == pCamper->m_pFM->m_eState ||
					CFM_Camper::EState::SPIDER_OUT == pCamper->m_pFM->m_eState) {
					//Save to Hook
					pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::HOOKRESCUEIN_BEEING);
					pCamper->Get_FM_Camper()->Update_Network("Set_State");
					return CFM_Camper::EState::HOOKRESCUEIN;
				}
				else {//Heal
					if (pCamper->m_pFM->m_eState != CFM_Camper::EState::BEINGHEALED && pCamper->m_pFM->m_eState != CFM_Camper::EState::BEINGHEALED_CRAW) {
						if (1 == pCamper->m_iHP) {
							pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::BEINGHEALED);
							pCamper->Get_FM_Camper()->Update_Network("Set_State");
							return CFM_Camper::EState::HEALCAMPER;
						}//ECollisionState m_eCollisionState = ECollisionState::NONE
						else if (0 == pCamper->m_iHP && pCamper->m_pFM->m_eState != CFM_Camper::EState::HOOKIN &&
							pCamper->m_pFM->m_eState != CFM_Camper::EState::HOOKOUT && pCamper->m_pFM->m_eState != CFM_Camper::EState::HOOKFREE &&
							pCamper->m_pFM->m_eState != CFM_Camper::EState::HOOKSTRUGGLE && pCamper->m_pFM->m_eState != CFM_Camper::EState::HOOKIDLE &&
							pCamper->m_pFM->m_eState != CFM_Camper::EState::HOOKRESCUEIN_BEEING && pCamper->m_pFM->m_eState != CFM_Camper::EState::HOOKRESCUEEND_BEEING &&
							pCamper->m_pFM->m_eState != CFM_Camper::EState::SPIDER_IN && pCamper->m_pFM->m_eState != CFM_Camper::EState::SPIDER_LOOP &&
							pCamper->m_pFM->m_eState != CFM_Camper::EState::SPIDER_OUT && pCamper->m_pFM->m_eState != CFM_Camper::EState::SPIDER_KILLIN &&
							pCamper->m_pFM->m_eState != CFM_Camper::EState::SPIDER_KILLLOOP && pCamper->m_pFM->m_eState != CFM_Camper::EState::SPIDER_KILLOUT &&
							pCamper->m_pFM->m_eState != CFM_Camper::EState::SPIDER_STRUGGLE) {
							pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::BEINGHEALED_CRAW);
							pCamper->Get_FM_Camper()->Update_Network("Set_State");
							return CFM_Camper::EState::HEALCAMPERCRAWL;
						}
					}
				}
				break;
			}
			case CCamper_GamePlay::ECollisionState::ChestBox: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");

				CInter_ChestBox* pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);

				if (pChestBox->Get_State() == CInter_ChestBox::ESTATE::IDLE) {
					pChestBox->Set_State(CInter_ChestBox::ESTATE::OPEN_STEP1);
					return CFM_Camper::EState::CHEST_IN;
				}
				if (pChestBox->Get_State() == CInter_ChestBox::ESTATE::OPEN_STEP3 && !pChestBox->Get_IsUse() &&
					!m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] &&
					!m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT])
					return CFM_Camper::EState::CHEST_PICKIN;
				break;
			}
			case CCamper_GamePlay::ECollisionState::GENERATOR: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->m_iTarObjDir = m_pCamper_GamePlay->m_iColObjDir;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				m_pCamper_GamePlay->Update_Network("Set_TarObjDir");

				enum Dir { FRONT, BACK, LEFT, RIGHT, END };
				CInter_Generator* pGenerator = static_cast<CInter_Generator*>(m_pCamper_GamePlay->Get_TarObject());
				if (CInter_Generator::EState::ON != pGenerator->Get_State() && CInter_Generator::EState::LOCK_IN != pGenerator->Get_State()
					&& CInter_Generator::EState::LOCK_IDLE != pGenerator->Get_State() && CInter_Generator::EState::LOCK_END != pGenerator->Get_State()) {
					switch (m_pCamper_GamePlay->m_iTarObjDir) {
					case FRONT:
						if (!pGenerator->Get_Exist(FRONT)) {
							return CFM_Camper::EState::GENERATOR_IDLEF;
						}
					case BACK:
						if (!pGenerator->Get_Exist(BACK)) {
							return CFM_Camper::EState::GENERATOR_IDLEB;
						}
					case LEFT:
						if (!pGenerator->Get_Exist(LEFT)) {
							return CFM_Camper::EState::GENERATOR_IDLEL;
						}
					case RIGHT:
						if (!pGenerator->Get_Exist(RIGHT)) {
							return CFM_Camper::EState::GENERATOR_IDLER;
						}
					}
				}
				break;
			}
			case CCamper_GamePlay::ECollisionState::TOTEM: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				return CFM_Camper::EState::TOTEM_IN;
			}
			case CCamper_GamePlay::ECollisionState::EXITDOOR: {
				CInter_ExitDoor* pEixtDoor = static_cast<CInter_ExitDoor*>(m_pCamper_GamePlay->m_pColObject);
				if (pEixtDoor->Get_State() == CInter_ExitDoor::ESTATE::CLOSED ) {
					m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
					CInter_ExitDoor* pExitDoor = static_cast<CInter_ExitDoor*>(m_pCamper_GamePlay->m_pTarObject);
					m_pCamper_GamePlay->Update_Network("Set_TarObjID");

					pExitDoor->Set_State(CInter_ExitDoor::ESTATE::GEAR, true);
					return CFM_Camper::EState::EXITDOOR_IN;
				}
				break;
			}
			}
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_SPACE)) {
			switch (m_pCamper_GamePlay->m_eCollisionState) {

			case CCamper_GamePlay::ECollisionState::LOCKER: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");

				CInter_Locker* pLocker = static_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);

				if (pLocker->Get_State() == CInter_Locker::ESTATE::IDLE) {
					pLocker->Set_CamperID(m_pCamper_GamePlay->Get_Id());
					pLocker->Update_Network("Set_CamperID");
					pLocker->Set_State(CInter_Locker::ESTATE::HIDE_CAMPER);
					return CFM_Camper::EState::CLOSSET_HIDE;
				}
				else if (pLocker->Get_State() == CInter_Locker::ESTATE::HIDE_CAMPER) {
					pLocker->Set_State(CInter_Locker::ESTATE::PEEKING_LOCKE);
					return CFM_Camper::EState::CLOSSET_ENTERING;
				}
				break;
			}
			case CCamper_GamePlay::ECollisionState::PLANK: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->m_iTarObjDir = m_pCamper_GamePlay->m_iColObjDir;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				m_pCamper_GamePlay->Update_Network("Set_TarObjDir");
				enum Dir { LEFT, RIGHT, END };
				CInter_PlankAnim* pPlank = static_cast<CInter_PlankAnim*>(m_pCamper_GamePlay->m_pTarObject);

				switch (m_pCamper_GamePlay->m_iTarObjDir) {
				case LEFT:
					if ((pPlank->Get_State() == CInter_PlankAnim::ESTATE::DOWNLOOP || pPlank->Get_State() == CInter_PlankAnim::ESTATE::FALLDOWN) &&
						pPlank->Get_State() != CInter_PlankAnim::ESTATE::BREAK)
						return CFM_Camper::EState::_JUMPOVER_FASTR;
					else if (pPlank->Get_State() == CInter_PlankAnim::ESTATE::IDLE) {
						pPlank->Set_State(CInter_PlankAnim::ESTATE::FALLDOWN);
						return CFM_Camper::EState::PULLDOWN_PLANK_R;
					}
					break;
				case RIGHT:
					if ((pPlank->Get_State() == CInter_PlankAnim::ESTATE::DOWNLOOP || pPlank->Get_State() == CInter_PlankAnim::ESTATE::FALLDOWN) &&
						pPlank->Get_State() != CInter_PlankAnim::ESTATE::BREAK)
						return CFM_Camper::EState::_JUMPOVER_FASTL;
					else if (pPlank->Get_State() == CInter_PlankAnim::ESTATE::IDLE) {
						pPlank->Set_State(CInter_PlankAnim::ESTATE::FALLDOWN);
						return CFM_Camper::EState::PULLDOWN_PLANK_L;
					}
					break;
				}
				break;
			}
			case CCamper_GamePlay::ECollisionState::HATCH: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				return CFM_Camper::EState::HATCH_IN;
			}
			case CCamper_GamePlay::ECollisionState::PARKOUR: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->m_iTarObjDir = m_pCamper_GamePlay->m_iColObjDir;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				m_pCamper_GamePlay->Update_Network("Set_TarObjDir");
				return CFM_Camper::EState::PARKOURVAULT_JUMP;
			}
			case CCamper_GamePlay::ECollisionState::PARKOUR_VAULT: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->m_iTarObjDir = m_pCamper_GamePlay->m_iColObjDir;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				m_pCamper_GamePlay->Update_Network("Set_TarObjDir");
				return CFM_Camper::EState::PARKOURVAULT_JUMP;
			}
																 //HOOK
			//case CCamper_GamePlay::ECollisionState::HOOK: {
			//	m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
			//	m_pCamper_GamePlay->Update_Network("Set_TarObjID");
			//	return CFM_Camper::EState::HookIn;
			//}
			}
		}
		//Anim
		if (!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT")) {
			if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 5.f, false))
			{
				if (2 == m_pCamper_GamePlay->Get_Hp())
					return CFM_Camper::EState::IDLE;
				else if (1 == m_pCamper_GamePlay->Get_Hp())
					return CFM_Camper::EState::HIT_IDLE;
			}
		}
		else if (!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_StandWalk_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_StandRun_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_StandWalk_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_StandRun_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|M_StandWalk_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|M_StandRun_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_StandWalk_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_StandRun_To_StandIdle_startFootLT")) {
			if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 3.f, false))
			{

				if (2 == m_pCamper_GamePlay->Get_Hp())
					return CFM_Camper::EState::IDLE;
				else if (1 == m_pCamper_GamePlay->Get_Hp())
					return CFM_Camper::EState::HIT_IDLE;
			}
		}
		else
			m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);

	}
	else {

		if (!m_pCamper_GamePlay->m_bIsCollider_Foot)
			m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::FALL);


		//Anim
		if (!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT")) {
			if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 5.f, false))
			{
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				}

			}
		}
		else if (!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_StandWalk_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "S22_DSkeleton_REF.ao|F_StandRun_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_StandRun_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "MT_DSkeleton_REF.ao|F_StandRun_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|M_StandWalk_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "BO_DSkeleton_REF.ao|M_StandRun_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_StandWalk_To_StandIdle_startFootLT") ||
			!strcmp(m_pCamper_GamePlay->m_pModel->Get_Anim_Name(), "DF_DSkeleton_REF.ao|M_StandRun_To_StandIdle_startFootLT")) {
			if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 2.f, false))
			{
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					if (2 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_GamePlay->Get_Hp())
						m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				}

			}

		}
		else
			m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	}

	return CFM_Camper::EState::END;
}
