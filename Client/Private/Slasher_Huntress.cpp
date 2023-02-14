#include "pch.h"
#include "Slasher_Huntress.h"
#include "FM_Slasher.h"
#include "FM_Spirit.h"
#include "Shadow.h"
#include "Camper_GamePlay.h"
#include "Effect_Free3.h"
#include "Inven.h"
#include "Perk.h"
#include "Inter_Generator.h"
#include "Inter_ExitDoor.h"
#include "Inter_PlankAnim.h"
#include "UI_Play_Icon_Hud.h"
#include "Inter_ExitDoor.h"
#include "Prop_EscapeBlocker.h"
#include "Setting_BGM.h"
#include "UI_Play_Score.h"
#include "UI_Play_Gauge_Inter.h"

CSlasher_Huntress* const CSlasher_Huntress::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CSlasher_Huntress* pInstance = new CSlasher_Huntress(pDevice, pContext, pNetwork);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CObject* const CSlasher_Huntress::Clone(void* const& pArg) {
	CSlasher_Huntress* pInstance = new CSlasher_Huntress(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CSlasher_Huntress::CSlasher_Huntress(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CSlasher_Huntress::CSlasher_Huntress(const CSlasher_Huntress& rhs)
	: CObject(rhs) {
}

void CSlasher_Huntress::Delete(void) {
	__super::Delete();
	Safe_Release(m_pNavi);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pModel2);
	Safe_Release(m_pModel3);
	Safe_Release(m_pModel_FPV);
	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform_GrabLocker);
	Safe_Release(m_pCollider_GrabLocker);
	Safe_Release(m_pTransform_Grab);
	Safe_Release(m_pCollider_Grab);
	Safe_Release(m_pFM);
	Safe_Release(m_pCollider_Foot);
	Safe_Release(m_pCollider_Body);
	Safe_Release(m_pCollider_Eyes);
	Safe_Release(m_pAudio);
}

const HRESULT CSlasher_Huntress::Init_Create(void) {
	return S_OK;
}

const HRESULT CSlasher_Huntress::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;

	CInfo::ESlasher eSlasher = *reinterpret_cast<CInfo::ESlasher*>(pArg);

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelAnim"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));

	if (eSlasher == CInfo::ESlasher::HUNTRESS) {
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Huntress"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Huntress"), TEXT("Model2"), reinterpret_cast<CComponent*&>(m_pModel2));
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_FPV"), TEXT("Model_FPV"), reinterpret_cast<CComponent*&>(m_pModel_FPV));
		m_bSlasher = true;
	}
	else {
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Spirit"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Spirit"), TEXT("Model2"), reinterpret_cast<CComponent*&>(m_pModel2));
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Spirit"), TEXT("Model3"), reinterpret_cast<CComponent*&>(m_pModel3));
		hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Slasher_Spirit_FPV"), TEXT("Model_FPV"), reinterpret_cast<CComponent*&>(m_pModel_FPV));
		m_bSlasher = false;
	}

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));

	//camper
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_GrabLocker"), reinterpret_cast<CComponent*&>(m_pTransform_GrabLocker));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_GrabLocker"), reinterpret_cast<CComponent*&>(m_pCollider_GrabLocker));
	m_pTransform_GrabLocker->Set_Row(CTransform::ERow::POS, XMVectorSet(-0.22f, 0.f, 0.86f, 1.f));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform_Grab"), reinterpret_cast<CComponent*&>(m_pTransform_Grab));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_Grab"), reinterpret_cast<CComponent*&>(m_pCollider_Grab));
	m_pTransform_Grab->Set_Row(CTransform::ERow::POS, XMVectorSet(0.f, 1.f, 0.6f, 1.f));
	m_pCollider_Grab->Set_Extents(_float3(0.5f, 0.5f, 0.9f));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Navi"), TEXT("Navi"), reinterpret_cast<CComponent*&>(m_pNavi));

	m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(160.5f, 0.f, 169.8f, 1.f));
	//m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(203.f, 0.f, 176.f, 1.f));
	//m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(222.149f, 0.f, 131.478f, 1.f));
	m_pCollider->Set_Center(_float3(0.f, 0.8f, 0.f));
	m_pCollider->Set_Extents(_float3(0.45f, 0.8f, 0.45f));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_Foot"), reinterpret_cast<CComponent*&>(m_pCollider_Foot));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_Body"), reinterpret_cast<CComponent*&>(m_pCollider_Body));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_Eyes"), reinterpret_cast<CComponent*&>(m_pCollider_Eyes));
	m_pCollider_Foot->Set_Center(_float3(0.f, 0.2f, 0.f));
	m_pCollider_Foot->Set_Extents(_float3(0.45f, 0.2f, 0.45f));
	m_pCollider_Body->Set_Center(_float3(0.f, 0.85f, 0.f));
	m_pCollider_Body->Set_Extents(_float3(0.45f, 0.75f, 0.45f));
	m_pCollider_Eyes->Set_Center(_float3(0.f, 1.8f, 0.f));
	m_pCollider_Eyes->Set_Extents(_float3(0.5f, 0.5f, 0.5f));

	//m_vecChange_Bone.emplace_back("joint_TorsoC_01");
	//m_vecChange_Bone.emplace_back("joint_TorsoB_01");
	//m_vecChange_Bone.emplace_back("joint_TorsoA_01");
	//m_vecChange_Bone.emplace_back("joint_Pelvis_01");

	if (true == m_bSlasher) {
		m_pModel->Set_Anim(19);
		m_pModel2->Set_Anim(9);
		m_pModel_FPV->Set_Anim(19);

		m_vecChange_Bone.emplace_back("joint_HipMaster_01");
		m_vecChange_Bone.emplace_back("joint_HipRT_01");
		m_vecChange_Bone.emplace_back("joint_KneeRT_01");
		m_vecChange_Bone.emplace_back("joint_FootRT_01");
		m_vecChange_Bone.emplace_back("joint_ToeRT_01");
		m_vecChange_Bone.emplace_back("joint_KneeRollRT_01");
		m_vecChange_Bone.emplace_back("joint_HipLT_01");
		m_vecChange_Bone.emplace_back("joint_KneeLT_01");
		m_vecChange_Bone.emplace_back("joint_FootLT_01");
		m_vecChange_Bone.emplace_back("joint_ToeLT_01");
		m_vecChange_Bone.emplace_back("joint_KneeRollLT_01");
	}
	else {
		m_pModel->Set_Anim(25);
		m_pModel2->Set_Anim(9);
		m_pModel3->Set_Anim(25);
		m_pModel_FPV->Set_Anim(25);

		m_vecChange_Bone.emplace_back("joint_HipMaster_01");
		m_vecChange_Bone.emplace_back("joint_HipRT_01");
		m_vecChange_Bone.emplace_back("joint_LegCutRT_T_01");
		m_vecChange_Bone.emplace_back("joint_LegCutRT_B_01");
		m_vecChange_Bone.emplace_back("joint_KneeRT_01");
		m_vecChange_Bone.emplace_back("joint_Foot_RT_01_IK");
		m_vecChange_Bone.emplace_back("joint_FootRT_01");
		m_vecChange_Bone.emplace_back("joint_ToeRT_01");
		m_vecChange_Bone.emplace_back("joint_HipLT_01");
		m_vecChange_Bone.emplace_back("joint_KneeLT_01");
		m_vecChange_Bone.emplace_back("joint_FootLT_01");
		m_vecChange_Bone.emplace_back("joint_Foot_LT_01_IK");
		m_vecChange_Bone.emplace_back("joint_ToeLT_01");

		m_vecChange_Bone2.emplace_back("joint_WeaponRT_01");
	}
	m_vecChange_Bone.emplace_back("joint_CamperAttach_01");

	/*m_vecChange_Bone.emplace_back("joint_ShoulderRollLT_01");
	m_vecChange_Bone.emplace_back("joint_ShoulderLT_01");
	m_vecChange_Bone.emplace_back("joint_ShoulderRollLT_02");
	m_vecChange_Bone.emplace_back("joint_ElbowLT_01");
	m_vecChange_Bone.emplace_back("joint_HandRoll01LT_02");
	m_vecChange_Bone.emplace_back("joint_HandRoll01LT_01");
	m_vecChange_Bone.emplace_back("joint_Hand_LT_02_IK");

	m_vecChange_Bone.emplace_back("joint_ThumbALT_01");
	m_vecChange_Bone.emplace_back("joint_ThumbBLT_01");
	m_vecChange_Bone.emplace_back("joint_ThumbCLT_01");

	m_vecChange_Bone.emplace_back("joint_IndexALT_01");
	m_vecChange_Bone.emplace_back("joint_IndexBLT_01");
	m_vecChange_Bone.emplace_back("joint_IndexCLT_01");

	m_vecChange_Bone.emplace_back("joint_FingerALT_01");
	m_vecChange_Bone.emplace_back("joint_FingerBLT_01");
	m_vecChange_Bone.emplace_back("joint_FingerCLT_01");

	m_vecChange_Bone.emplace_back("joint_RingALT_01");
	m_vecChange_Bone.emplace_back("joint_RingBLT_01");
	m_vecChange_Bone.emplace_back("joint_RingCLT_01");

	m_vecChange_Bone.emplace_back("joint_PinkyALT_01");
	m_vecChange_Bone.emplace_back("joint_PinkyBLT_01");
	m_vecChange_Bone.emplace_back("joint_PinkyCLT_01");
	*/

	//CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//m_pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_SlasherSideSmoke"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_SlasherSideSmoke")));

	m_pFM = new CFM_Slasher(m_pDevice, m_pContext, m_pNetwork, this);

	//hwang
	CInven* pInven = CInven::Get_Instance();
	if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::ADD_AXE)))
		m_iWeaponMax = 7;

	//ShowCursor(m_bMouse);
	return hr;
}

