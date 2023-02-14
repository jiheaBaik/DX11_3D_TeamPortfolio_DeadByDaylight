#pragma once
#include "Object.h"
#include "FM_Slasher.h"
#include "FS_Slasher_Idle_Intro.h"
#include "FS_Slasher_Idle_Slash.h"
#include "FS_Slasher_Idle_Carry.h"
#include "FS_Slasher_Idle_Hunt.h"
#include "FS_Slasher_Idle_Spirit.h"
#include "FS_Slasher_Run_Slash.h"
#include "FS_Slasher_Run_Carry.h"
#include "FS_Slasher_Run_Hunt.h"
#include "FS_Slasher_Run_Spirit.h"
#include "FS_Slasher_Attack_Carry_In.h"
#include "FS_Slasher_Attack_Carry_Swing.h"
#include "FS_Slasher_Attack_Carry_Out.h"
#include "FS_Slasher_Attack_Slash_In.h"
#include "FS_Slasher_Attack_Slash_Swing.h"
#include "FS_Slasher_Attack_Slash_Miss.h"
#include "FS_Slasher_Attack_Slash_Wipe.h"
#include "FS_Slasher_PickUp_In.h"
#include "FS_Slasher_PickUp.h"
#include "FS_Slasher_Stun_Drop_In.h"
#include "FS_Slasher_Stun_Drop_Out.h"
#include "FS_Slasher_Stun_Flash.h"
#include "FS_Slasher_Stun_Flash_In.h"
#include "FS_Slasher_Stun_Flash_Out.h"
#include "FS_Slasher_Stun.h"
#include "FS_Slasher_Stun_Locker.h"
#include "FS_Slasher_Throw_CoolDown.h"
#include "FS_Slasher_Throw_In.h"
#include "FS_Slasher_Throw_Swing.h"
#include "FS_Slasher_Spirit_Charge.h"
#include "FS_Slasher_Spirit_Out.h"
#include "FS_Slasher_Destroy_Gene.h"
#include "FS_Slasher_Destroy_Hatch.h"
#include "FS_Slasher_Destroy_Plank.h"
#include "FS_Slasher_Destroy_Totem.h"
#include "FS_Slasher_Locker_Grab.h"
#include "FS_Slasher_Locker_Reload.h"
#include "FS_Slasher_Locker_Search.h"
#include "FS_Slasher_Mori.h"
#include "FS_Slasher_Drop.h"
#include "FS_Slasher_Hook.h"
#include "FS_Slasher_Parkour.h"
#include "FS_Slasher_Parkour_Vault.h"
#include "FS_Slasher_Grab_Gene.h"
#include "FS_Slasher_Grab_Gene_In.h"
#include "FS_Slasher_Grab_Parkour_BK.h"
#include "FS_Slasher_Grab_Parkour_BK_In.h"
#include "FS_Slasher_Grab_Parkour_FT.h"
#include "FS_Slasher_Grab_Parkour_FT_In.h"

