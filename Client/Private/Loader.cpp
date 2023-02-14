#include "pch.h"
#include "Loader.h"

//Logo
#include "UI_Logo_Logo.h"
#include "UI_Logo_Title.h"
#include "UI_Logo_Back.h"
#include "UI_Logo_BackSmoke.h"
#include "UI_Logo_BackSmoke2.h"
#include "UI_Logo_Under.h"
#include "UI_Logo_Smoke.h"
#include "UI_Logo_Smoke2.h"
#include "UI_Logo_Smoke3.h"
#include "UI_Logo_Smoke4.h"	
#include "UI_Logo_Smoke5.h"
#include "UI_Logo_Text.h"
#include "Effect_LogoParticle.h"

//Lobby
#include "Camera_Lobby.h"
#include "Prop_Lobby.h"
#include "Prop_Lobby_PoliceCar.h"
#include "Slasher_Huntress_Lobby.h"
#include "Slasher_Spirit_Lobby.h"
#include "Camper_Lobby.h"
#include "Camper_Lobby_Sit.h"
#include "Weapon_Slash_Lobby.h"
#include "Weapon_Hunt_Lobby.h"
#include "Weapon_Spirit_Lobby.h"
#include "Weapon_Spirit_Hand_Lobby.h"
#include "Effect_LobbyCampFireZ.h"
#include "Effect_LobbyCampFireSmoke.h"
#include "Effect_LobbyCampFireSpark.h"
#include "Effect_Camper_Lobby.h"
#include "Light_Police.h"
#include "Setting_CamperSlot.h"
#include "Inter_Crow_Lobby.h"

#include "UI_Lobby_Panel_First.h"
#include "UI_Lobby_Btn_Select_Role.h"
#include "UI_Lobby_Panel_Slasher.h"
#include "UI_Lobby_Btn_Slasher_Menu.h"
#include "UI_Lobby_Btn_Select_To_Start.h"
#include "UI_Lobby_Panel_Slasher_Choice.h"
#include "UI_Lobby_Panel_Camper_Choice.h"
#include "UI_Lobby_Panel_Camper.h"
#include "UI_Lobby_Btn_Camper_Menu.h"
#include "UI_Lobby_Btn_Ready.h"
#include "UI_Lobby_Btn_Setting.h"
#include "UI_Lobby_Panel_Exit.h"
#include "UI_Lobby_Btn_Exit.h"
#include "UI_Lobby_Icon_Ready_Camper.h"
#include "UI_Lobby_Icon_Ready_Back.h"
#include "UI_Lobby_Icon_Ready_Bone.h"
#include "UI_Lobby_Panel_Loadout.h"
#include "UI_Lobby_Panel_Explain.h"

//GamePlay
#include "Camera_Slasher.h"
#include "Camera_Camper.h"
#include "Light_Slasher_Eye.h"
#include "Camper_GamePlay.h"
#include "Slasher_Huntress.h"
#include "Weapon_Slash.h"
#include "Weapon_Hunt.h"
#include "Weapon_Throw.h"
#include "Weapon_Spirit.h"
#include "Weapon_Spirit_Hand.h"
#include "Setting_CamperPos.h"
#include "Setting_BGM.h"

#include "Inter_ChestBox.h"
#include "Inter_Crow.h"
#include "Inter_Crow_Wander.h"
#include "Inter_Generator.h"
#include "Inter_Hatch.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_PlankBroken.h"
#include "Inter_Rat.h"
#include "Inter_Totem.h"
#include "Inter_ExitDoor.h"
#include "Inter_Exit.h"
#include "Inter_Hook.h"
#include "Inter_Parkour.h"
#include "Inter_Parkour_Vault.h"
#include "Inter_EndGame.h"
#include "Prop_GamePlay.h"
#include "Prop_EscapeBlocker.h"
#include "Prop_Corn.h"
#include "Prop_Grass.h"
#include "Prop_Grass2.h"
#include "Prop_Grass3.h"
#include "Prop_Grass4.h"
#include "Prop_Grass5.h"