const _ubyte CSlasher_Huntress::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (true == m_bControl) {
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_9)) {
			m_bMouse = !m_bMouse;
			//ShowCursor(m_bMouse);
		}
		if (true == m_bMouse) {
			RECT rc;
			POINT pt;
			GetClientRect(g_hWnd, &rc);
			pt.x = (rc.left + rc.right) / (_long)2.0;
			pt.y = (rc.bottom + rc.top) / (_long)2.0;
			ClientToScreen(g_hWnd, &pt);
			SetCursorPos(pt.x, pt.y);
		}
		if (nullptr == m_plistCamper)
			m_plistCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
		if (nullptr != m_plistCamper) {
			for (auto& iter : *m_plistCamper) {
				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
				CCollider* pCamperCollider = dynamic_cast<CCollider*>(pCamper->Get_Component(TEXT("Collider")));

				CUI_Play_Icon_Hud* pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"));
				// m_pcollider_grab
				if ((pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::PARKOURVAULT_JUMP || pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::GENERATOR_IDLEF
					|| pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::GENERATOR_IDLEB || pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::GENERATOR_IDLER
					|| pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::GENERATOR_IDLEL) && true == m_pCollider_Grab->Is_Collision(pCamperCollider)) {


					if (m_eCollisionState == ECollisionState::GENERATOR) {
						dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::SONSANG), true);
						dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::SONSANG, false, false);
					}
					m_pColObject = iter;

					if (pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::PARKOURVAULT_JUMP)
						m_eCollisionState = ECollisionState::PARKOUR_GRAB;
					else
						m_eCollisionState = ECollisionState::GENE_GRAB;
				}
				else if (m_eType == EType::SLASH
					&& (m_pFM->Get_State() != CFM_Slasher::EState::THROW_COOLDOWN || m_pFM->Get_State() != CFM_Slasher::EState::PICKUP_IN
						|| m_pFM->Get_State() != CFM_Slasher::EState::PICKUP || m_pFM->Get_State() != CFM_Slasher::EState::DROP
						|| m_pFM->Get_State() != CFM_Slasher::EState::STUN_DROP_IN || m_pFM->Get_State() != CFM_Slasher::EState::STUN_DROP_OUT
						|| m_pFM->Get_State() != CFM_Slasher::EState::STUN_FLASH_IN || m_pFM->Get_State() != CFM_Slasher::EState::STUN_FLASH_OUT)
					&& (pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::CRAWL_IDLE || pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::CRAWL_WALK)
					&& true == m_pCollider->Is_Collision(pCamperCollider)) {
					// Camper Pick Mori UI On 들기 살해
					CInven* pInven = CInven::Get_Instance();
					if (true == m_bSlasher) {
						
						if (0 < m_iWeaponCount && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) == 9999) {
							pHud->Set_AllHudsOn(CUI_Play_Icon_Hud::ESlasher::PASSIVE1, CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::ESlasher::LIFT, CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::ESlasher::KILL, CUI_Play_Icon_Hud::EKey::MOUSE_L);
						}
						else if (0 == m_iWeaponCount && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) == 9999) {
							// 들기, 살해
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::LEFT);
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::KILL), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::RIGHT);
						}
						else if ((0 < m_iWeaponCount && false == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))) ||
							(0 < m_iWeaponCount && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) != 9999) {
							// 도끼, 들기
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::LEFT);
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::RIGHT);
						}
						else if ((0 == m_iWeaponCount && false == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))) ||
							(0 == m_iWeaponCount && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) != 9999) {
							// 들기
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::CENTER);
						}
					}
					else {
						if (true == m_bSpirit && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) == 9999) {
							pHud->Set_AllHudsOn(CUI_Play_Icon_Hud::ESlasher::PASSIVE1, CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::ESlasher::LIFT, CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::ESlasher::KILL, CUI_Play_Icon_Hud::EKey::MOUSE_L);
						}
						else if (false == m_bSpirit && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) == 9999) {
							// 들기, 살해
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::LEFT);
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::KILL), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::RIGHT);
						}
						else if ((true == m_bSpirit && false == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))) ||
							(true == m_bSpirit && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) != 9999) {
							// 영적세계, 들기
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::LEFT);
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::RIGHT);
						}
						else if ((false == m_bSpirit && false == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))) ||
							(false == m_bSpirit && true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)))
							&& pInven->Get_PerkDuration(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER)) != 9999) {
							// 들기
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::CENTER);
						}
					}
					
					// 
					// set ui
					if (m_eCollisionState == ECollisionState::GENERATOR) {
						pHud->Set_HudOff(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::SONSANG), true);
						dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::SONSANG, false, false);
					}
					/*m_pColObject*/

					m_pColObject = iter;
					m_eCollisionState = ECollisionState::CAMPER_CRAWL;
				}
				else if (pCamper->Get_FM_Camper()->Get_State() == CFM_Camper::EState::IDLE || m_eType == EType::CARRY 
					|| (m_pFM->Get_State() == CFM_Slasher::EState::PICKUP_IN || m_pFM->Get_State() == CFM_Slasher::EState::PICKUP 
						|| m_pFM->Get_State() == CFM_Slasher::EState::DROP)                         
					|| false == m_pCollider->Is_Collision(pCamperCollider)) {
					if (Get_ColObject() == pCamper) {
						// Camper Pick Mori UI Off
						pHud->Set_HudOff(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), true);
						pHud->Set_HudOff(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::KILL), true);
						//pHud->Set_AllHuds(false);
						/*if (0 < m_iWeaponCount)
							pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::CENTER);*/

						Set_ColObject(nullptr);
						Set_CollisionState(CSlasher_Huntress::ECollisionState::NONE);
					}
				}
			}
			_float fCamperDist = 9999;
			for (auto& iter : *m_plistCamper) {
				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
				CFM_Camper::EState eCamperState = pCamper->Get_FM_Camper()->Get_State();
				if (eCamperState == CFM_Camper::EState::CLOSSET_IDLE ||
					eCamperState == CFM_Camper::EState::HOOKIN ||
					eCamperState == CFM_Camper::EState::HOOKOUT ||
					eCamperState == CFM_Camper::EState::HOOKIDLE ||
					eCamperState == CFM_Camper::EState::SPIDER_IN ||
					eCamperState == CFM_Camper::EState::SPIDER_LOOP ||
					eCamperState == CFM_Camper::EState::SPIDER_OUT ||
					eCamperState == CFM_Camper::EState::SPIDER_KILLIN ||
					eCamperState == CFM_Camper::EState::SPIDER_KILLLOOP ||
					eCamperState == CFM_Camper::EState::SPIDER_KILLOUT ||
					eCamperState == CFM_Camper::EState::HOOKSTRUGGLE ||
					eCamperState == CFM_Camper::EState::SPIDER_STRUGGLE ||
					eCamperState == CFM_Camper::EState::ENDING_RUN ||
					eCamperState == CFM_Camper::EState::HATCH_IN ||
					eCamperState == CFM_Camper::EState::HATCH_OUT ||
					eCamperState == CFM_Camper::EState::HIT_CRAWL_F ||
					eCamperState == CFM_Camper::EState::HIT_CRAWL_B ||
					eCamperState == CFM_Camper::EState::HIT_CRAWL_L ||
					eCamperState == CFM_Camper::EState::HIT_CRAWL_R ||
					eCamperState == CFM_Camper::EState::CRAWL_IDLE ||
					eCamperState == CFM_Camper::EState::CRAWL_WALK ||
					eCamperState == CFM_Camper::EState::ENDGAME ||
					eCamperState == CFM_Camper::EState::MORI)
					continue;
				CTransform* m_pCamperTransform = dynamic_cast<CTransform*>(iter->Get_Component(TEXT("Transform")));
				_float fCamperDist2 = m_pTransform->Get_Dist(m_pCamperTransform);
				if (fCamperDist > fCamperDist2)
					fCamperDist = fCamperDist2;
			}
			CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::STATIC), TEXT("Setting_BGM")));
			if (TEXT("mu_killer_exit-2") != pBGM->Get_BGM_Name()) {
				switch (m_pFM->Get_State()) {
				case CFM_Slasher::EState::ATTACK_CARRY_IN:
				case CFM_Slasher::EState::ATTACK_CARRY_OUT:
				case CFM_Slasher::EState::ATTACK_CARRY_SWING:
				case CFM_Slasher::EState::IDLE_CARRY:
				case CFM_Slasher::EState::RUN_CARRY_FT:
				case CFM_Slasher::EState::RUN_CARRY_BK:
				case CFM_Slasher::EState::RUN_CARRY_LT:
				case CFM_Slasher::EState::RUN_CARRY_RT:
				case CFM_Slasher::EState::RUN_CARRY_FTLT:
				case CFM_Slasher::EState::RUN_CARRY_FTRT:
				case CFM_Slasher::EState::RUN_CARRY_BKLT:
				case CFM_Slasher::EState::RUN_CARRY_BKRT:
				case CFM_Slasher::EState::PICKUP:
					pBGM->Play_BGM(TEXT("mu_killer_carrying-0"), 0.05f, 2.f);
					break;
				default:
					if (10.f > fCamperDist)
						pBGM->Play_BGM(TEXT("mu_killer_chase-2"), 0.05f, 2.f);
					else
						pBGM->Play_BGM(TEXT("mu_killer_normal_02"), 0.05f, 2.f);
					break;
				}
			}
		}

		m_fNetworkTime += static_cast<_float>(dTimeDelta);
		if (m_fNetworkTime >= 0.2f) {
			m_fNetworkTime = 0.f;
			Update_Network("WorldMatrix");
		}

		_long MouseMove = 0;
		if (MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X)) {
			Update_Network("Set_Dir");
		}

		/*if (true == m_bSlasher && false == m_bLullaby) {
			m_dIntroTime += dTimeDelta;
			if (m_dIntroTime >= 12.) {
				m_bLullaby = true;
				m_pAudio->Play_Sound(TEXT("mu_k08_lullaby"), 1, 0.05f, FMOD_2D | FMOD_LOOP_NORMAL);
				m_pAudio->Set_3DAttribute(1, m_pTransform);
			}
		}*/

		/*switch (m_pFM->Get_State()) {
		case CFM_Slasher::EState::RUN_SLASH_FT:
		case CFM_Slasher::EState::RUN_SLASH_BK:
		case CFM_Slasher::EState::RUN_SLASH_RT:
		case CFM_Slasher::EState::RUN_SLASH_LT:
		case CFM_Slasher::EState::RUN_SLASH_FTRT:
		case CFM_Slasher::EState::RUN_SLASH_FTLT:
		case CFM_Slasher::EState::RUN_SLASH_BKRT:
		case CFM_Slasher::EState::RUN_SLASH_BKLT:
		case CFM_Slasher::EState::RUN_HUNT_FT:
		case CFM_Slasher::EState::RUN_HUNT_BK:
		case CFM_Slasher::EState::RUN_HUNT_RT:
		case CFM_Slasher::EState::RUN_HUNT_LT:
		case CFM_Slasher::EState::RUN_HUNT_FTRT:
		case CFM_Slasher::EState::RUN_HUNT_FTLT:
		case CFM_Slasher::EState::RUN_HUNT_BKRT:
		case CFM_Slasher::EState::RUN_HUNT_BKLT:
		case CFM_Slasher::EState::RUN_CARRY_FT:
		case CFM_Slasher::EState::RUN_CARRY_BK:
		case CFM_Slasher::EState::RUN_CARRY_RT:
		case CFM_Slasher::EState::RUN_CARRY_LT:
		case CFM_Slasher::EState::RUN_CARRY_FTRT:
		case CFM_Slasher::EState::RUN_CARRY_FTLT:
		case CFM_Slasher::EState::RUN_CARRY_BKRT:
		case CFM_Slasher::EState::RUN_CARRY_BKLT:
			m_dFootTime += dTimeDelta;
			if (m_dFootTime >= 0.4) {
				m_dFootTime = 0.;
				if (true == m_bSlasher)
					m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
				else
					m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);
			}
			break;
		}*/

		/*if (m_pFM->Get_State() == CFM_Slasher::EState::RUN_SLASH_FT || m_pFM->Get_State() == CFM_Slasher::EState::RUN_SLASH_FT || m_pFM->Get_State() == CFM_Slasher::EState::RUN_SLASH_FT) {
			m_dFootTime += dTimeDelta;
			if (m_dFootTime >= 0.4) {
				m_dFootTime = 0.;
				m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
			}
		}*/
	}
	else {
		/*if (true == m_bSlasher && false == m_bLullaby) {
			m_dIntroTime += dTimeDelta;
			if (m_dIntroTime >= 12.) {
				m_bLullaby = true;
				m_pAudio->Play_Sound(TEXT("mu_k08_lullaby"), -1, 0.2f, FMOD_3D | FMOD_LOOP_NORMAL, m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			}
		}*/
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
	CProp_EscapeBlocker* pBlocker = dynamic_cast<CProp_EscapeBlocker*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Prop_EscapeBlocker")));
	if (nullptr != pBlocker) {
		CCollider* pBlockerCollider = dynamic_cast<CCollider*>(pBlocker->Get_Component(TEXT("Collider")));
		if (m_pCollider_Body->Is_Collision(pBlockerCollider)) {
			m_bCollider_Move = true;
			XMStoreFloat3(&m_vCollider_Move, XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&pBlockerCollider->Get_Center()), 0.f) - m_pTransform->Get_Row(CTransform::ERow::POS)));
		}
	}

	m_pFM->Update(dTimeDelta);
	m_pCollider->Update(m_pTransform->Get_Matrix());
	m_pCollider_Foot->Update(m_pTransform->Get_Matrix());
	m_pCollider_Body->Update(m_pTransform->Get_Matrix());
	m_pCollider_Eyes->Update(m_pTransform->Get_Matrix());
	m_pCollider_GrabLocker->Update(m_pTransform_GrabLocker->Get_Matrix() * m_pTransform->Get_Matrix());
	m_pCollider_Grab->Update(m_pTransform_Grab->Get_Matrix() * m_pTransform->Get_Matrix());

	/*if (CFM_Slasher::EState::PARKOUR_VAULT != m_pFM->Get_State() && CFM_Slasher::EState::LOCKER_GRAB != m_pFM->Get_State()
		&& CFM_Slasher::EState::LOCKER_RELOAD != m_pFM->Get_State() && CFM_Slasher::EState::PARKOUR != m_pFM->Get_State()) {*/

	if (false == m_bFlag) {
		m_vAccel.y += -9.8f * static_cast<_float>(dTimeDelta);
		_vector vResist = XMVector3Normalize(XMVectorSetY(-XMLoadFloat3(&m_vAccel), 0.f));
		vResist *= 5.f * static_cast<_float>(dTimeDelta);
		XMStoreFloat3(&m_vAccel, XMLoadFloat3(&m_vAccel) + vResist);
		m_pTransform->Set_Row(CTransform::ERow::POS, m_pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&m_vAccel) * static_cast<_float>(dTimeDelta));
		m_pCollider->Update(m_pTransform->Get_Matrix());
		m_pCollider_Foot->Update(m_pTransform->Get_Matrix());
		m_pCollider_Body->Update(m_pTransform->Get_Matrix());
		// 
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

		_float fDist = 0.f;
		for (auto& iter : listCell) {
			switch (iter->Get_Type()) {
			case CCell::EType::FLOOR: {
				if (!m_pCollider_Foot->Is_Collision(iter))
					break;

				_float4 vPlane;
				_float3* pPoint = iter->Get_Point();
				XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2])));

				if (!TriangleTests::Intersects(m_pTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, 100.f, 0.f, 0.f), XMVectorSet(0.f, -1.f, 0.f, 0.f), XMLoadFloat3(&pPoint[0]), XMLoadFloat3(&pPoint[1]), XMLoadFloat3(&pPoint[2]), fDist))
					break;
				//m_fDist = abs(100 - fDist);
				_float3 vPos; XMStoreFloat3(&vPos, m_pTransform->Get_Row(CTransform::ERow::POS));
				vPos.y = -((vPlane.x * vPos.x + vPlane.z * vPos.z + vPlane.w) / vPlane.y);

				//if (m_fDist < 3.f)
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
			}
		}
	}

	if (m_eType == EType::SPIRIT) {
		if (m_fDissolveLength < -0.1f) {
			m_fDissolveLength += 2.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength > -0.1f) {
				m_fDissolveLength = -0.1f;
			}
		}
		else if (m_fDissolveLength > -0.1f) {
			m_fDissolveLength -= 2.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength < -0.1f) {
				m_fDissolveLength = -0.1f;
			}
		}
	}
	else {
		if (m_fDissolveLength < 2.f) {
			m_fDissolveLength += 2.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength > 2.f) {
				m_fDissolveLength = 2.f;
			}
		}
		else if (m_fDissolveLength > 2.f) {
			m_fDissolveLength -= 2.f * static_cast<_float>(dTimeDelta);
			if (m_fDissolveLength < 2.f) {
				m_fDissolveLength = 2.f;
			}
		}
	}

	if (m_pFM->Get_State() == CFM_Slasher::IDLE_SPIRIT && m_bSideEffectFirst) {
		m_pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_SlasherSideSmoke"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_SlasherSideSmoke")));
		m_bSideEffectFirst = false;
	}
	else {
		if (m_pEffect != nullptr)
			m_pEffect->Set_RepeatEnd();
	}

	_float3 vPos2 = {};
	XMStoreFloat3(&vPos2, m_pTransform->Get_Row(CTransform::ERow::POS));

	if (vPos2.y <= 0.f) {
		vPos2.y = 0.f;
		m_pTransform->Set_Row(CTransform::ERow::POS, XMLoadFloat3(&vPos2));
		m_vAccel.y = 0.f;
	}

	Update_Network();
	m_pFM->Update_Network();

	if (true == m_bSlasher) {
		if (true == m_bCancle) {
			m_dCancleTime += dTimeDelta;
			if (m_dCancleTime >= 0.25) {
				m_bCancle = false;
				m_dCancleTime = 0.;
			}
		}
	}
	else {
		if (false == m_bSpirit) {
			m_dSpiritTime += dTimeDelta * 20.0;

			if (m_dSpiritTime >= 100.f) {
				m_bSpirit = true;
				m_dSpiritTime = 0.;

				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
					Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::CENTER);
			}
		}
	}

	//hwang////////////////
	if (m_bControl == true)
		Check_Perk(dTimeDelta);
	//FindSlasher Perk
	if (m_bFindPerkRender == true) {
		m_dFindPerkTime -= dTimeDelta;
		if (m_dFindPerkTime <= 0.0) {
			m_dFindPerkTime = 5.0;
			m_bFindPerkRender = false;
		}
	}
	/////////////////
	return OBJ_NOEVENT;
}

