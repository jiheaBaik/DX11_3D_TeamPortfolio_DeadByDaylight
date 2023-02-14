#pragma once
#include "Object.h"
#include "FM_Camper.h"
#include "Navi.h"

BEGIN_NAMESPACE(Client)
class CCamper_GamePlay final : public CObject {
public:
	enum class ECollisionState : _ubyte {
		NONE, LOCKER, PLANK, ChestBox, HOOK, GENERATOR, HATCH, EXITDOOR, PARKOUR, PARKOUR_VAULT, CAMPER, TOTEM, END
	};
	enum class EItemKind : _ubyte {
		MEDIKIT, FLASHLIGHT, END
	};
	enum class ECamperKind : _ubyte {
		CHERYL, BILL, DWIGHT, MEG, END
	};
	static CCamper_GamePlay* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CCamper_GamePlay(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CCamper_GamePlay(const CCamper_GamePlay& rhs);
	virtual ~CCamper_GamePlay(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;

	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;
	class CModel* m_pModel = nullptr;
	class CCollider* m_pCollider = nullptr;
	class CCollider* m_pCollider_Foot = nullptr;
	class CCollider* m_pCollider_Body = nullptr;
private:
	class CTransform* m_pItemTransform = nullptr;
	class CModel* m_pMediKit_Model = nullptr;
	class CModel* m_pFlashLight_Model = nullptr;
	class CCollider* m_pFlashLightCollider = nullptr;

	class CShader* m_pShader2 = nullptr;
	class CNavi* m_pNavi = nullptr;
	class CAudio* m_pAudio = nullptr;
private:
	class CCollider* m_pOtherCamperCollider = nullptr;
	class CCollider* m_pGrassCollider = nullptr;
	class CTarget* m_pTarget_Depth = nullptr;
public:
	class CFM_Camper* m_pFM = nullptr;

private:
	friend CFM_Camper;
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
	friend CFS_Camper_HatchOut;
	friend CFS_Camper_Closet_Grap;
	friend CFS_Camper_Closet_Entering;
	friend CFS_Camper_Pick1;
	friend CFS_Camper_Pick2;
	friend CFS_Camper_Carry_Idle;
	friend CFS_Camper_Carry_Wiggle;
	friend CFS_Camper_Drop;
	friend CFS_Camper_HookIn;
	friend CFS_Camper_HookOut;
	friend CFS_Camper_HookIdle;
	friend CFS_Camper_HookStruggle;
	friend CFS_Camper_HookFree;
	friend CFS_Camper_StunDrop;
	friend CFS_Camper_Spider_IN;
	friend CFS_Camper_Spider_Loop;
	friend CFS_Camper_Spider_Out;
	friend CFS_Camper_Hook_Death;
	friend CFS_Camper_Spider_Struggle;
	friend CFS_Camper_HookIdle2;
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









public:
	ECollisionState Get_CollisionState(void) { return m_eCollisionState; }
	void Set_CollisionState(ECollisionState eCollisionState) { m_eCollisionState = eCollisionState; }

	CObject* Get_ColObject(void) { return m_pColObject; }
	void Set_ColObject(CObject* pColObject) { m_pColObject = pColObject; }

	CObject* Get_TarObject(void) { return m_pTarObject; }
	void Set_TarObject(CObject* pTarObject) { m_pTarObject = pTarObject; }
public:
	CFM_Camper* Get_FM_Camper(void) { return m_pFM; }
public:
	void Set_Render(_bool bisRender) { m_bisRender = bisRender; }

	_bool Get_Hide() { return m_isHide; }
	void Set_Hide(_bool bisHide) { m_isHide = bisHide; }

	_uint Get_Hp() { return m_iHP; }
	void Set_AddHp(_int hp) { m_iHP += hp; }
	void Set_Hp(_int hp) { m_iHP = hp; }

	//_bool Get_IsItem() { return m_bisGetItem; }
	void Set_RenderItem(_bool isItem) { m_bisGetItem = isItem; }

	_uint Get_ColObjDir() { return m_iColObjDir; }
	void Set_ColObjDir(_uint iColObjDir) { m_iColObjDir = iColObjDir; }

	_uint Get_TarObjDir() { return m_iTarObjDir; }
	void Set_TarObjDir(_uint iTarObjDir) { m_iTarObjDir = iTarObjDir; }

	_int Get_TarObjID() { return m_iTarObjID; }
	void Set_TarObjID(_int iTarObjID);

	const _bool& Get_End(void) { return m_bIsEnd; }
	void Set_End(const _bool& bExit) { m_bIsEnd = bExit; }

	const _bool& Get_IsItem(const EItemKind& eKind) { return m_bItem[static_cast<_uint>(eKind)]; }
	const _float& Get_SelfHealGauge() { return static_cast<_float>(m_SelfHealGauge); }

	const _float& Get_SpiderTime() { return m_fSpiderTime; }
	const _uint& Get_SpiderRound() { return m_fSpiderRound; }

	const _double& Get_HealGauge(void) { return m_HealGauge; }
	const ECamperKind& Get_CamperKind(void) { return m_eCampKind; }

	// �ǰ�����Ʈ ȣ���Լ� - �¿� 221108
public:
	void Call_BloodEffect(_uint type);

private: // axcel
	void Set_Accel(_float3 accel);

	//hwang
public: // FindCamper Perk - hwang
	void Set_FindPerk(_double dDuringTime, _bool bFind);
	const _bool& Get_HaveStayCrowPerk() const;
private: //Inven - hwang
	void Check_Perk(_double dTimeDelta);

private: //control
	class CObject* m_pColObject = nullptr;
	class CObject* m_pTarObject = nullptr;
	ECollisionState m_eCollisionState = ECollisionState::NONE;

	_uint m_iColObjDir = 0;
	_uint m_iTarObjDir = 0;

	_int m_iTarObjID = 0;
private:
	_bool m_bisRender = true;
	_bool m_isHide = false;
	_bool m_bisGetItem = false;
	_uint m_iHP = 2;
	_bool m_bItem[(_uint)EItemKind::END] = { 0, };
private://Gauge
	_float m_dWiggleGauge = 0.;
	_double m_SelfHealGauge = 0.;
	_double m_HealGauge = 0.;

private:
	_float m_fNetworkTime = 0.f;
	_float3 m_vPos;
	_float3 m_vDir = { 0.f,0.f,1.f };
	_float3 m_vDotDir = { 0.f, 0.f, 0.f };
	_float m_fAngle = 0.f;
private://Hook spider
	_float m_fSpiderTime = 60.f;
	//_float m_fSpiderMaxTime = 120.f;
	_float m_fSpiderHP = 100.f;
	_uint m_fSpiderRound = 0;
	_bool m_isAttack = false;

private:
	_float3 m_vAccel{ 0.f, 0.f, 0.f };
	_bool m_bFlag = false;
	_bool m_bIsCollider_Foot = true;
	_bool m_bMouse = false;
	_bool m_bIsEnd = false;

	//hwang
private:
	_float m_fPerkSpeed = 0.f;				//Network	
	_bool m_bRunAway = false;				//Network	RunAway
	_bool m_bStopRunAway = false;			//Network	RunAway
	_bool m_bEndGene = false;				//Network	END_GENERATOR
	_bool m_bFast = false;					//Network	FAST
	_bool m_bStopFast = false;				//Network	FAST
	_bool m_bEscapeCarry = false;
	_bool m_bFindPerkRender = false;		//FindCamper		//use
	_bool m_bHaveStayCrowPerk = false;		//Stay_Crow			//use
	_bool m_bIsEndGenerator = false;		//End_Generator		//use
	_double m_dFindPerkTime = 5.0;			//FindCamper		//use
private:
	_float m_fRoughness = 0.275f;
	_float m_fRefract = 0.275f;
	_float m_fMetalness = 0.f;
	_float m_fMetalness2 = 0.f;
private:
	class CEffect_FlashLight* m_pEffect[2] = { nullptr };
	class CLight_Flash* m_pLight = nullptr;
	_bool m_bLightFirstAtk = true;
	_bool m_bCollider_Move = false;
	_float3 m_vCollider_Move = { 0.f, 0.f, 0.f };
private:
	CObject* m_pSlasher = nullptr;
	class CTransform* m_pSlasherTransform = nullptr;
private:
	_double m_dSoundTime = 0.;
	_float m_fVolume = 0.f;
private:
	ECamperKind m_eCampKind = ECamperKind::END;
	// yeon effect
private:
	_bool m_bIsMedikitPerk = false;
private:
	class CEffect_Fog* m_pFogEffect[15] = { nullptr };
	class CEffect_Free3* m_pSideSmokeEffect = nullptr;
	_double m_dFogCreateTime = 0.;
	_bool m_bHookFirst = true;
	_bool m_bHookDeath = false;
	_float m_fDissolveLength = 0.f;
	_float3 m_fDissolveColor = { 8.f, 0.f,0.f };
	_float3 m_fDissolveColor2 = { 8.f, 1.5f,0.f };
	_bool m_bPlayBGM = true;
	_float m_fDist = 3.f;
	_float3 m_fPrePos = { 0.f,0.f,0.f };
};
END_NAMESPACE