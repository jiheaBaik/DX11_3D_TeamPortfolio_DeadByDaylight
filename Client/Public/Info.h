#pragma once
#include "Object.h"

BEGIN_NAMESPACE(Client)
class CInfo final : public CBase {
public:
	enum class ECamper : _ubyte { CHERYL, BILL, DWIGHT, MEG, END };
	enum class ESlasher : _ubyte { HUNTRESS, SPIRIT, END };
	DECLARE_SINGLETON(CInfo);
private:
	explicit CInfo(void);
	virtual ~CInfo(void) override = default;
	virtual void Delete(void) override;
public:
	void Update(void);
public:
	void Update_Network(const string& pKey);
	void Update_Network(void);
public:
	void Set_Ready5(void);

	const _bool& Get_MyReady(void);
	void Set_MyReady(const _bool& bReady);
	const _uint& Get_MyCamperSlot(void);
	void Set_MyCamperSlot(const _uint& iSlot);
	void Init_pCamperSlot(void);
	class CCamper_Lobby** const Get_pCamperSlot(void);
	void Set_pCamperSlot(const _uint& iIndex, class CCamper_Lobby* const pCamper);

	const _uint& Get_Ready(void);
	void Add_Ready(const _int& iAdd);
	const _uint& Get_CamperSlot(void);
	void Add_CamperSlot(const _int& iAdd);
	const _uint& Get_SlasherSlot(void);
	void Add_SlasherSlot(void);

	const _bool& Get_Player(void);
	void Set_Player(const _bool& bPlayer);
	const ECamper& Get_Camper(void);
	void Set_Camper(const ECamper& eCamper);
	const ESlasher& Get_Slasher(void);
	void Set_Slasher(const ESlasher eSlasher);
public:
	void Init_CamperCount(void);
	const _uint& Get_CamperCount(void);
	void Add_CamperCount(const _int& iAdd);
private:
	class CNetwork* m_pNetwork = nullptr;
private:
	_bool m_bReady = false;
	_uint m_iMyCamperSlot = 0;
	class CCamper_Lobby* m_pCamperSlot[4] = {nullptr, nullptr, nullptr, nullptr };

	_uint m_iReady = 0;
	_uint m_iCamperSlot = 0;
	_uint m_iSlasherSlot = 0;

	_bool m_bPlayer = false; // false: Camper true: Slsher
	ECamper m_eCamepr = ECamper::DWIGHT;
	ESlasher m_eSlasher = ESlasher::HUNTRESS;
private:
	_uint m_iCamperCount = 0;
};
END_NAMESPACE