void CSlasher_Huntress::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
	//m_pCollider->Update_Edit(1);
	m_pTransform_Grab->Update_Edit(1);
	m_pCollider_Grab->Update_Edit(2);
	//m_pGrapTransform->Update_Edit(2);
	//m_pGrapCollider->Update_Edit(3);
	ImGui::DragFloat("Roughness", &m_fRoughness, 0.001f);
	ImGui::DragFloat("Refract", &m_fRefract, 0.001f);
	ImGui::DragFloat("Metalness", &m_fMetalness, 0.001f);
	ImGui::DragFloat("Metalness2", &m_fMetalness2, 0.001f);
}

void CSlasher_Huntress::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::SHADOW, this);
	if (m_bFindPerkRender == true)
		m_pRenderer->Add_Object(CRenderer::EOrder::PENETRATE, this);
}

const HRESULT CSlasher_Huntress::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		//m_pCollider->Render(pCamera);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		m_pShader->Set_RawValue("g_fRoughness", &m_fRoughness, sizeof(_float));
		m_pShader->Set_RawValue("g_fRefract", &m_fRefract, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness", &m_fMetalness, sizeof(_float));
		m_pShader->Set_RawValue("g_fMetalness2", &m_fMetalness2, sizeof(_float));

		if (true == m_bControl) {
			if (m_pFM->Get_State() == CFM_Slasher::EState::IDLE_INTRO || m_pFM->Get_State() == CFM_Slasher::EState::MORI) {
				for (_uint i = 0; i < m_pModel_FPV->Get_NumMesh(); ++i) {
					m_pModel_FPV->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
					m_pModel_FPV->Render(i, m_pShader, 0);
				}
			}
			else {
				m_pModel_FPV->Set_Resource(0, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel_FPV->Render(0, m_pShader, 0);

				for (_uint i = 3; i < m_pModel_FPV->Get_NumMesh(); ++i) {
					if (i == 3) {
						if (m_pFM->Get_State() == CFM_Slasher::STUN_DROP_OUT || m_pFM->Get_State() == CFM_Slasher::STUN_DROP_IN || m_pFM->Get_State() == CFM_Slasher::DROP)
							continue;

						m_pModel_FPV->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
						if (FAILED(m_pModel_FPV->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
							m_pModel_FPV->Render(i, m_pShader, 0);
						else
							m_pModel_FPV->Render(i, m_pShader, 7);
					}
					if (i == 6)
						continue;
					if (i == 7 && m_pFM->Get_State() == CFM_Slasher::IDLE_SLASH)
						continue;
					if (false == m_bSlasher) {
						if ((i == 8 || i == 9) && m_pFM->Get_State() != CFM_Slasher::IDLE_SLASH)
							continue;
					}

					m_pModel_FPV->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
					m_pModel_FPV->Render(i, m_pShader, 0);
				}
			}
		}
		else {
			if (m_pFM->Get_State() != CFM_Slasher::IDLE_SPIRIT) {
				CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
				CTexture* pDissolve = dynamic_cast<CTexture*>(pGame_Instance->Get_Component_Prototype(static_cast<_ubyte>(EScene::STATIC), TEXT("Texture_T_BurnedAsh")));
				pDissolve->Set_Resource(m_pShader, "g_DissolveTexture", 0);
				m_pShader->Set_RawValue("g_DissolveLength", &m_fDissolveLength, sizeof(_float));
				_float3 fDissolveColor = { 8.f, 1.0f, 1.f };
				m_pShader->Set_RawValue("g_DissolveColor", &fDissolveColor, sizeof(_float3));

				for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
					m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
					if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
						m_pModel->Render(i, m_pShader, 11);
					else
						m_pModel->Render(i, m_pShader, 11);
				}
			}
		}
		break;
	}
										 break;
	case CRenderer::EOrder::SHADOW: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CShadow* pShadow = dynamic_cast<CShadow*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow")));
		pShadow->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		if (true == m_bControl) {
			for (_uint i = 0; i < m_pModel_FPV->Get_NumMesh(); ++i) {
				m_pModel_FPV->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel_FPV->Render(i, m_pShader, 1);
			}
		}
		else {
			for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
				m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel->Render(i, m_pShader, 1);
			}
		}
	}
								  break;
								  //hwang
	case CRenderer::EOrder::PENETRATE: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		CTarget* pDepth = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Depth"));
		ID3D11ShaderResourceView* pDepthSRV = pDepth->Get_ShaderResourceView();
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
		//m_pGrapCollider->Render(pCamera);
		//m_pCollider_Grab->Render(pCamera);
		//m_pCollider_Eyes->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

