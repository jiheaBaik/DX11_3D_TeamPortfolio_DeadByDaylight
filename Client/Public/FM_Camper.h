#pragma once
#include "FS_Camper_Idle.h"
#include "FS_Camper_Walk.h"
#include "FS_Camper_Sit.h"
#include "FS_Camper_Run.h"
#include "FS_Camper_Commu.h"
#include "FS_Camper_Closet_Hide.h"
#include "FS_Camper_Closet_UnHide.h"
#include "FS_Camper_plank.h"
#include "FS_Camper_Chest_Out.h"
#include "FS_Camper_Chest_Open.h"
#include "FS_Camper_Chest_Loop.h"
#include "FS_Camper_Chest_In.h"
#include "FS_Camper_ExitDoor_In.h"
#include "FS_Camper_ExitDoor_Loop.h"
#include "FS_Camper_Hatch.h"
#include "FS_Camper_Generator.h"
#include "FS_Camper_Closet_Idle.h"
#include "FS_Camper_Chest_PickIn.h"
#include "FS_Camper_Chest_PickOut.h"
#include "FS_Camper_Hit.h"
#include "FS_Camper_HealingSelf.h"
#include "FS_Camper_HitIdle.h"
#include "FS_Camper_Hit_Craw.h"
#include "FS_Camper_CrawIdle.h"
#include "FS_Camper_CrawWalk.h"
#include "FS_Camper_HatchOut.h"
#include "FS_Camper_Closet_Grap.h"
#include "FS_Camper_Closet_Entering.h"
#include "FS_Camper_Pick1.h"
#include "FS_Camper_Pick2.h"
#include "FS_Camper_Carry_Idle.h"
#include "FS_Camper_Carry_Wiggle.h"
#include "FS_Camper_Drop.h"
#include "FS_Camper_HookIn.h"
#include "FS_Camper_HookOut.h"
#include "FS_Camper_HookIdle.h"
#include "FS_Camper_HookStruggle.h"
#include "FS_Camper_HookFree.h"
#include "FS_Camper_StunDrop.h"
#include "FS_Camper_Spider_IN.h"
#include "FS_Camper_Spider_Loop.h"
#include "FS_Camper_Spider_Out.h"
#include "FS_Camper_Hook_Death.h"
#include "FS_Camper_Spider_Struggle.h"
#include "FS_Camper_HookIdle2.h"
#include "FS_Camper_ParkourVault_Jump.h"
#include "FS_Camper_Fall.h"
#include "FS_Camper_HealCamper.h"
#include "FS_Camper_BeingHealed.h"
#include "FS_Camper_Spider_KillIn.h"
#include "FS_Camper_Spider_KillLoop.h"
#include "FS_Camper_Spider_KillOut.h"
#include "FS_Camper_HookRescueIn.h"
#include "FS_Camper_HookRescueEnd.h"
#include "FS_Camper_HookBeeingRescuedIn.h"
#include "FS_Camper_HookBeeingRescuedEnd.h"
#include "FS_Camper_Ending.h"
#include "FS_Camper_Mori.h"
#include "FS_Camper_FlashLight.h"
#include "FS_Camper_TotemIn.h"
#include "FS_Camper_TotemOut.h"
#include "FS_Camper_TotemLoop.h"
#include "FS_Camper_EndGame.h"
#include "FS_Camper_Closet_FullSwing.h"
#include "FS_Camper_GeneratorFaile.h"
#include "FS_Camper_Closet_Hide2.h"
#include "FS_Camper_Parkour_Grab.h"
#include "FS_Camper_Parkour_Grab_In.h"
#include "FS_Camper_Parkour_Grab_B.h"
#include "FS_Camper_Parkour_Grab_B_In.h"
#include "FS_Camper_Generator_Grab_In.h"
#include "FS_Camper_Generator_Grab.h"
#include "FS_Camper_HealCamper_Craw.h"
#include "FS_Camper_BeingHealed_Craw.h"