#include "Effect_Blood_Free.h"
#include "Effect_BloodStain.h"
#include "Effect_CamBlood_Free.h"
#include "Effect_CamperHookDeath.h"
#include "Effect_DrumFire.h"
#include "Effect_DrumFireEtc.h"
#include "Effect_FallingLeaf.h"
#include "Effect_FlashLight.h"
#include "Effect_Fog.h"
#include "Effect_Free3.h"
#include "Effect_Hatch.h"
#include "Effect_HookSmoke.h"
#include "Light_Generator.h"
#include "Light_GeneratorFail.h"
#include "Light_Flash.h"
#include "Cube_Free.h"

#include "UI_Play_Panel_First.h"
#include "UI_Play_Panel_Second.h"
#include "UI_Play_Icon_Propile.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Icon_Passive.h"
#include "UI_Play_Icon_Perk.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Gauge_Exit.h"
#include "UI_Play_Icon_SkillCheck.h"
#include "UI_End_Panel_First.h"
#include "UI_End_Panel_Second.h"
#include "UI_End_Panel_FX.h"
#include "UI_End_Btn_Return.h"
#include "UI_Play_Panel_Manager.h"

#include "Slasher_FPV_Trail.h"
#include "Slasher_Trail.h"
#include "Throw_Trail.h"
#include "Slasher_Spirit_FPV_Trail.h"
#include "Slasher_Spirit_Trail.h"

#include "Ending_Spon.h"
#include "Ending_Ground.h"
#include "Ending_Grass.h"
#include "Ending_PlayerPos.h"
#include "Ending_Camper.h"
#include "Ending_Slasher.h"
#include "Camera_Ending.h"

#include "UI_Play_Score.h"