void CSlasher_Huntress::Set_Type(const EType& eType) {
	m_eType = eType;
}

void CSlasher_Huntress::Update_Network(const string& pKey) {
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
	if ("Set_Type" == pKey) {
		root["Type"] = static_cast<_int>(m_eType);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_Stun" == pKey) {
		root["Stun"] = static_cast<_bool>(m_bStun);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_Dir" == pKey) {
		_float3 vDir = m_pTransform->Get_Float3(CTransform::ERow::LOOK);
		root["Dir"]["x"] = vDir.x;
		root["Dir"]["y"] = vDir.y;
		root["Dir"]["z"] = vDir.z;
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_Cancle" == pKey) {
		root["Cancle"] = static_cast<_bool>(m_bCancle);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_LockerCancle" == pKey) {
		root["LockerCancle"] = static_cast<_bool>(m_bLockerCancle);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Set_Speed" == pKey) {
		root["Speed"] = static_cast<_float>(m_fPerkSpeed);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
}

void CSlasher_Huntress::Update_Network(void) {
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
		else if ("Set_Type" == key) {
			m_eType = static_cast<CSlasher_Huntress::EType>(root["Type"].asInt());
			iter = recv.erase(iter);
		}
		else if ("Set_Stun" == key) {
			m_bStun = static_cast<_bool>(root["Stun"].asBool());
			iter = recv.erase(iter);
		}
		else if ("Set_Dir" == key) {
			_float3 vDir = {};
			/*vDir.x = (*iter)["Dir"]["x"].asFloat();
			vDir.y = (*iter)["Dir"]["y"].asFloat();
			vDir.z = (*iter)["Dir"]["z"].asFloat();
			m_pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 0.f);
			m_pTransform->LookAt(m_pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&m_vDir));*/
			m_vLook.x = root["Dir"]["x"].asFloat();
			m_vLook.y = root["Dir"]["y"].asFloat();
			m_vLook.z = root["Dir"]["z"].asFloat();
			m_pTransform->LookAt(m_pTransform->Get_Row(CTransform::ERow::POS) + XMLoadFloat3(&m_vLook));
			iter = recv.erase(iter);
		}
		else if ("Set_Cancle" == key) {
			m_bCancle = static_cast<_bool>(root["Cancle"].asBool());
			iter = recv.erase(iter);
		}
		else if ("Set_LockerCancle" == key) {
			m_bLockerCancle = static_cast<_bool>(root["LockerCancle"].asBool());
			iter = recv.erase(iter);
		}
		else if ("Set_Speed" == key) {
			m_fPerkSpeed = static_cast<_float>(root["Speed"].asFloat());
			iter = recv.erase(iter);
		}
		else
			++iter;
	}
}

