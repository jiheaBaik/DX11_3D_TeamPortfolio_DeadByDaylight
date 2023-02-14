#include "pch.h"
#include "FS_Slasher_Idle_Slash.h"
#include "Slasher_Huntress.h"
#include "Weapon_Spirit.h"
#include "Camper_GamePlay.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"
#include "Inter_Hook.h"
#include "Inter_Totem.h"
#include "Inter_Parkour.h"

CFS_Slasher_Idle_Slash::CFS_Slasher_Idle_Slash(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
}

void CFS_Slasher_Idle_Slash::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Idle_Slash::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Idle_Slash::Init(_uint iAnimIndex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pSlasher_Huntress->m_bFlag = false;
	m_pSlasher_Huntress->m_dFootTime = 0.;

	if (true == m_pSlasher_Huntress->m_bSlasher) {
		if (19 == iAnimIndex) {
			m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(31, 0.5f);
		}
		else {
			m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
		}
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(25, 0.5f);
		m_pSlasher_Huntress->m_pModel->Set_Anim(25);

		CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
		pWeapon_Spirit->Render_Off();
	}

	return S_OK;
}

const _ubyte  CFS_Slasher_Idle_Slash::Update(const _double& dTimeDelta) {
	CInfo* pInfo = CInfo::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl && 0 < pInfo->Get_CamperCount()) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(46))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(54))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(96))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_2D);
		}

		if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(20) && true == m_pSlasher_Huntress->m_bControl && true == m_pSlasher_Huntress->m_pFM->Get_CamMori()) {
			m_pSlasher_Huntress->m_pFM->Set_CamMori(false);

			CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
			CTransform* pCamera_Transform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Transform")));

			_matrix SlasherBoneMatrix = m_pSlasher_Huntress->m_pModel_FPV->Get_BoneMatrix("joint_Cam_01");
			_matrix SocketMatrix;
			SocketMatrix = SlasherBoneMatrix * m_pSlasher_Huntress->m_pTransform->Get_Matrix();

			pCamera_Transform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
			pCamera_Transform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
			pCamera_Transform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
			pCamera_Transform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);
		}

		// Input
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W))
			m_pSlasher_Huntress->m_bMove[0] = false;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S))
			m_pSlasher_Huntress->m_bMove[1] = false;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D))
			m_pSlasher_Huntress->m_bMove[2] = false;
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A))
			m_pSlasher_Huntress->m_bMove[3] = false;

		if ((false == m_pSlasher_Huntress->m_bMove[0] || false == m_pSlasher_Huntress->m_bMove[1])
			&& (false == m_pSlasher_Huntress->m_bMove[2] || false == m_pSlasher_Huntress->m_bMove[3])) {
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_W) & 0x80)
				return CFM_Slasher::EState::RUN_SLASH_FT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_S) & 0x80)
				return CFM_Slasher::EState::RUN_SLASH_BK;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_D) & 0x80)
				return CFM_Slasher::EState::RUN_SLASH_RT;
			if (pGame_Instance->Get_Input_KeyboardKey(DIK_A) & 0x80)
				return CFM_Slasher::EState::RUN_SLASH_LT;
		}
		// 도끼 상태일때 인게임 보고 수정 해야함
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R) && 0 < m_pSlasher_Huntress->Get_WeaponCount()) {
				m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::HUNT;
				m_pSlasher_Huntress->Update_Network("Set_Type");
				m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Set_T_MoveDir");
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

				//m_pSlasher_Huntress->m_iHookCount = 0;
				return CFM_Slasher::EState::MORI;
			}
			else {
				m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Set_MoveDir");
				return CFM_Slasher::EState::ATTACK_SLASH_IN;
			}
		}

		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_SPACE)) {
			if (CSlasher_Huntress::ECollisionState::LOCKER == m_pSlasher_Huntress->m_eCollisionState) {
				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				CInter_Locker* pLocker = dynamic_cast<CInter_Locker*>(m_pSlasher_Huntress->m_pTarObject);

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
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
		//m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
		//m_pSlasher_Huntress->m_pModel_FPV->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
	}
	else {
		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(46))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(54))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(96))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_haiti_fingers_crack"), 6, -1, 1.f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
		//m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
		//m_pSlasher_Huntress->m_pModel_FPV->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Idle_Slash::Get_Dir(void) {
	return m_vDir;
}
