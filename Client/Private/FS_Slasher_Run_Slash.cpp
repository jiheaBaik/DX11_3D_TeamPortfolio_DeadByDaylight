#include "pch.h"
#include "FS_Slasher_Run_Slash.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"
#include "Inter_Hook.h"
#include "Inter_Totem.h"

CFS_Slasher_Run_Slash::CFS_Slasher_Run_Slash(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Run_Slash::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Run_Slash::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Run_Slash::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	return S_OK;
}

const _ubyte  CFS_Slasher_Run_Slash::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(16))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
		}
		else {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(16))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);
		}

		_vector vLook = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 0.f));
		_vector vRight = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 0.f));

		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		// Input
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
			vDir += vLook;
			m_pSlasher_Huntress->m_bMove[0] = true;
			m_eCurRun = ERun::RUN_FT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W)) {
			m_pSlasher_Huntress->m_bMove[0] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S)) {
			m_pSlasher_Huntress->m_bMove[1] = true;
			m_eCurRun = ERun::RUN_BK;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S)) {
			m_pSlasher_Huntress->m_bMove[1] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D)) {
			vDir += vRight;
			m_pSlasher_Huntress->m_bMove[2] = true;
			m_eCurRun = ERun::RUN_RT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D)) {
			m_pSlasher_Huntress->m_bMove[2] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A)) {
			vDir -= vRight;
			m_pSlasher_Huntress->m_bMove[3] = true;
			m_eCurRun = ERun::RUN_LT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A)) {
			m_pSlasher_Huntress->m_bMove[3] = false;
		}

		if ((true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[1]) || (true == m_pSlasher_Huntress->m_bMove[2] && true == m_pSlasher_Huntress->m_bMove[3]))
			return CFM_Slasher::EState::IDLE_SLASH;

		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
			vDir += vLook;
			vDir += vRight;
			m_eCurRun = ERun::RUN_FTRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
			vDir += vLook;
			vDir -= vRight;
			m_eCurRun = ERun::RUN_FTLT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
			vDir += vRight;
			m_eCurRun = ERun::RUN_BKRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
			vDir -= vRight;
			m_eCurRun = ERun::RUN_BKLT;
		}

		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R) && 0 < m_pSlasher_Huntress->Get_WeaponCount()) {
				m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::HUNT;
				m_pSlasher_Huntress->Update_Network("Set_Type");

				vDir = XMVector3Normalize(vDir);
				XMStoreFloat3(&m_vDir, vDir);
				m_pSlasher_Huntress->m_pFM->Update_Network("Set_T_MoveDir");

				return CFM_Slasher::EState::THROW_IN;
			}
		}
		else {
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R) && true == m_pSlasher_Huntress->m_bSpirit) {
				m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SPIRIT;
				m_pSlasher_Huntress->Update_Network("Set_Type");
				return CFM_Slasher::EState::SPIRIT_CHARGE;
			}
		}

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			CInven* pInven = CInven::Get_Instance();
			CInfo* pInfo = CInfo::Get_Instance();
			if (CSlasher_Huntress::ECollisionState::CAMPER_CRAWL == m_pSlasher_Huntress->m_eCollisionState 
				&& true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");

				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
				pCamper->Set_TarObject(m_pSlasher_Huntress);
				pCamper->Set_TarObjID(m_pSlasher_Huntress->Get_Id());
				pCamper->Update_Network("Set_TarObjID");
				pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::MORI);
				pCamper->Get_FM_Camper()->Update_Network("Set_State");

				m_pSlasher_Huntress->m_iHookCount = 0;
				return CFM_Slasher::EState::MORI;
			}
			else {
				vDir = XMVector3Normalize(vDir);
				XMStoreFloat3(&m_vDir, vDir);
				m_pSlasher_Huntress->m_pFM->Update_Network("Set_MoveDir");

				return CFM_Slasher::EState::ATTACK_SLASH_IN;
			}
		}

		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_SPACE)) {
			if (CSlasher_Huntress::ECollisionState::LOCKER == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				CInter_Locker* pLocker = dynamic_cast<CInter_Locker*>(m_pSlasher_Huntress->m_pTarObject);

				//if (true == pLocker->Get_IsInCamper()) {
				if (pLocker->Get_State() == CInter_Locker::ESTATE::HIDE_CAMPER || pLocker->Get_State() == CInter_Locker::ESTATE::HIDEFAST_CAMPER) {
					pLocker->Set_State(CInter_Locker::ESTATE::TAKEOUT_CAMPER);

					CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), pLocker->Get_CamperID()));
					pCamper->Set_TarObject(m_pSlasher_Huntress);
					pCamper->Set_TarObjID(m_pSlasher_Huntress->Get_Id());
					pCamper->Update_Network("Set_TarObjID");
					pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::CLOSSET_GRAB);
					pCamper->Get_FM_Camper()->Update_Network("Set_State");

					return CFM_Slasher::EState::LOCKER_GRAB;
				}
				else if (true == m_pSlasher_Huntress->m_bSlasher && m_pSlasher_Huntress->m_iWeaponCount < 5) {
					pLocker->Set_State(CInter_Locker::ESTATE::TAKEOUT_WEAPON);
					return CFM_Slasher::EState::LOCKER_RELOAD;
				}
				else {
					pLocker->Set_State(CInter_Locker::ESTATE::OPEN_SLASHER);
					return CFM_Slasher::EState::LOCKER_SEARCH;
				}
			}
			else if (CSlasher_Huntress::ECollisionState::PLANK == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->m_iTarObjDir = m_pSlasher_Huntress->m_iColObjDir;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				m_pSlasher_Huntress->Update_Network("Set_TarObjDir");

				CInter_PlankAnim* pPlank = dynamic_cast<CInter_PlankAnim*>(m_pSlasher_Huntress->m_pTarObject);
				pPlank->Set_State(CInter_PlankAnim::ESTATE::BREAK);
				return CFM_Slasher::EState::DESTROY_PLANK;
			}
			else if (CSlasher_Huntress::ECollisionState::HATCH == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");

				CInter_Hatch* pHatch = dynamic_cast<CInter_Hatch*>(m_pSlasher_Huntress->m_pTarObject);
				pHatch->Set_State(CInter_Hatch::ESTATE::CLOSE_SLASHER);
				return CFM_Slasher::EState::DESTROY_HATCH;
			}
			else if (CSlasher_Huntress::ECollisionState::GENERATOR == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->m_iTarObjDir = m_pSlasher_Huntress->m_iColObjDir;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				m_pSlasher_Huntress->Update_Network("Set_TarObjDir");

				CInter_Generator* pGenerator = dynamic_cast<CInter_Generator*>(m_pSlasher_Huntress->m_pTarObject);
				pGenerator->Set_State(CInter_Generator::EState::CRASH);
				return CFM_Slasher::EState::DESTROY_GENE;
			}
			else if (CSlasher_Huntress::ECollisionState::TOTEM == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");

				//CInter_Totem* pTotem = dynamic_cast<CInter_Totem*>(m_pSlasher_Huntress->m_pTarObject);
				return CFM_Slasher::EState::DESTROY_TOTEM;
			}
			else if (CSlasher_Huntress::ECollisionState::CAMPER_CRAWL == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");

				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
				pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::CRAWL_PICK1);
				pCamper->Get_FM_Camper()->Update_Network("Set_State");

				return CFM_Slasher::EState::PICKUP;
			}
			else if (CSlasher_Huntress::ECollisionState::PARKOUR == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->m_iTarObjDir = m_pSlasher_Huntress->m_iColObjDir;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				m_pSlasher_Huntress->Update_Network("Set_TarObjDir");

				return CFM_Slasher::EState::PARKOUR;
			}
			else if (CSlasher_Huntress::ECollisionState::PARKOUR_VAULT == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->m_iTarObjDir = m_pSlasher_Huntress->m_iColObjDir;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				m_pSlasher_Huntress->Update_Network("Set_TarObjDir");

				return CFM_Slasher::EState::PARKOUR_VAULT;
			}
			else if (CSlasher_Huntress::ECollisionState::PARKOUR_GRAB == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");

				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
				pCamper->Set_TarObject(m_pSlasher_Huntress);
				pCamper->Set_TarObjID(m_pSlasher_Huntress->Get_Id());
				pCamper->Update_Network("Set_TarObjID");

				CTransform* pCamperTransform = dynamic_cast<CTransform*>(pCamper->Get_Component(TEXT("Transform")));
				_matrix MatrixInverse = pCamperTransform->Get_Matrix_Inverse();
				_float3 vLocal;
				XMStoreFloat3(&vLocal, XMVector3TransformCoord(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS), MatrixInverse));
				if (vLocal.z > 0) {
					pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::PARKOUR_GRAB_F_IN);
					pCamper->Get_FM_Camper()->Update_Network("Set_State");
					return CFM_Slasher::EState::GRAB_PARKOUR_FT_IN;
				}
				else {
					pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::PARKOUR_GRAB_IN_B);
					pCamper->Get_FM_Camper()->Update_Network("Set_State");
					return CFM_Slasher::EState::GRAB_PARKOUR_BK_IN;
				}
			}
			else if (CSlasher_Huntress::ECollisionState::GENE_GRAB == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");

				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
				pCamper->Set_TarObject(m_pSlasher_Huntress);
				pCamper->Set_TarObjID(m_pSlasher_Huntress->Get_Id());
				pCamper->Update_Network("Set_TarObjID");

				pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::GENERATOR_GRAB_IN);
				pCamper->Get_FM_Camper()->Update_Network("Set_State");
				return CFM_Slasher::EState::GRAB_GENE_IN;
			}
		}
		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (false == m_pSlasher_Huntress->m_bCollider_Move) {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
		}
		else {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
			}
		}


		if (false == m_pSlasher_Huntress->m_bMove[0] && false == m_pSlasher_Huntress->m_bMove[1] && false == m_pSlasher_Huntress->m_bMove[2] && false == m_pSlasher_Huntress->m_bMove[3])
			return CFM_Slasher::EState::IDLE_SLASH;
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);

		if (m_ePreRun == m_eCurRun)
			return CFM_Slasher::EState::END;
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_FTRT;
			return CFM_Slasher::EState::RUN_SLASH_FTRT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_FTLT;
			return CFM_Slasher::EState::RUN_SLASH_FTLT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_BKRT;
			return CFM_Slasher::EState::RUN_SLASH_BKRT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_BKLT;
			return CFM_Slasher::EState::RUN_SLASH_BKLT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true) {
			m_ePreRun = ERun::RUN_FT;
			return CFM_Slasher::EState::RUN_SLASH_FT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true) {
			m_ePreRun = ERun::RUN_BK;
			return CFM_Slasher::EState::RUN_SLASH_BK;
		}
		if (m_pSlasher_Huntress->m_bMove[2] == true) {
			m_ePreRun = ERun::RUN_RT;
			return CFM_Slasher::EState::RUN_SLASH_RT;
		}
		if (m_pSlasher_Huntress->m_bMove[3] == true) {
			m_ePreRun = ERun::RUN_LT;
			return CFM_Slasher::EState::RUN_SLASH_LT;
		}
	}
	else {
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(16))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}
		else {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(16))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}

		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (false == m_pSlasher_Huntress->m_bCollider_Move) {
			if (CFM_Slasher::EState::RUN_SLASH_FTRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SLASH_FTLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SLASH_BKRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SLASH_BKLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SLASH_FT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SLASH_BK == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SLASH_RT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SLASH_LT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
		}
		else {
			//if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[0]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[1]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[2]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[3]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
			//}
		}
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Run_Slash::Get_Dir(void) {
	return m_vDir;
}