CLoader* const CLoader::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const EScene& eScene) {
	CLoader* pInstnace = new CLoader(pDevice, pContext);
	if (FAILED(pInstnace->Init(eScene)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CLoader::CLoader(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	ZeroMemory(&m_Critical_Section, sizeof(CRITICAL_SECTION));
}

void CLoader::Delete(void) {
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

unsigned int APIENTRY Loader_Thread(void* const pArg);

const HRESULT CLoader::Init(const EScene& eScene) {
	m_eScene = eScene;
	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, Loader_Thread, this, 0, nullptr));
	if (0 == m_hThread)
		return E_FAIL;
	InitializeCriticalSection(&m_Critical_Section);
	return S_OK;
}

unsigned int APIENTRY Loader_Thread(void* const pArg) {
	CLoader* pLoader = reinterpret_cast<CLoader*>(pArg);
	pLoader->Update();
	return 0;
}

void CLoader::Update(void) {
	EnterCriticalSection(&m_Critical_Section);
	if (FAILED(CoInitializeEx(nullptr, 0))) {
		LeaveCriticalSection(&m_Critical_Section);
		return;
	}
	HRESULT hr = S_OK;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CNetwork* pNetwork = pGame_Instance->Get_Network();

	switch (m_eScene) {
	case EScene::LOGO:
		hr |= Load_Texture(m_pDevice, m_pContext, EScene::LOGO, TEXT("../Bin/Resource/01.Logo/Texture/*.*"));
		hr |= Load_Model(m_pDevice, m_pContext, EScene::LOGO, TEXT("../Bin/Resource/01.Logo/Model/*.*"));
		hr |= pGame_Instance->Load_Sound(static_cast<_ubyte>(EScene::LOGO), TEXT("../Bin/Resource/01.Logo/Sound/*.*"));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LogoParticle"), CEffect_LogoParticle::Create(m_pDevice, m_pContext, pNetwork,TEXT("../Bin/Resource/01.Logo/Effect/LogoParticle_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Logo"), CUI_Logo_Logo::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Title"), CUI_Logo_Title::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Back"), CUI_Logo_Back::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_BackSmoke"), CUI_Logo_BackSmoke::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_BackSmoke2"), CUI_Logo_BackSmoke2::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Under"), CUI_Logo_Under::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Smoke"), CUI_Logo_Smoke::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Smoke2"), CUI_Logo_Smoke2::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Smoke3"), CUI_Logo_Smoke3::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Smoke4"), CUI_Logo_Smoke4::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Smoke5"), CUI_Logo_Smoke5::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Logo_Text"), CUI_Logo_Text::Create(m_pDevice, m_pContext, pNetwork));
		break;
	case EScene::LOBBY:
		hr |= Load_Texture(m_pDevice, m_pContext, EScene::LOBBY, TEXT("../Bin/Resource/02.Lobby/Texture/*.*"));
		hr |= Load_Model(m_pDevice, m_pContext, EScene::LOBBY, TEXT("../Bin/Resource/02.Lobby/Model/*.*"));
		hr |= pGame_Instance->Load_Sound(static_cast<_ubyte>(EScene::LOBBY), TEXT("../Bin/Resource/02.Lobby/Sound/*.*"));

		hr |= pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::LOBBY), TEXT("Navi"), CNavi::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/02.Lobby/Navi.txt")));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Camera_Lobby"), CCamera_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_Huntress_Lobby"), CSlasher_Huntress_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_Spirit_Lobby"), CSlasher_Spirit_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Camper_Lobby"), CCamper_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Camper_Lobby_Sit"), CCamper_Lobby_Sit::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Slash_Lobby"), CWeapon_Slash_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Hunt_Lobby"), CWeapon_Hunt_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Spirit_Lobby"), CWeapon_Spirit_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Spirit_Hand_Lobby"), CWeapon_Spirit_Hand_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Lobby"), CProp_Lobby::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Lobby_PoliceCar"), CProp_Lobby_PoliceCar::Create(m_pDevice, m_pContext, pNetwork));
		
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyCamperSmoke"), CEffect_Camper_Lobby::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyPlayerCamperStart_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyCameFireZ"), CEffect_LobbyCampFireZ::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyCampFire_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyCameFireSmoke"), CEffect_LobbyCampFireSmoke::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyCampFireSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyCameFireSpark"), CEffect_LobbyCampFireSpark::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyCampFireSpark_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbySlasherSmoke"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbySlasherSmoke.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbySlasherParticle"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbySlasherfirstParticle_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyOtherPlayerZone"), CEffect_Camper_Lobby::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyOtherSummonZoneGround_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyOtherPlayerZoneBack"), CEffect_Camper_Lobby::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyOtherSummonZone_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyOtherPlayerZoneParticle0"), CEffect_Camper_Lobby::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyStartCamperParticleBlack_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_LobbyOtherPlayerZoneParticle1"), CEffect_Camper_Lobby::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/02.Lobby/Effect/LobbyStartCamperParticleRed_r.txt")));
		
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Light_Police"), CLight_Police::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Crow_Lobby"), CInter_Crow_Lobby::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Setting_CamperSlot"), CSetting_CamperSlot::Create(m_pDevice, m_pContext, pNetwork));
		
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Panel_First"), CUI_Lobby_Panel_First::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Btn_Select_Role"), CUI_Lobby_Btn_Select_Role::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Panel_Slasher"), CUI_Lobby_Panel_Slasher::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Btn_Slasher_Menu"), CUI_Lobby_Btn_Slasher_Menu::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Btn_Select_To_Start"), CUI_Lobby_Btn_Select_To_Start::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Panel_Slasher_Choice"), CUI_Lobby_Panel_Slasher_Choice::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Panel_Camper"), CUI_Lobby_Panel_Camper::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Btn_Camper_Menu"), CUI_Lobby_Btn_Camper_Menu::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Panel_Camper_Choice"), CUI_Lobby_Panel_Camper_Choice::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Btn_Ready"), CUI_Lobby_Btn_Ready::Create(m_pDevice, m_pContext, pNetwork));	
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Btn_Setting"), CUI_Lobby_Btn_Setting::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Icon_Ready_Camper"), CUI_Lobby_Icon_Ready_Camper::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Icon_Ready_Back"), CUI_Lobby_Icon_Ready_Back::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Icon_Ready_Bone"), CUI_Lobby_Icon_Ready_Bone::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Panel_Loadout"), CUI_Lobby_Panel_Loadout::Create(m_pDevice, m_pContext, pNetwork));	
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Lobby_Panel_Explain"), CUI_Lobby_Panel_Explain::Create(m_pDevice, m_pContext, pNetwork));

		break;
	case EScene::GAMEPLAY:
		hr |= Load_Texture(m_pDevice, m_pContext, EScene::GAMEPLAY, TEXT("../Bin/Resource/03.GamePlay/Texture/*.*"));
		hr |= Load_Model(m_pDevice, m_pContext, EScene::GAMEPLAY, TEXT("../Bin/Resource/03.GamePlay/Model/*.*"));
		hr |= pGame_Instance->Load_Sound(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("../Bin/Resource/03.GamePlay/Sound/*.*"));
		hr |= pGame_Instance->Add_Component_Prototype(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Navi"), CNavi::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/03.GamePlay/Navi.txt")));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Camera_Camper"), CCamera_Camper::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Camera_Slasher"), CCamera_Slasher::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Light_Slasher_Eye"), CLight_Slasher_Eye::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Light_Generator"), CLight_Generator::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Light_GeneratorFail"), CLight_GeneratorFail::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Light_Flash"), CLight_Flash::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_Huntress"), CSlasher_Huntress::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Camper_GamePlay"), CCamper_GamePlay::Create(m_pDevice, m_pContext, pNetwork));
		//hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_FPV"), CSlasher_Huntress::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Slash"), CWeapon_Slash::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Hunt"), CWeapon_Hunt::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Throw"), CWeapon_Throw::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Spirit"), CWeapon_Spirit::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Weapon_Spirit_Hand"), CWeapon_Spirit_Hand::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_ChestBox"), CInter_ChestBox::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Crow"), CInter_Crow::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Crow_Wander"), CInter_Crow_Wander::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Generator"), CInter_Generator::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Hatch"), CInter_Hatch::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Hook"), CInter_Hook::Create(m_pDevice, m_pContext, pNetwork));
		/*hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_HookHuntress"), CInter_HookHuntress::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_HookSpider"), CInter_HookSpider::Create(m_pDevice, m_pContext, pNetwork));*/
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Locker"), CInter_Locker::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_PlankAnim"), CInter_PlankAnim::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Rat"), CInter_Rat::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Totem"), CInter_Totem::Create(m_pDevice, m_pContext, pNetwork));		
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_ExitDoor"), CInter_ExitDoor::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Exit"), CInter_Exit::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Parkour"), CInter_Parkour::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_Parkour_Vault"), CInter_Parkour_Vault::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Inter_EndGame"), CInter_EndGame::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_Blindness"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/Blindness_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_Blood"), CEffect_Blood_Free::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_BloodStain"), CEffect_BloodStain::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/3DBloodStain.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_CamBlood"), CEffect_CamBlood_Free::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_CamperHookDeath"), CEffect_CamperHookDeath::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/CamperHookDeath_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_CamperHookSideSmoke"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/CamperHookSideSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_DrumFire"), CEffect_DrumFire::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/DrumFire_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_DrumFireAshes"), CEffect_DrumFireEtc::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/DumpFireAshes_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_DrumFireSmoke"), CEffect_DrumFireEtc::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/DrumSmoke.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_DrumFireSpark"), CEffect_DrumFireEtc::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/DrumFireSpark_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_EndGameBoom"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/EndGameBoom_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_EndGameParticle"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/EndgameParticle_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_EndGameSmoke"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/EndGameSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_EscapeLeverElectricity"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/LeverElectricity_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_EscapeLeverSpark"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/Leverspark_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_FallingLeaf"), CEffect_FallingLeaf::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/TreeLeaf_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_FallingLeaf_H"), CEffect_FallingLeaf::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/TreeLeaf_rr.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_FlashLightAttack"), CEffect_FlashLight::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/LightFlashAttack.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_FlashLightIdle"), CEffect_FlashLight::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/LightFlashIdle.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_Fog"), CEffect_Fog::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/Fog_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorClearSpark"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorClearSpark_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorCrashSpark"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorCrashSpark_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorFailSmoke"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorFailSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorFailSpark"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorFail_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorLockParticle"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorLockParticle_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorLockSmoke"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorLockSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorProgressSpark1"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorParticle.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorProgressSpark2"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorParticle2.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorProgressSparkF"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorParticleF.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorSuccesParticle"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork,TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorSuccesLightParticle_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_GeneratorSuccesSmokeUI"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/GeneratorSuccesUI_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_HuntressWeaponThrow"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/HuntressWeaponThrowWood_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_HatchGround"), CEffect_Hatch::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/HatchGround_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_HatchSmoke"), CEffect_Hatch::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/HatchSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_HookFlower"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/Hookflower_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_HookSmoke"), CEffect_HookSmoke::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/HookFlowerSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_PlantBroken"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/PlantBroken_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_SlasherSideSmoke"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/SlasherAttSideSmoke_r.txt")));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Effect_TotemSpark"), CEffect_Free3::Create(m_pDevice, m_pContext, pNetwork, TEXT("../Bin/Resource/03.GamePlay/Effect/TotemSpark_r.txt")));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Cube_Crack"), CCube_Free::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_GamePlay"), CProp_GamePlay::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_EscapeBlocker"), CProp_EscapeBlocker::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Corn"), CProp_Corn::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Grass"), CProp_Grass::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Grass2"), CProp_Grass2::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Grass3"), CProp_Grass3::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Grass4"), CProp_Grass4::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Prop_Grass5"), CProp_Grass5::Create(m_pDevice, m_pContext, pNetwork));
		
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Panel_First"), CUI_Play_Panel_First::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Panel_Second"), CUI_Play_Panel_Second::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Icon_Hud"), CUI_Play_Icon_Hud::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Icon_Propile"), CUI_Play_Icon_Propile::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Icon_Passive"), CUI_Play_Icon_Passive::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Icon_Perk"), CUI_Play_Icon_Perk::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Icon_SkillCheck"), CUI_Play_Icon_SkillCheck::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Gauge_Inter"), CUI_Play_Gauge_Inter::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Gauge_Exit"), CUI_Play_Gauge_Exit::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_End_Panel_First"), CUI_End_Panel_First::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_End_Panel_Second"), CUI_End_Panel_Second::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_End_Panel_FX"), CUI_End_Panel_FX::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_End_Btn_Return"), CUI_End_Btn_Return::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Panel_Manager"), CUI_Play_Panel_Manager::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_FPV_Trail"), CSlasher_FPV_Trail::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_Trail"), CSlasher_Trail::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Throw_Trail"), CThrow_Trail::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_Spirit_FPV_Trail"), CSlasher_Spirit_FPV_Trail::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Slasher_Spirit_Trail"), CSlasher_Spirit_Trail::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Ending_Spon"), CEnding_Spon::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Ending_Ground"), CEnding_Ground::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Ending_Grass"), CEnding_Grass::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Ending_PlayerPos"), CEnding_PlayerPos::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Ending_Camper"), CEnding_Camper::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Ending_Slasher"), CEnding_Slasher::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Camera_Ending"), CCamera_Ending::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Setting_CamperPos"), CSetting_CamperPos::Create(m_pDevice, m_pContext, pNetwork));
		hr |= pGame_Instance->Add_Object_Prototype(TEXT("Setting_BGM"), CSetting_BGM::Create(m_pDevice, m_pContext, pNetwork));

		hr |= pGame_Instance->Add_Object_Prototype(TEXT("UI_Play_Score"), CUI_Play_Score::Create(m_pDevice, m_pContext, pNetwork));
		break;
	}
	if (FAILED(hr))
		MSG_BOX("Fail to Loading");
	else
		m_bIsFinish = true;
	LeaveCriticalSection(&m_Critical_Section);
}
