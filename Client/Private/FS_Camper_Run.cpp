#include "pch.h"
#include "FS_Camper_Run.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_Generator.h"
#include "Cube_Free.h"
#include "Camera_Camper.h"


CFS_Camper_Run::CFS_Camper_Run(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Run::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Run::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);

	XMStoreFloat3(&m_fPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));
	m_pCamper_GamePlay->m_bFlag = false;

	if (m_pCamper_GamePlay->m_bControl)
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
	else
		m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });

	return S_OK;
}

const _ubyte  CFS_Camper_Run::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {

		if (m_pCamper_GamePlay->m_pFM->m_eState == m_pCamper_GamePlay->m_pFM->RUN) {
			if (CCamper_GamePlay::ECamperKind::CHERYL == m_pCamper_GamePlay->m_eCampKind || 
				CCamper_GamePlay::ECamperKind::MEG == m_pCamper_GamePlay->m_eCampKind) {
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(9))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(10))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(19))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(29))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(39))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(47))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(56))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(65))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(72))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
			}
			else {
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(13))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(22))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(32))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(42))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(52))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(61))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(72))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(79))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
			}
		}
		else if (m_pCamper_GamePlay->m_pFM->m_eState == m_pCamper_GamePlay->m_pFM->HIT_RUN) {
			if (CCamper_GamePlay::ECamperKind::CHERYL == m_pCamper_GamePlay->m_eCampKind ||
				CCamper_GamePlay::ECamperKind::MEG == m_pCamper_GamePlay->m_eCampKind) {
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(10))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(19))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(28))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(37))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(44))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(55))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(62))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(71))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
			}
			else {
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(9))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(19))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(28))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(37))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(46))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(55))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(63))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(71))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_2D);
			}
		}

		if (m_pCamper_GamePlay->m_iHP == 2) {
			m_dSoundTime += dTimeDelta;
			if (m_dSoundTime >= 1.5) {
				m_dSoundTime = 0.;
				
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_running"), 26, -1, 0.3f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_running"), 15, -1, 0.3f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_running"), 17, -1, 0.3f, FMOD_2D);
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_running"), 16, -1, 0.3f, FMOD_2D);
					break;
				}
			}
		}

		CObject* pObj = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Camera_Camper"));

		CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(TEXT("Transform")));

		if (pTransform == nullptr)
			return CFM_Camper::EState::IDLE;

		_uint iDir[4] = { 0, };
		_vector vLook = pTransform->Get_Row(CTransform::ERow::LOOK) * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_vector vRight = pTransform->Get_Row(CTransform::ERow::RIGHT) * XMVectorSet(1.f, 0.f, 1.f, 0.f);
		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		if (!m_pCamper_GamePlay->m_bIsCollider_Foot && XMVectorGetY(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS)) > 0.5f)
			m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::FALL);

		if (m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT]) {
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R)) {
				return CFM_Camper::EState::FLASHLIGHT;
			}
		}

		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
			vDir += vLook;
			iDir[0] = 1;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S)) {
			vDir -= vLook;
			iDir[1] = 1;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A)) {
			vDir -= vRight;
			iDir[2] = 1;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D)) {
			vDir += vRight;
			iDir[3] = 1;
		}

		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_LSHIFT))
		{
			vDir = XMVector3Normalize(vDir);
			XMStoreFloat3(&m_pCamper_GamePlay->m_vDotDir, vDir);
			m_pCamper_GamePlay->Get_FM_Camper()->Update_Network("Set_Dir");

			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::WALK_F;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_WALK;
		}

		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W))
			iDir[0] = 0;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S))
			iDir[1] = 0;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A))
			iDir[2] = 0;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D))
			iDir[3] = 0;
		/*	if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_1))
				return CFM_Camper::EState::Hit_F;*/


		if (!iDir[0] && !iDir[1] && !iDir[2] && !iDir[3]) {
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::RUNTO_IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R)) {
			if (m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT])
				return CFM_Camper::EState::FLASHLIGHT;
		}

		if (pGame_Instance->Get_Input_MouseButton(CInput::EMouseButton::L)) {
			switch (m_pCamper_GamePlay->Get_CollisionState()) {
			case CCamper_GamePlay::ECollisionState::CAMPER: {
				m_pCamper_GamePlay->m_pTarObject = m_pCamper_GamePlay->m_pColObject;
				m_pCamper_GamePlay->Update_Network("Set_TarObjID");
				CCamper_GamePlay* pCamper = static_cast<CCamper_GamePlay*>(m_pCamper_GamePlay->m_pTarObject);
				if (CFM_Camper::EState::HOOKOUT == pCamper->m_pFM->m_eState ||
					CFM_Camper::EState::HOOKIDLE == pCamper->m_pFM->m_eState) {
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
						}
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
				if (CInter_Generator::EState::ON != pGenerator->Get_State()) {
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
				if (pEixtDoor->Get_State() == CInter_ExitDoor::ESTATE::CLOSED) {
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
			switch (m_pCamper_GamePlay->Get_CollisionState()) {
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
					else {
						pPlank->Set_State(CInter_PlankAnim::ESTATE::FALLDOWN);
						return CFM_Camper::EState::PULLDOWN_PLANK_R;
					}
					break;
				case RIGHT:
					if ((pPlank->Get_State() == CInter_PlankAnim::ESTATE::DOWNLOOP || pPlank->Get_State() == CInter_PlankAnim::ESTATE::FALLDOWN) &&
						pPlank->Get_State() != CInter_PlankAnim::ESTATE::BREAK)
						return CFM_Camper::EState::_JUMPOVER_FASTL;
					else {
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
			}
		}
		_float fPerkSpeed = m_pCamper_GamePlay->m_fPerkSpeed;

		vDir = XMVector3Normalize(vDir);
		_float fDotDir = XMVectorGetX(XMVector3Dot(XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f), vDir));
		if (fDotDir != 1.f) {
			XMStoreFloat3(&m_pCamper_GamePlay->m_vDotDir, vDir);
			m_pCamper_GamePlay->Get_FM_Camper()->Update_Network("Set_Dir");
		}
		XMStoreFloat3(&m_pCamper_GamePlay->m_vDotDir, vDir);
		if (false == m_pCamper_GamePlay->m_bCollider_Move)
			m_pCamper_GamePlay->m_pTransform->Translate(vDir, 4.f + fPerkSpeed, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCamper_GamePlay->m_vCollider_Move), vDir));
			if (fDotTarget < 0)
				m_pCamper_GamePlay->m_pTransform->Translate(vDir, 4.f + fPerkSpeed, dTimeDelta);
		}

		_vector vDot = XMVector3Dot(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);
		_vector vCross = XMVector3Cross(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);

		if (XMVectorGetY(vCross) > 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
		}
		else if (XMVectorGetY(vCross) < 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), -abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
		}
	}
	else {

		if (m_pCamper_GamePlay->m_pFM->m_eState == m_pCamper_GamePlay->m_pFM->RUN) {
			if (CCamper_GamePlay::ECamperKind::CHERYL == m_pCamper_GamePlay->m_eCampKind ||
				CCamper_GamePlay::ECamperKind::MEG == m_pCamper_GamePlay->m_eCampKind) {

				if (CCamper_GamePlay::ECamperKind::CHERYL == m_pCamper_GamePlay->m_eCampKind || CCamper_GamePlay::ECamperKind::MEG == m_pCamper_GamePlay->m_eCampKind) {
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(9))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(10))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(19))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(29))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(39))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(47))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(56))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(65))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(72))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				}
				else{
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(13))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(22))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(32))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(42))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(52))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(61))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(72))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
					if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(79))
						m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				}
			}
		}
		else if (m_pCamper_GamePlay->m_pFM->m_eState == m_pCamper_GamePlay->m_pFM->HIT_RUN) {
			if (CCamper_GamePlay::ECamperKind::CHERYL == m_pCamper_GamePlay->m_eCampKind ||
				CCamper_GamePlay::ECamperKind::MEG == m_pCamper_GamePlay->m_eCampKind) {
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(10))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(19))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(28))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(37))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(44))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(55))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(62))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(71))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
			}
			else {
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(0))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(9))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(19))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(28))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(37))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(46))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(55))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(63))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
				if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(71))
					m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_footsteps_boots_concrete_run"), 10, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 1.f });
			}
		}
		_float fPerkSpeed = m_pCamper_GamePlay->m_fPerkSpeed;	//hwang

	if (m_pCamper_GamePlay->m_iHP == 2) {
		m_dSoundTime += dTimeDelta;
		if (m_dSoundTime >= 1.5) {
			m_dSoundTime = 0.;
			
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f11_state_running"), 26, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_f01_state_running"), 15, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m04_state_running"), 17, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("vo_svr_m01_state_running"), 16, -1, 0.3f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
				break;
			}
		}
	}

		if (false == m_pCamper_GamePlay->m_bCollider_Move)
			m_pCamper_GamePlay->m_pTransform->Translate(XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f), 4.f + fPerkSpeed, dTimeDelta);
		else {
			_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCamper_GamePlay->m_vCollider_Move), XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f)));
			if (fDotTarget < 0)
				m_pCamper_GamePlay->m_pTransform->Translate(XMVectorSet(m_pCamper_GamePlay->m_vDotDir.x, m_pCamper_GamePlay->m_vDotDir.y, m_pCamper_GamePlay->m_vDotDir.z, 0.f), 4.f + fPerkSpeed, dTimeDelta);
		}

		_vector vDir = XMLoadFloat3(&m_pCamper_GamePlay->m_vDotDir);
		_vector vDot = XMVector3Dot(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);
		_vector vCross = XMVector3Cross(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::LOOK) *= XMVectorSet(1.f, 0.f, 1.f, 0.f), vDir);

		if (XMVectorGetY(vCross) > 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(480.f), dTimeDelta);
		}
		else if (XMVectorGetY(vCross) < 0.f) {
			if (XMVectorGetX(vDot) > 0.f) {
				if (abs(acos(XMVectorGetX(vDot))) > XMConvertToRadians(480.f) * dTimeDelta)
					m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
				else {
					if (!isnan(abs(acos(XMVectorGetX(vDot)))))
						m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), -abs(acos(XMVectorGetX(vDot))), 1);
				}
			}
			else
				m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-480.f), dTimeDelta);
		}
	}
	// SSD - 221110 by JSY
	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player()) {
		m_fDist += XMVectorGetX(XMVector3Length(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) - XMLoadFloat3(&m_fPrePos)));
		XMStoreFloat3(&m_fPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));
		if (m_fDist > 3.f) {
			CCube_Free* pCrack = dynamic_cast<CCube_Free*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Cube_Crack"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Cube_Crack")));
			pCrack->Set_Pos(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) - XMVectorSet(0.f, 0.f, 0.f, 0.f));
			m_fDist = 0.f;
		}
	}
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