BEGIN_NAMESPACE(Client)
class CSlasher_Huntress final : public CObject {
public:
	enum class ECollisionState : _ubyte {
		NONE, LOCKER, PLANK, CAMPER_CRAWL, GENE_GRAB, PARKOUR_GRAB, HOOK, GENERATOR, HATCH, TOTEM, PARKOUR, PARKOUR_VAULT, END
	};
	enum class EType : _ubyte {
		SLASH, CARRY, HUNT, SPIRIT, END
	};
public:
	static CSlasher_Huntress* const Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	virtual CObject* const Clone(void* const& pArg = nullptr) override;
private:
	explicit CSlasher_Huntress(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork);
	explicit CSlasher_Huntress(const CSlasher_Huntress& rhs);
	virtual ~CSlasher_Huntress(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init_Create(void);
	const HRESULT Init_Clone(void* const& pArg = nullptr);
	virtual const _ubyte Update(const _double& dTimeDelta) override;
	virtual void Update_Edit(void) override;
	virtual void Late_Update(const _double& dTimeDelta) override;
	virtual const HRESULT Render(const CRenderer::EOrder& eOrder) override;
public:
	CFM_Slasher* Get_FM_Slasher(void) { return m_pFM; }

	CFM_Slasher::EState Get_State(void) { return m_pFM->Get_State(); }

	const _uint& Get_WeaponCount(void) { return m_iWeaponCount; }
	void Set_WeaponCount(const _int& iWeaponCount) { m_iWeaponCount += iWeaponCount; }
	void Set_WeaponMax(void) { m_iWeaponCount = m_iWeaponMax; }

	const ECollisionState& Get_CollisionState(void) { return m_eCollisionState; }
	void Set_CollisionState(const ECollisionState& eCollisionState) { m_eCollisionState = eCollisionState; }

	const EType& Get_Type(void) { return m_eType; }
	void Set_Type(const EType& eType);

	CObject* Get_ColObject(void) { return m_pColObject; }
	void Set_ColObject(CObject* pColObject) { m_pColObject = pColObject; }

	CObject* Get_TarObject(void) { return m_pTarObject; }
	void Set_TarObject(CObject* pTarObject) { m_pTarObject = pTarObject; }

	const _uint& Get_ColObjDir(void) { return m_iColObjDir; }
	void Set_ColObjDir(const _uint& iColObjDir) { m_iColObjDir = iColObjDir; }

	const _uint& Get_TarObjDir(void) { return m_iTarObjDir; }
	void Set_TarObjDir(const _uint& iTarObjDir) { m_iTarObjDir = iTarObjDir; }

	const _int& Get_TarObjID(void) { return m_iTarObjID; }
	void Set_TarObjID(const _int& iTarObjID) { m_iTarObjID = iTarObjID; }

	const _bool& Get_Slasher(void) { return m_bSlasher; }
	void Set_Slasher(const _bool& bSlasher) { m_bSlasher = bSlasher; }

	const _bool& Get_Stun(void) { return m_bStun; };
	void Set_Stun(const _bool& bStun) { m_bStun = bStun; }

	void Set_Accel(_float3 accel);
	void Set_IntroCam(const _bool& bIntroCam) { m_bIntroCam = bIntroCam; }

	void Set_Flag(const _bool& bFlag) { m_bFlag = bFlag; }

	const _bool& Get_Cancle(void) { return m_bCancle; }
	void Set_Cancle(const _bool& bCancle) { m_bCancle = bCancle; }

	const _bool& Get_LockerCancle(void) { return m_bLockerCancle; }
	void Set_LockerCancle(const _bool& bCancle) { m_bLockerCancle = bCancle; }

	const _bool& Get_Spirit(void) { return m_bSpirit; }
	void Set_Spirit(const _bool& bSpirit) { m_bSpirit = bSpirit; }

	const _bool& Get_Trail(void) { return m_bTrail; };
	void Set_Trail(const _bool& bTrail) { m_bTrail = bTrail; };

	const _double& Get_SpiritTime() { return m_dSpiritTime; }

	virtual void Update_Network(const string& pKey) override;
	virtual void Update_Network(void) override;

	//hwang
public:
	void Set_FindPerk(_double dDuringTime, _bool bFind);
private:
	void Check_Perk(_double dTimeDelta);
private:
	class CTransform* m_pTransform = nullptr;
	class CRenderer* m_pRenderer = nullptr;
	class CShader* m_pShader = nullptr;

	class CModel* m_pModel_FPV = nullptr;
	class CModel* m_pModel = nullptr;
	class CModel* m_pModel2 = nullptr;
	class CModel* m_pModel3 = nullptr;
	class CCollider* m_pCollider = nullptr;

	class CCollider* m_pCollider_Foot = nullptr;
	class CCollider* m_pCollider_Body = nullptr;
	class CCollider* m_pCollider_Eyes = nullptr;
	//camper
	class CTransform* m_pTransform_GrabLocker = nullptr;
	class CCollider* m_pCollider_GrabLocker = nullptr;
	class CTransform* m_pTransform_Grab = nullptr;
	class CCollider* m_pCollider_Grab = nullptr;

	class CNavi* m_pNavi = nullptr;
	class CAudio* m_pAudio = nullptr;
private:
	class CFM_Slasher* m_pFM = nullptr;
private:
	class CObject* m_pColObject = nullptr;
	class CObject* m_pTarObject = nullptr;

	ECollisionState m_eCollisionState = ECollisionState::NONE;
	EType m_eType = EType::SLASH;

	_uint m_iColObjDir = 0;
	_uint m_iTarObjDir = 0;
	_int m_iTarObjID = 0;

	_bool m_bSlasher = false;	// true = huntress / false = spirit

	_bool m_bStun = false;
	_bool m_bFlag = false;
	_bool m_bCancle = false;
	_bool m_bLockerCancle = false;
	_bool m_bLullaby = false;
	_double m_dCancleTime = 0.;
	_double m_dIntroTime = 0.;

	_bool m_bIntroCam = true;
private:
	_uint m_iWeaponCount = 5;
	_uint m_iWeaponMax = 5;

	_bool m_bSpirit = true;
	_bool m_bSaturation = false;
	_bool m_bTrail = false;
	_float m_fSpiritGauge = 0.f;
	_double m_dSpiritTime = 0.;
	_double m_dFootTime = 0.;
	
	vector<const char*> m_vecChange_Bone;
	vector<const char*> m_vecChange_Bone2;

	_bool m_bMouse = false;
	_bool m_bMove[4] = { false };
	_bool m_bCollider_Move = false;
	_float3 m_vCollider_Move{ 0.f, 0.f, 0.f };

	_float m_fNetworkTime = 0.f;

	_float3 m_vPos;
	_float3 m_vDir{ 0.f, 0.f, 1.f };
	_float3 m_vLook = {};
private:
	_float3 m_vAccel{ 0.f, 0.f, 0.f };
	_float m_fDist = 0.f;

	friend CFM_Slasher;
	friend CFS_Slasher_Idle_Intro;
	friend CFS_Slasher_Idle_Slash;
	friend CFS_Slasher_Idle_Carry;
	friend CFS_Slasher_Idle_Hunt;
	friend CFS_Slasher_Idle_Spirit;
	friend CFS_Slasher_Run_Slash;
	friend CFS_Slasher_Run_Carry;
	friend CFS_Slasher_Run_Hunt;
	friend CFS_Slasher_Run_Spirit;
	friend CFS_Slasher_Attack_Carry_In;
	friend CFS_Slasher_Attack_Carry_Swing;
	friend CFS_Slasher_Attack_Carry_Out;
	friend CFS_Slasher_Attack_Slash_In;
	friend CFS_Slasher_Attack_Slash_Swing;
	friend CFS_Slasher_Attack_Slash_Miss;
	friend CFS_Slasher_Attack_Slash_Wipe;
	friend CFS_Slasher_PickUp_In;
	friend CFS_Slasher_PickUp;
	friend CFS_Slasher_Stun_Drop_In;
	friend CFS_Slasher_Stun_Drop_Out;
	friend CFS_Slasher_Stun_Flash;
	friend CFS_Slasher_Stun_Flash_In;
	friend CFS_Slasher_Stun_Flash_Out;
	friend CFS_Slasher_Stun;
	friend CFS_Slasher_Stun_Locker;
	friend CFS_Slasher_Throw_CoolDown;
	friend CFS_Slasher_Throw_In;
	friend CFS_Slasher_Throw_Swing;
	friend CFS_Slasher_Spirit_Charge;
	friend CFS_Slasher_Spirit_Out;
	friend CFS_Slasher_Destroy_Gene;
	friend CFS_Slasher_Destroy_Hatch;
	friend CFS_Slasher_Destroy_Plank;
	friend CFS_Slasher_Destroy_Totem;
	friend CFS_Slasher_Locker_Grab;
	friend CFS_Slasher_Locker_Reload;
	friend CFS_Slasher_Locker_Search;
	friend CFS_Slasher_Mori;
	friend CFS_Slasher_Drop;
	friend CFS_Slasher_Hook;
	friend CFS_Slasher_Pakour;
	friend CFS_Slasher_Pakour_Vault;
	friend CFS_Slasher_Grab_Gene;
	friend CFS_Slasher_Grab_Gene_In;
	friend CFS_Slasher_Grab_Parkour_BK;
	friend CFS_Slasher_Grab_Parkour_BK_In;
	friend CFS_Slasher_Grab_Parkour_FT;
	friend CFS_Slasher_Grab_Parkour_FT_In;

private:
	class CEffect_Free3* m_pEffect = nullptr;
	_bool m_bSideEffectFirst = true;
private:
	_uint m_iHookCount = 0;

private:
	_float m_fKickGenerator = 1.0f;				//KickGenerator		//use
	_float m_fPerkSpeed = 0.f;					//TOTEM_SPEED		//use
	_bool m_bFindPerkRender = false;			//Find				//use
	_bool m_bLockGene = false;					//LockGenerator		//use UI
	_double m_dFindPerkTime = 5.0;				//Find				//use

	_bool m_bTotemSpeed = false;
private:
	list<class CObject*>* m_plistCamper = nullptr;
	_float m_fDissolveLength = 2.f;
	_float m_fGoalDissolveLength = 0.f;
	private:
		_float m_fRoughness = 0.275f;
		_float m_fRefract = 0.275f;
		_float m_fMetalness = 0.f;
		_float m_fMetalness2 = 0.f;
};
END_NAMESPACE