void CSlasher_Huntress::Check_Perk(_double dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CInven* pInven = CInven::Get_Instance();
	if (pInven->OkayPerk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER)) == true) {
		if (m_bControl == true) {
			pInven->Update(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), dTimeDelta);
			//1.LockGenerator Perk -------------------------- UI --------------- m_bLockGene ->>>> false(파멸 E) -> true
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::LOCK_GENERATOR))) {
				if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_E)) {
					if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::LOCK_GENERATOR))) {
						m_bLockGene = true;
						list<CObject*>* pListObj = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Generator"));
						if (nullptr != pListObj) {
							for (auto& iter : *pListObj) {
								CInter_Generator* pGenerator = dynamic_cast<CInter_Generator*>(iter);
								if (pGenerator->Get_State() != CInter_Generator::EState::ON) {
									pGenerator->Set_Flag(true);
									pGenerator->Set_State(CInter_Generator::EState::LOCK_IN);
								}
							}
						}
						CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
						if (pScore != nullptr) {
							pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::SACRIFICE_ENTITY_SUMMONED));
							pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::SACRIFICE), 200);
						}
					}
				}
				if (m_bLockGene == true) {
					CPerk* pPerk = pInven->Get_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::LOCK_GENERATOR));
					if (pPerk != nullptr) {
						if (pPerk->ReadyStart() == true)
							m_bLockGene = false;
					}
				}
			}
			//2.MORI_CAMPER -> FS_Slasher_Hook
			//3.FIND_CAMPER -> FS_Slasher_Hook
			//4.Find_HealCamper
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_HEALCAMPER))) {
				list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
				if (nullptr != listCamper) {
					for (auto& iter : *listCamper) {
						CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
						if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::HEALCAMPER
							|| pCamper->m_pFM->Get_State() == CFM_Camper::EState::HEALINGSELF
							|| pCamper->m_pFM->Get_State() == CFM_Camper::EState::BEINGHEALED) {
							if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_HEALCAMPER)))
								pCamper->Set_FindPerk(3.0, true);
						}
					}
				}
			}
			//5.Find_ExitCamper
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_EXITCAMPER))) {
				CInter_ExitDoor* pExitDoor = dynamic_cast<CInter_ExitDoor*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_ExitDoor")));
				if (nullptr != pExitDoor) {
					if (pExitDoor->Get_State() == CInter_ExitDoor::ESTATE::OPENING) {
						if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_EXITCAMPER))) {
							list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
							if (nullptr != listCamper) {
								for (auto& iter : *listCamper) {
									CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
									pCamper->Set_FindPerk(5.0, true);
								}
							}
						}
					}
				}
			}
			//6.Kick_Generator -> FS_Slasher_Hook
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::KICK_GENERATOR))) {
				CPerk* pPerk = pInven->Get_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::KICK_GENERATOR));
				if (pPerk != nullptr)
					if (pPerk->Get_DuringTIme() <= 0)
						m_fKickGenerator = 1.0f;
			}
			//7.Find_LockerCamper
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_LOCKERCAMPER))) {
				list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
				if (nullptr != listCamper) {
					for (auto& iter : *listCamper) {
						CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
						if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::CLOSSET_UNHIDE) {
							if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_LOCKERCAMPER)))
								pCamper->Set_FindPerk(3.0, true);
						}
					}
				}
			}
			//8.FIND_CHESTBOXCAMPER
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_CHESTBOXCAMPER))) {
				list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
				if (nullptr != listCamper) {
					for (auto& iter : *listCamper) {
						CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
						if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::CHEST_PICKOUT) {
							if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_CHESTBOXCAMPER)))
								pCamper->Set_FindPerk(3.0, true);
						}
					}
				}
			}
			//9.TOTEM_SPEED
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::TOTEM_SPEED))) {
				CInter_ExitDoor* pExitDoor = dynamic_cast<CInter_ExitDoor*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_ExitDoor")));
				if (pExitDoor->Get_State() == CInter_ExitDoor::ESTATE::OPENING) {
					list<CObject*>* listTotem = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Totem"));
					if (nullptr != listTotem) {
						if (false == m_bTotemSpeed) {
							m_bTotemSpeed = true;
							m_fPerkSpeed = 1.f;
							Update_Network("Set_Speed");
						}
					}
				}
			}
			//10.TOTEM_FIND -> FS_Slasher_Destroy_Totem
			//11.ADD_AXE -> Init()
			//12.RESCUE_HOOK 
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::RESCUE_HOOK))) {
				list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
				if (nullptr != listCamper) {
					for (auto& iter : *listCamper) {
						CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
						if (pCamper->m_pFM->Get_State() == CFM_Camper::EState::HOOKRESCUEIN_BEEING ||
							pCamper->m_pFM->Get_State() == CFM_Camper::EState::HOOKRESCUEIN) {
							pCamper->Set_FindPerk(10.0, true);
						}
					}
				}
			}
		}
	}
}

void CSlasher_Huntress::Set_FindPerk(_double dDuringTime, _bool bFind) {
	m_dFindPerkTime = dDuringTime;
	m_bFindPerkRender = bFind;
}

void CSlasher_Huntress::Set_Accel(_float3 accel) {
	m_vAccel = accel;
}