BEGIN_NAMESPACE(Client)
class CFM_Camper final : public CBase {
public:
	enum EState : _ubyte {
		IDLE, CLOSSET_IDLE, CLOSSET_HIDE, CLOSSET_HIDE2,CLOSSET_HIDEFAST, CLOSSET_UNHIDE, CLOSSET_FULLSWING,  CLOSSET_GRAB, CLOSSET_ENTERING, HIT_F, HIT_B, HIT_IDLE, HEALINGSELF,
		HIT_WALK, HITWALKTO_IDLE, HITRUNTO_IDLE, HIT_RUN, HIT_SIT, HIT_SITWALK,
		HIT_CRAWL_F, HIT_CRAWL_B, HIT_CRAWL_L, HIT_CRAWL_R,  CRAWL_WALK, CRAWL_IDLE, CRAWL_PICK1, CRAWL_PICK2, CARRY_IDLE, CARRY_WIGGLE, STUNDROP, DROP,
		HOOKIN, HOOKOUT, HOOKIDLE, HOOKIDLE2,  HOOKSTRUGGLE, HOOKFREE, HOOKRESCUEIN, HOOKRESCUEEND, HOOKRESCUEIN_BEEING, HOOKRESCUEEND_BEEING,
		WALK_F, SIT, SIT_WALK_F, SIT_WALK_B, SITWALKTO_SIT, WALKTO_IDLE, RUNTO_IDLE, RUN,
		SPIDER_IN, SPIDER_LOOP, SPIDER_OUT, SPIDER_STRUGGLE, SPIDER_KILLIN, SPIDER_KILLLOOP, SPIDER_KILLOUT, /*SPIDER_STRUGGLE2SACRIFICE, SPIDER_STRUGGLE_FAIL,*/ HOOK_DEATH,
		PULLDOWN_PLANK_L, PULLDOWN_PLANK_R, CHEST_IN, CHEST_OUT, CHEST_OPEN,
		CHEST_LOOP, CHEST_PICKIN, CHEST_PICKOUT, EXITDOOR_IN, EXITDOOR_LOOP,
		 _JUMPOVER_FASTL, _JUMPOVER_FASTR, HATCH_IN, HATCH_OUT, HATCH_CRAWLOUT,
		GENERATOR_IDLEF, GENERATOR_IDLEB, GENERATOR_IDLEL, GENERATOR_IDLER, GENERATOR_IDLEF_FAILE, GENERATOR_IDLEB_FAILE, GENERATOR_IDLEL_FAILE, GENERATOR_IDLER_FAILE,
		GENERATOR_GRAB_IN, GENERATOR_GRAB,
		PARKOUR_GRAB_F_IN, PARKOUR_GRAB_F, PARKOUR_GRAB_B, PARKOUR_GRAB_IN_B, PARKOUR_FASTJUMP, PARKOURVAULT_JUMP,
		FALL, SITTO_IDLE, SITTO_HIT_IDLE, HERE, POINTTO, HEALCAMPER, HEALCAMPERCRAWL,  BEINGHEALED, BEINGHEALED_CRAW, ENDING_RUN,
		TOTEM_IN, TOTEM_OUT, TOTEM_LOOP, ENDGAME,
		FLASHLIGHT, MORI, END
	};
public:
	explicit CFM_Camper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CCamper_GamePlay* const& pCamper_GamePlay);
	virtual ~CFM_Camper(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	void Update(const _double& dTimeDelta);
public:
	void Set_State(const EState& eState);
	const EState& Get_State(void);
	const EState& Get_PreState(void);

	void Update_Network(const string& pKey);
	void Update_Network(void);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	class CNetwork* m_pNetwork = nullptr;

	class CCamper_GamePlay* m_pCamper_GamePlay = nullptr;
	class CFS* m_pFS[EState::END];
	EState m_eState = EState::IDLE;
	EState m_ePreState = m_eState;

	friend CFS_Camper_Idle;
	friend CFS_Camper_Walk;
	friend CFS_Camper_Sit;
	friend CFS_Camper_Run;
	friend CFS_Camper_Commu;
	friend CFS_Camper_Closet_Hide;
	friend CFS_Camper_Closet_UnHide;
	friend CFS_Camper_plank;
	friend CFS_Camper_Chest_Out;
	friend CFS_Camper_Chest_Open;
	friend CFS_Camper_Chest_Loop;
	friend CFS_Camper_Chest_In;

	friend CFS_Camper_ExitDoor_In;
	friend CFS_Camper_ExitDoor_Loop;
	friend CFS_Camper_Hatch;
	friend CFS_Camper_Generator;
	friend CFS_Camper_Closet_Idle;
	friend CFS_Camper_Chest_PickIn;
	friend CFS_Camper_Chest_PickOut;
	friend CFS_Camper_Hit;
	friend CFS_Camper_HealingSelf;
	friend CFS_Camper_HitIdle;
	friend CFS_Camper_Hit_Craw;
	friend CFS_Camper_CrawIdle;
	friend CFS_Camper_CrawWalk;
	friend 	CFS_Camper_HatchOut;
	friend 	CFS_Camper_Closet_Grap;
	friend 	CFS_Camper_Closet_Entering;
	friend 	CFS_Camper_Pick1;
	friend 	CFS_Camper_Pick2;
	friend 	CFS_Camper_Carry_Idle;
	friend 	CFS_Camper_Carry_Wiggle;
	friend 	CFS_Camper_Drop;
	friend 	CFS_Camper_HookIn;
	friend 	CFS_Camper_HookOut;
	friend 	CFS_Camper_HookIdle;
	friend 	CFS_Camper_HookStruggle;
	friend 	CFS_Camper_HookFree;
	friend 	CFS_Camper_StunDrop;
	friend 	CFS_Camper_Spider_IN;
	friend 	CFS_Camper_Spider_Loop;
	friend 	CFS_Camper_Spider_Out;
	friend 	CFS_Camper_Hook_Death;
	friend 	CFS_Camper_Spider_Struggle;
	friend 	CFS_Camper_HookIdle2;
	friend CFS_Camper_ParkourVault_Jump;
	friend CFS_Camper_Fall;
	friend CFS_Camper_HealCamper;
	friend CFS_Camper_BeingHealed;
	friend CFS_Camper_Spider_KillIn;
	friend CFS_Camper_Spider_KillLoop;
	friend CFS_Camper_Spider_KillOut;
	friend CFS_Camper_HookRescueIn;
	friend CFS_Camper_HookRescueEnd;
	friend CFS_Camper_HookBeeingRescuedIn;
	friend CFS_Camper_HookBeeingRescuedEnd;
	friend CFS_Camper_Ending;
	friend CFS_Camper_Mori;
	friend CFS_Camper_FlashLight;
	friend CFS_Camper_TotemIn;
	friend CFS_Camper_TotemLoop;
	friend CFS_Camper_TotemOut;
	friend CFS_Camper_EndGame;
	friend CFS_Camper_Closet_FullSwing;
	friend CFS_Camper_GeneratorFaile;
	friend CFS_Camper_Closet_Hide2;
	friend CFS_Camper_Parkour_Grab;
	friend CFS_Camper_Parkour_Grab_In;
	friend CFS_Camper_Parkour_Grab_B;
	friend CFS_Camper_Parkour_Grab_B_In;
	friend CFS_Camper_Generator_Grab_In;
	friend 	CFS_Camper_Generator_Grab;
	friend 	CFS_Camper_HealCamper_Craw;
	friend 	CFS_Camper_BeingHealed_Craw;

	
	
};
END_NAMESPACE;
