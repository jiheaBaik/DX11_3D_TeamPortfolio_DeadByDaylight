#include "pch.h"
#include "FM_Camper.h"
#include "Camera_Camper.h"
#include "Camper_GamePlay.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"
#include "Slasher_Huntress.h"

CFM_Camper::CFM_Camper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pNetwork(pNetwork) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pNetwork);
	m_pCamper_GamePlay = pCamper_GamePlay;
	Init();
}

void CFM_Camper::Delete(void) {

	for (int i = 0; i < EState::END; i++)
		Safe_Release(m_pFS[i]);

	Safe_Release(m_pNetwork);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

}

const HRESULT CFM_Camper::Init(void) {
	m_pFS[EState::IDLE] = new CFS_Camper_Idle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::WALKTO_IDLE] = m_pFS[EState::IDLE];
	m_pFS[EState::RUNTO_IDLE] = m_pFS[EState::IDLE];
	m_pFS[EState::CLOSSET_IDLE] = new CFS_Camper_Closet_Idle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::GENERATOR_IDLEF] = new CFS_Camper_Generator(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::GENERATOR_IDLEB] = m_pFS[EState::GENERATOR_IDLEF];
	m_pFS[EState::GENERATOR_IDLEL] = m_pFS[EState::GENERATOR_IDLEF];
	m_pFS[EState::GENERATOR_IDLER] = m_pFS[EState::GENERATOR_IDLEF];
	m_pFS[EState::GENERATOR_IDLEF_FAILE] = new CFS_Camper_GeneratorFaile(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::GENERATOR_IDLEB_FAILE] = m_pFS[EState::GENERATOR_IDLEF_FAILE];
	m_pFS[EState::GENERATOR_IDLEL_FAILE] = m_pFS[EState::GENERATOR_IDLEF_FAILE];
	m_pFS[EState::GENERATOR_IDLER_FAILE] = m_pFS[EState::GENERATOR_IDLEF_FAILE];
	m_pFS[EState::GENERATOR_GRAB_IN] = new CFS_Camper_Generator_Grab_In(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::GENERATOR_GRAB] = new CFS_Camper_Generator_Grab(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::CLOSSET_HIDE] = new CFS_Camper_Closet_Hide(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CLOSSET_HIDEFAST] = m_pFS[EState::CLOSSET_HIDE];
	m_pFS[EState::CLOSSET_HIDE2] = new CFS_Camper_Closet_Hide2(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CLOSSET_UNHIDE] = new CFS_Camper_Closet_UnHide(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CLOSSET_GRAB] = new CFS_Camper_Closet_Grap(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::PULLDOWN_PLANK_L] = new CFS_Camper_plank(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::PULLDOWN_PLANK_R] = m_pFS[EState::PULLDOWN_PLANK_L];
	m_pFS[EState::_JUMPOVER_FASTL] = m_pFS[EState::PULLDOWN_PLANK_L];
	m_pFS[EState::_JUMPOVER_FASTR] = m_pFS[EState::PULLDOWN_PLANK_L];

	m_pFS[EState::CHEST_IN] = new CFS_Camper_Chest_In(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CHEST_OUT] = new CFS_Camper_Chest_Out(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CHEST_OPEN] = new CFS_Camper_Chest_Open(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CHEST_LOOP] = new CFS_Camper_Chest_Loop(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CHEST_PICKIN] = new CFS_Camper_Chest_PickIn(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CHEST_PICKOUT] = new CFS_Camper_Chest_PickOut(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::EXITDOOR_IN] = new CFS_Camper_ExitDoor_In(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::EXITDOOR_LOOP] = new CFS_Camper_ExitDoor_Loop(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::HATCH_IN] = new CFS_Camper_Hatch(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HATCH_OUT] = new CFS_Camper_HatchOut(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HATCH_CRAWLOUT] = new CFS_Camper_HatchOut(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::WALK_F] = new CFS_Camper_Walk(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SIT_WALK_F] = m_pFS[EState::WALK_F];
	m_pFS[EState::SIT_WALK_B] = m_pFS[EState::WALK_F];
	m_pFS[EState::RUN] = new CFS_Camper_Run(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SIT] = new CFS_Camper_Sit(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SITWALKTO_SIT] = m_pFS[EState::SIT];
	m_pFS[EState::HERE] = new CFS_Camper_Commu(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::POINTTO] = new CFS_Camper_Commu(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HIT_F] = new CFS_Camper_Hit(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HIT_B] = m_pFS[EState::HIT_F];

	m_pFS[EState::HEALINGSELF] = new CFS_Camper_HealingSelf(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HIT_IDLE] = new CFS_Camper_HitIdle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HIT_WALK] = m_pFS[EState::WALK_F];
	m_pFS[EState::HIT_RUN] = m_pFS[EState::RUN];
	m_pFS[EState::HITWALKTO_IDLE] = m_pFS[EState::HIT_IDLE];
	m_pFS[EState::HITRUNTO_IDLE] = m_pFS[EState::HIT_IDLE];
	m_pFS[EState::SITTO_HIT_IDLE] = m_pFS[EState::HIT_IDLE];

	m_pFS[EState::HIT_CRAWL_F] = new CFS_Camper_Hit_Craw(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HIT_CRAWL_B] = m_pFS[EState::HIT_CRAWL_F];
	m_pFS[EState::HIT_CRAWL_L] = m_pFS[EState::HIT_CRAWL_F];
	m_pFS[EState::HIT_CRAWL_R] = m_pFS[EState::HIT_CRAWL_F];

	m_pFS[EState::CRAWL_IDLE] = new CFS_Camper_CrawIdle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CRAWL_WALK] = new CFS_Camper_CrawWalk(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::HIT_SIT] = m_pFS[EState::SIT];
	m_pFS[EState::HIT_SITWALK] = m_pFS[EState::WALK_F];

	m_pFS[EState::CLOSSET_ENTERING] = new CFS_Camper_Closet_Entering(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::CRAWL_PICK1] = new CFS_Camper_Pick1(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CRAWL_PICK2] = new CFS_Camper_Pick2(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CARRY_IDLE] = new CFS_Camper_Carry_Idle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CARRY_WIGGLE] = new CFS_Camper_Carry_Wiggle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::DROP] = new CFS_Camper_Drop(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::STUNDROP] = new CFS_Camper_StunDrop(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::HOOKIN] = new CFS_Camper_HookIn(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOKOUT] = new CFS_Camper_HookOut(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOKIDLE] = new CFS_Camper_HookIdle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOKIDLE2] = new CFS_Camper_HookIdle2(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::HOOKSTRUGGLE] = new CFS_Camper_HookStruggle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOKFREE] = new CFS_Camper_HookFree(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::SPIDER_IN] = new CFS_Camper_Spider_IN(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SPIDER_LOOP] = new CFS_Camper_Spider_Loop(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SPIDER_OUT] = new CFS_Camper_Spider_Out(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOK_DEATH] = new CFS_Camper_Hook_Death(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SPIDER_STRUGGLE] = new CFS_Camper_Spider_Struggle(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::PARKOURVAULT_JUMP] = new CFS_Camper_ParkourVault_Jump(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::PARKOUR_FASTJUMP] = m_pFS[EState::PARKOURVAULT_JUMP];
	m_pFS[EState::PARKOUR_GRAB_F_IN] = new CFS_Camper_Parkour_Grab_In(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::PARKOUR_GRAB_F] = new CFS_Camper_Parkour_Grab(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::PARKOUR_GRAB_B] = new CFS_Camper_Parkour_Grab_B(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::PARKOUR_GRAB_IN_B] = new CFS_Camper_Parkour_Grab_B_In(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::FALL] = new CFS_Camper_Fall(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SITTO_IDLE] = m_pFS[EState::IDLE];
	m_pFS[EState::HEALCAMPER] = new CFS_Camper_HealCamper(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HEALCAMPERCRAWL] = new CFS_Camper_HealCamper_Craw(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	
	m_pFS[EState::BEINGHEALED] = new CFS_Camper_BeingHealed(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::BEINGHEALED_CRAW] = new CFS_Camper_BeingHealed_Craw(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::SPIDER_KILLIN] = new CFS_Camper_Spider_KillIn(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SPIDER_KILLLOOP] = new CFS_Camper_Spider_KillLoop(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::SPIDER_KILLOUT] = new CFS_Camper_Spider_KillOut(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::HOOKRESCUEIN] = new CFS_Camper_HookRescueIn(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOKRESCUEEND] = new CFS_Camper_HookRescueEnd(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOKRESCUEIN_BEEING] = new CFS_Camper_HookBeeingRescuedIn(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::HOOKRESCUEEND_BEEING] = new CFS_Camper_HookBeeingRescuedEnd(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::ENDING_RUN] = new CFS_Camper_Ending(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::MORI] = new CFS_Camper_Mori(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::FLASHLIGHT] = new CFS_Camper_FlashLight(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::TOTEM_IN] = new CFS_Camper_TotemIn(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::TOTEM_LOOP] = new CFS_Camper_TotemLoop(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::TOTEM_OUT] = new CFS_Camper_TotemOut(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	m_pFS[EState::ENDGAME] = new CFS_Camper_EndGame(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);
	m_pFS[EState::CLOSSET_FULLSWING] = new CFS_Camper_Closet_FullSwing(m_pDevice, m_pContext, m_pNetwork, m_pCamper_GamePlay);

	return S_OK;
}

void CFM_Camper::Update(const _double& dTimeDelta) {
	if (true == m_pCamper_GamePlay->m_bControl) {
		EState eState = static_cast<EState>(m_pFS[m_eState]->Update(dTimeDelta));

		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CObject* pObj = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Camera_Camper"));//Intro
		if (nullptr != pObj) {
			if (!static_cast<CCamera_Camper*>(pObj)->Get_IntroEnd()) {
				m_pCamper_GamePlay->m_vAccel.y = 0.f;
				_float3 fPos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);
				m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(fPos.x, 0.f, fPos.z, 1.f));
				eState = CFM_Camper::EState::END;
			}
		}

		if (eState != EState::END) {
			Set_State(eState);
		}

		// set hud on
		if (m_pCamper_GamePlay->m_bItem[static_cast<_ubyte>(CCamper_GamePlay::EItemKind::MEDIKIT)] && m_pCamper_GamePlay->m_iHP == 1 && m_eState != EState::HEALINGSELF) {
			if (m_pCamper_GamePlay->Get_CollisionState() == CCamper_GamePlay::ECollisionState::NONE) {
				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
					Set_HudOn(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::HEALSEL), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
			}
			else {
				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
					Set_HudOff(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::HEALSEL), true);
				dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::HEALSEL, false, false);
			}
		}

	}
	else {
		m_pFS[m_eState]->Update(dTimeDelta);
	}
}

