#pragma once

BEGIN_NAMESPACE(Client)
class CFM_Slasher final : public CBase {
public:
	enum EState : _ubyte {
		IDLE_INTRO, IDLE_SLASH, IDLE_CARRY, IDLE_HUNT, IDLE_SPIRIT,
		RUN_SLASH_FT, RUN_SLASH_BK, RUN_SLASH_RT, RUN_SLASH_LT, RUN_SLASH_FTRT, RUN_SLASH_FTLT, RUN_SLASH_BKRT, RUN_SLASH_BKLT,
		RUN_CARRY_FT, RUN_CARRY_BK, RUN_CARRY_RT, RUN_CARRY_LT, RUN_CARRY_FTRT, RUN_CARRY_FTLT, RUN_CARRY_BKRT, RUN_CARRY_BKLT,
		RUN_HUNT_FT, RUN_HUNT_BK, RUN_HUNT_RT, RUN_HUNT_LT, RUN_HUNT_FTRT, RUN_HUNT_FTLT, RUN_HUNT_BKRT, RUN_HUNT_BKLT,
		RUN_SPIRIT_FT, RUN_SPIRIT_BK, RUN_SPIRIT_RT, RUN_SPIRIT_LT, RUN_SPIRIT_FTRT, RUN_SPIRIT_FTLT, RUN_SPIRIT_BKRT, RUN_SPIRIT_BKLT,
		ATTACK_CARRY_IN, ATTACK_CARRY_SWING, ATTACK_CARRY_OUT, ATTACK_SLASH_IN, ATTACK_SLASH_SWING, ATTACK_SLASH_MISS, ATTACK_SLASH_WIPE,
		PICKUP_IN, PICKUP, HOOK, DROP, STUN_DROP_IN, STUN_DROP_OUT, STUN_FLASH, STUN_FLASH_IN, STUN_FLASH_OUT, STUN, STUN_LOCKER,
		THROW_COOLDOWN, THROW_IN, THROW_SWING, MORI, PARKOUR, PARKOUR_VAULT,
		SPIRIT_CHARGE, SPIRIT_OUT, SPIRIT_OUT_BK, SPIRIT_OUT_LT, SPIRIT_OUT_RT,
		DESTROY_GENE, DESTROY_HATCH, DESTROY_PLANK, DESTROY_TOTEM, LOCKER_GRAB, LOCKER_RELOAD, LOCKER_SEARCH,
		GRAB_GENE, GRAB_GENE_IN, GRAB_PARKOUR_BK, GRAB_PARKOUR_BK_IN, GRAB_PARKOUR_FT, GRAB_PARKOUR_FT_IN, END
	};
public:
	explicit CFM_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork, class CSlasher_Huntress* const& pSlasher_Huntress);
	virtual ~CFM_Slasher(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(void);
	void Update(const _double& dTimeDelta);
public:
	const EState& Get_State(void) { return m_eState; }
	void Set_State(const EState& eState);

	const _bool& Get_CamMori(void);
	void Set_CamMori(const _bool& bCamMori);

	void Update_Network(const string& pKey);
	void Update_Network(void);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	class CNetwork* m_pNetwork = nullptr;

	class CSlasher_Huntress* m_pSlasher_Huntress = nullptr;
	class CFS* m_pFS[EState::END];
	EState m_eState = EState::IDLE_INTRO;

	_bool m_bCamMori = false;
};
END_NAMESPACE;