void CFM_Camper::Set_State(const EState& eState) {
	if (m_eState == eState)
		return;

	if (EState::HEALINGSELF == m_ePreState && EState::HIT_IDLE == eState && m_pCamper_GamePlay->m_bIsMedikitPerk == false)
		m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] = 1;

	CInven* pInven = CInven::Get_Instance();
	if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::SELF_HEAL)))
		m_pCamper_GamePlay->Set_RenderItem(false);
	//m_pCamper_GamePlay->Set_RenderItem(true);//TEST
	switch (eState) {
	case EState::IDLE: {
		m_pCamper_GamePlay->Set_Hp(2);
		m_eState = eState;

		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_StandIdle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_StandIdle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandIdle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandIdle");
			break;
		}

		m_ePreState = m_eState;
		break;
	}
	case EState::CLOSSET_IDLE: {
		m_pCamper_GamePlay->Set_Render(false);
		m_pCamper_GamePlay->Set_RenderItem(false);

		m_eState = eState;

		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ClosetIdle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ClosetIdle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ClosetIdle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ClosetIdle");
			break;
		}

		m_ePreState = m_eState;
		break;
	}
	case EState::CLOSSET_HIDE: {
		m_pCamper_GamePlay->Set_RenderItem(false);
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ClosetHide");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ClosetHide");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ClosetHide");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ClosetHide");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CLOSSET_HIDEFAST: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ClosetHideFast");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ClosetHideFast");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ClosetHideFast");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ClosetHideFast");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CLOSSET_UNHIDE: {
		m_pCamper_GamePlay->Set_Render(true);
		if (m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] == 1 ||
			m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] == 1)
			m_pCamper_GamePlay->Set_RenderItem(true);

		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ClosetUnHide");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ClosetUnHide");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ClosetUnhide");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ClosetUnhide");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::WALK_F: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_StandWalk");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_StandWalk");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandWalk");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandWalk");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::SIT_WALK_F: {
		m_eState = eState;

		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrouchWalk");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrouchWalk");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_CrouchWalk");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_CrouchWalk");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::SIT_WALK_B: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrouchWalk_Back");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrouchWalk_Back");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_CrouchWalk_Back");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_CrouchWalk_Back");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::RUN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandRun");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::SIT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrouchIdle_FootLT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrouchIdle_FootLT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_CrouchIdle_FootLT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_CrouchIdle_FootLT");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::PULLDOWN_PLANK_L: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|M_StandPullDownObjectLT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|M_StandPullDownObjectLT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandPullDownObjectLT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandPullDownObjectLT");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::PULLDOWN_PLANK_R: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|M_StandPullDownObjectRT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|M_StandPullDownObjectRT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandPullDownObjectRT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandPullDownObjectRT");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::_JUMPOVER_FASTL: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_JumpOverAngle60LTFast");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_JumpOverAngle60LTFast");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_JumpOverAngle60LTFast");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_JumpOverAngle60LTFast");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::_JUMPOVER_FASTR: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_JumpOverAngle60RTFast");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_JumpOverAngle60RTFast");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_JumpOverAngle60RTFast");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_JumpOverAngle60RTFast");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CHEST_IN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Chest_In");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Chest_In");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Chest_In");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Chest_In");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CHEST_OUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Chest_Out");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Chest_Out");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Chest_Out");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Chest_Out");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CHEST_OPEN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|CF_LootChest_Open");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|CF_LootChest_Open");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_LootChest_Open");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_LootChest_Open");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CHEST_LOOP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Chest_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Chest_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Chest_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Chest_Loop");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CHEST_PICKIN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Chest_Loot_Perk_In");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Chest_Loot_Perk_In");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Chest_Loot_Perk_In");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Chest_Loot_Perk_In");
			break;
		}

		//m_pCamper_GamePlay->m_bisGetItem = true;
		m_ePreState = m_eState;

		break;
	}
	case EState::CHEST_PICKOUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Chest_Loot_Perk_Out");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Chest_Loot_Perk_Out");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Chest_Loot_Perk_Out");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Chest_Loot_Perk_Out");
			break;
		}

		m_ePreState = m_eState;
		//m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] = 1;
		break;
	}
	case EState::EXITDOOR_IN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_UnlockExit");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_UnlockExit");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_UnlockExit");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_UnlockExit");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::EXITDOOR_LOOP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_UnlockExitIdle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_UnlockExitIdle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_UnlockExitIdle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_UnlockExitIdle");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HATCH_IN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_JumpInHatch_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_JumpInHatch_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_JumpInHatch_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_JumpInHatch_IN");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HATCH_OUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_JumpInHatch_OUT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_JumpInHatch_OUT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_JumpInHatch_OUT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_JumpInHatch_OUT");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::GENERATOR_IDLEF: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Generator_Idle_FT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Generator_Idle_FT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Generator_Idle_FT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Generator_Idle_FT_RTFoot");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::GENERATOR_IDLEB: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Generator_Idle_BK_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Generator_Idle_BK_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Generator_Idle_BK_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Generator_Idle_BK_RTFoot");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::GENERATOR_IDLEL: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Generator_Idle_LT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Generator_Idle_LT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Generator_Idle_LT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Generator_Idle_LT_RTFoot");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::GENERATOR_IDLER: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Generator_Idle_RT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Generator_Idle_RT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Generator_Idle_RT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Generator_Idle_RT_RTFoot");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::GENERATOR_IDLEF_FAILE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_GeneratorFail_FT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_GeneratorFail_FT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_GeneratorFail_FT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_GeneratorFail_FT_RTFoot");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::GENERATOR_IDLEB_FAILE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_GeneratorFail_BK");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_GeneratorFail_BK");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_GeneratorFail_BK_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_GeneratorFail_BK_RTFoot");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::GENERATOR_IDLEL_FAILE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_GeneratorFail_LT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_GeneratorFail_LT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_GeneratorFail_LT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_GeneratorFail_LT_RTFoot");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::GENERATOR_IDLER_FAILE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_GeneratorFail_RT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_GeneratorFail_RT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_GeneratorFail_RT_RTFoot");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_GeneratorFail_RT_RTFoot");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::HERE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ComeHere");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ComeHere");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ComeHere");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ComeHere");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::POINTTO: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_PointTo");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_PointTo");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_PointTo");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_PointTo");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_F: {
		m_pCamper_GamePlay->Set_Hp(1);
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Hit_Harpooned_FT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Hit_Harpooned_FT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Hit_Harpooned_FT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Hit_Harpooned_FT");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_B: {
		m_pCamper_GamePlay->Set_Hp(1);
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_K24_Power_Hit_BK");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_K24_Power_Hit_BK");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|F_K24_Power_Hit_BK");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|F_K24_Power_Hit_BK");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::HEALINGSELF: {
		m_pCamper_GamePlay->m_bisGetItem = false;
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HealingSelf");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HealingSelf");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HealingSelf");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HealingSelf");
			break;
		}

		m_ePreState = m_eState;
		m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] = 0;
		break;

	}
	case EState::HIT_IDLE: {
		m_pCamper_GamePlay->Set_Hp(1);
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_inj_StandIdle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_inj_StandIdle");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_WALK: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Inj_StandWalk");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Inj_StandWalk");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_inj_StandWalk");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_inj_StandWalk");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_RUN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Inj_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Inj_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_inj_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_inj_StandRun");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_CRAWL_F: {
		m_pCamper_GamePlay->Set_Hp(0);
		m_pCamper_GamePlay->m_iHP;
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HitFromFT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HitFromFT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Hit_From_FT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Hit_From_FT_To_Crawl");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_CRAWL_B: {
		m_pCamper_GamePlay->Set_Hp(0);
		m_pCamper_GamePlay->m_iHP;

		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HitFromBK_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HitFromBK_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Hit_From_BK_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Hit_From_BK_To_Crawl");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_CRAWL_L: {
		m_pCamper_GamePlay->Set_Hp(0);
		m_pCamper_GamePlay->m_iHP;

		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HitFromLT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HitFromLT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Hit_From_FT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Hit_From_FT_To_Crawl");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_CRAWL_R: {
		m_pCamper_GamePlay->Set_Hp(0);
		m_pCamper_GamePlay->m_iHP;

		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HitFromRT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HitFromRT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Hit_From_RT_To_Crawl");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Hit_From_RT_To_Crawl");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::CRAWL_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrawlRecover");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrawlRecover");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Crawl_Recover");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Crawl_Recover");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CRAWL_WALK: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Crawl_FT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Crawl_FT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Crawl_FT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Crawl_FT");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::WALKTO_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_StandWalk_To_StandIdle_startFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_StandWalk_To_StandIdle_startFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandWalk_To_StandIdle_startFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandWalk_To_StandIdle_startFootLT", 0.7);
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::RUNTO_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_StandRun_To_StandIdle_startFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_StandRun_To_StandIdle_startFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandRun_To_StandIdle_startFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandRun_To_StandIdle_startFootLT", 0.7);
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HITWALKTO_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Inj_StandWalk_To_StandIdle_startFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Inj_StandWalk_To_StandIdle_startFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_inj_StandWalk_To_StandIdle_startFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_inj_StandWalk_To_StandIdle_startFootLT");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HITRUNTO_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Inj_StandRun_To_StandIdle_startFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Inj_StandRun_To_StandIdle_startFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_inj_StandRun_To_StandIdle_startFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_inj_StandRun_To_StandIdle_startFootLT");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::SITWALKTO_SIT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT", 0.7);
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT", 0.7);
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_SIT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Inj_CrouchIdle_Quick");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Inj_CrouchIdle_Quick");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Inj_CrouchIdle_Quick");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Inj_CrouchIdle_Quick");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HIT_SITWALK: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_inj_CrouchWalk");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_inj_CrouchWalk");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_inj_CrouchWalk");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_inj_CrouchWalk");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::HATCH_CRAWLOUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Crawl_Into_Hatch");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Crawl_Into_Hatch");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Crawl_Into_Hatch");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Crawl_Into_Hatch");
			break;
		}

		m_ePreState = m_eState;

		break;
	}
	case EState::CLOSSET_GRAB: {
		if (m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] == 1 ||
			m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] == 1)
			m_pCamper_GamePlay->Set_RenderItem(true);
		m_pCamper_GamePlay->Set_Render(true);
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Grab_Locker");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Grab_Locker");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Grab_Locker");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Grab_Locker");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Grab_Locker");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Grab_Locker");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Grab_Locker");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Grab_Locker");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CLOSSET_ENTERING: {
		if (m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] == 1 ||
			m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] == 1)
			m_pCamper_GamePlay->Set_RenderItem(true);
		m_pCamper_GamePlay->Set_Render(true);
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ClosetFull_Entering");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ClosetFull_Entering");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ClosetFull_Entering");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ClosetFull_Entering");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CRAWL_PICK1: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_PickUp_IN");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_PickUp_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_PickUp_IN");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_PickUp_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_PickUP_IN");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_PickUP_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_PickUP_IN");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_PickUP_IN");
			break;
		}
		m_ePreState = m_eState;

		break;
	}
	case EState::CRAWL_PICK2: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_PickUp");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_PickUp");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_PickUp");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_PickUp");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_PickUP");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_PickUP");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_PickUP");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_PickUP");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::CARRY_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Carry_Idle");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Carry_Idle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Carry_Idle");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Carry_Idle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Carry_Idle");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Carry_Idle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Carry_Idle");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Carry_Idle");
			break;
		}
		m_ePreState = m_eState;
		break;
	}

	case EState::CARRY_WIGGLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Wiggle");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Wiggle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Wiggle");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Wiggle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Wiggle");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Wiggle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Wiggle");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Wiggle");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::DROP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Drop");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Drop");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Drop");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Drop");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Drop");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Drop");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Drop");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Drop");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::STUNDROP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Stun_Drop");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|M_WI_Stun_Drop");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Stun_Drop");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|M_WI_Stun_Drop");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Stun_Drop");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Stun_Drop");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Stun_Drop");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Stun_Drop");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKIN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Hook_IN");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Hook_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Hook_IN");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Hook_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Hook_IN");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Hook_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Hook_IN");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Hook_IN");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKOUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Hook_OUT");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Hook_OUT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Hook_OUT");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Hook_OUT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Hook_OUT");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Hook_OUT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Hook_OUT");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Hook_OUT");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKIDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookedIdle01");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookedIdle01");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookedIdle01");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookedIdle01");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKIDLE2: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookedIdle01");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookedIdle01");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookedIdle01");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookedIdle01");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKSTRUGGLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookStruggle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookStruggle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookStruggle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookStruggle");
			break;
		}

		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKFREE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookedFree");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookedFree");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookedFree");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookedFree");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::SPIDER_IN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Spider_Reaction_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Spider_Reaction_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Spider_Reaction_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Spider_Reaction_IN");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::SPIDER_LOOP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Spider_Reaction_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Spider_Reaction_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Spider_Reaction_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Spider_Reaction_Loop");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::SPIDER_OUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Spider_Reaction_Out");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Spider_Reaction_Out");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Spider_Reaction_Out");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Spider_Reaction_Out");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::SPIDER_KILLIN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BeingKilledBySpiderIN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BeingKilledBySpiderIN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_BeingKilledBySpiderIN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_BeingKilledBySpiderIN");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::SPIDER_KILLLOOP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BeingKilledBySpiderLoop");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BeingKilledBySpiderLoop");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_BeingKilledBySpiderLoop");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_BeingKilledBySpiderLoop");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::SPIDER_KILLOUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BeingKilledBySpiderOut");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BeingKilledBySpiderOut");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_BeingKilledBySpiderOut");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_BeingKilledBySpiderOut");
			break;
		}

		m_ePreState = m_eState;
		break;
	}
	case EState::HOOK_DEATH: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookedDeath");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookedDeath");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookedDeath");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookedDeath");
			break;
		}

		m_ePreState = m_eState;
		break;
	}
	case EState::SPIDER_STRUGGLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Spider_Struggle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Spider_Struggle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Spider_Struggle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Spider_Struggle");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::PARKOUR_FASTJUMP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WindowVaultFast");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WindowVaultFast");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WindowVaultFast");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WindowVaultFast");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::PARKOURVAULT_JUMP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WindowVaultMid");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WindowVaultMid");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WindowVaultMid");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WindowVaultMid");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::FALL: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_Fall");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_Fall");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Fall");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Fall");
			break;
		}

		m_ePreState = m_eState;
		break;
	}
	case EState::SITTO_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HEALCAMPER: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HealCamper");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HealCamper");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HealCamper");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HealCamper");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HEALCAMPERCRAWL: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HealCamperCrawl");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HealCamperCrawl");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HealCamperCrawl");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HealCamperCrawl");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::BEINGHEALED: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BeingHeal");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BeingHeal");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|F_BeingHeal");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|F_BeingHeal");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::BEINGHEALED_CRAW: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrawlRecover");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrawlRecover");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_Crawl_Recover");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_Crawl_Recover");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKRESCUEIN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookRescueCamperIn");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookRescueCamperIn");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookRescueCamperIn");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookRescueCamperIn");
			break;
		}

		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKRESCUEEND: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookRescueCamperEnd");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookRescueCamperEnd");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookRescueCamperEnd");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookRescueCamperEnd");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKRESCUEIN_BEEING: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookBeeingRescuedIn");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookBeeingRescuedIn");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookBeeingRescuedIn");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookBeeingRescuedIn");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::HOOKRESCUEEND_BEEING: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HookBeeingRescuedEnd");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HookBeeingRescuedEnd");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HookBeeingRescuedEnd");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HookBeeingRescuedEnd");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::ENDING_RUN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_StandRun");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_StandRun");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::MORI: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BE_Mori01");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_HK_Mori01");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BE_Mori01");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_HK_Mori01");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_BE_Mori01");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_HK_Mori01");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_BE_Mori01");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_HK_Mori01");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::FLASHLIGHT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_FlashLightStand_Idle");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_FlashLightStand_Idle");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_FlashlightStand_Idle");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_FlashlightStand_Idle");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::TOTEM_IN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BlessTotem_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BlessTotem_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|F_BlessTotem_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|F_BlessTotem_IN");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::TOTEM_LOOP: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BlessTotem_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BlessTotem_Loop");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_BlessTotem_LOOP");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_BlessTotem_LOOP");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::TOTEM_OUT: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_BlessTotem_Out");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_BlessTotem_Out");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_BlessTotem_Out");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_BlessTotem_Out");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::ENDGAME: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_EndGame_FT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_EndGame_FT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_EndGame_FT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_EndGame_FT");
			break;
		}
		m_pFS[eState]->Init("S22_DSkeleton_REF.ao|");
		m_ePreState = m_eState;
		break;
	}
	case EState::CLOSSET_FULLSWING: {
		m_eState = eState;
		if (m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] == 1 ||
			m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] == 1)
			m_pCamper_GamePlay->Set_RenderItem(true);
		m_pCamper_GamePlay->Set_Render(true);
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ClosetFullSwing");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ClosetFullSwing");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ClosetFullSwing");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ClosetFullSwing");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::CLOSSET_HIDE2: {
		
		if(m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::FLASHLIGHT] ==1 ||
			m_pCamper_GamePlay->m_bItem[(_ubyte)CCamper_GamePlay::EItemKind::MEDIKIT] == 1)
			m_pCamper_GamePlay->Set_RenderItem(true);

		m_pCamper_GamePlay->Set_Render(true);
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_ClosetPeekingReaction_SW00");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_ClosetPeekingReaction_SW00");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_ClosetPeekingReaction_SW00");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_ClosetPeekingReaction_SW00");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::PARKOUR_GRAB_IN_B: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::PARKOUR_GRAB_B: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Grab_Obstacles_BK");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Grab_Obstacles_BK");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Grab_Obstacles_BK");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Grab_Obstacles_BK");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Grab_Obstacles_BK");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Grab_Obstacles_BK");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Grab_Obstacles_BK");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Grab_Obstacles_BK");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::PARKOUR_GRAB_F_IN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT_IN");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::PARKOUR_GRAB_F: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Grab_Obstacles_FT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Grab_Obstacles_FT");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Grab_Obstacles_FT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Grab_Obstacles_FT");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Grab_Obstacles_FT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Grab_Obstacles_FT");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Grab_Obstacles_FT");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::GENERATOR_GRAB_IN: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Grab_Generic_Fast_IN");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Grab_Generic_Fast_IN");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Grab_Generic_Fast_IN");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Grab_Generic_Fast_IN");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::GENERATOR_GRAB: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_TT_Grab_Generic_Fast");
			else
				m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_WI_Grab_Generic_Fast");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_TT_Grab_Generic_Fast");
			else
				m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_WI_Grab_Generic_Fast");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_TT_Grab_Generic_Fast");
			else
				m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_WI_Grab_Generic_Fast");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			if (dynamic_cast<CSlasher_Huntress*>(m_pCamper_GamePlay->m_pSlasher)->Get_Slasher())
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_TT_Grab_Generic_Fast");
			else
				m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_WI_Grab_Generic_Fast");
			break;
		}
		m_ePreState = m_eState;
		break;
	}
	case EState::SITTO_HIT_IDLE: {
		m_eState = eState;
		switch (m_pCamper_GamePlay->m_eCampKind)
		{
		case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pFS[eState]->Init("S22_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::MEG:
			m_pFS[eState]->Init("MT_DSkeleton_REF.ao|F_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::BILL:
			m_pFS[eState]->Init("BO_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		case CCamper_GamePlay::ECamperKind::DWIGHT:
			m_pFS[eState]->Init("DF_DSkeleton_REF.ao|M_CrouchWalk_To_CrouchIdle_endFootLT");
			break;
		}
		m_ePreState = m_eState;
		break;
	}


	}
	if (m_pCamper_GamePlay->m_bControl == true)
		Update_Network("Set_State");
}

const CFM_Camper::EState& CFM_Camper::Get_State(void) {
	return m_eState;
}
const CFM_Camper::EState& CFM_Camper::Get_PreState(void) {
	return m_ePreState;
}
void CFM_Camper::Update_Network(const string& pKey) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	Json::Value root;
	if ("Set_State" == pKey) {
		root["State"] = static_cast<_int>(m_eState);
		m_pNetwork->Set_Send(m_pCamper_GamePlay->m_iId, pKey, root);
	}
	if ("Set_Dir" == pKey) {
		root["Dir"][0] = static_cast<_float>(m_pCamper_GamePlay->m_vDotDir.x);
		root["Dir"][1] = static_cast<_float>(m_pCamper_GamePlay->m_vDotDir.y);
		root["Dir"][2] = static_cast<_float>(m_pCamper_GamePlay->m_vDotDir.z);
		m_pNetwork->Set_Send(m_pCamper_GamePlay->m_iId, pKey, root);
	}
	if ("Set_HealGauge" == pKey) {
		root["HealGauge"] = static_cast<_float>(m_pCamper_GamePlay->m_HealGauge);
		m_pNetwork->Set_Send(m_pCamper_GamePlay->m_iId, pKey, root);
	}
}

void CFM_Camper::Update_Network(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	list<Json::Value>& recv = m_pNetwork->Get_Recv(m_pCamper_GamePlay->m_iId);
	for (auto iter = recv.begin(); iter != recv.end();) {
		Json::Value root = (*iter);
		string key = root["Key"].asString();
		if ("Set_State" == key) {
			Set_State(static_cast<EState>(root["State"].asInt()));
			iter = recv.erase(iter);
		}
		else if ("Set_Dir" == key) {
			m_pCamper_GamePlay->m_vDotDir.x = (static_cast<_float>(root["Dir"][0].asFloat()));
			m_pCamper_GamePlay->m_vDotDir.y = (static_cast<_float>(root["Dir"][1].asFloat()));
			m_pCamper_GamePlay->m_vDotDir.z = (static_cast<_float>(root["Dir"][2].asFloat()));
			iter = recv.erase(iter);
		}
		else if ("Set_HealGauge" == key) {
			m_pCamper_GamePlay->m_HealGauge = (static_cast<_float>(root["HealGauge"].asFloat()));
			iter = recv.erase(iter);
		}
		else
			++iter;
	}
}
