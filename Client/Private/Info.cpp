#include "pch.h"
#include "Info.h"

IMPLEMENT_SINGLETON(CInfo)

CInfo::CInfo(void)
	: CBase() {
	m_pNetwork = CNetwork::Get_Instance();
}

void CInfo::Delete(void) {
}

void CInfo::Update(void) {
	Update_Network();
}

void CInfo::Update_Network(const string& pKey) {
	Json::Value root;
	if ("Plus_CamperSlot" == pKey) {
		m_pNetwork->Set_Send(200, pKey, root);
	}
	if ("Minus_CamperSlot" == pKey) {
		m_pNetwork->Set_Send(200, pKey, root);
	}
	if ("Plus_Ready" == pKey) {
		m_pNetwork->Set_Send(200, pKey, root);
	}
	if ("Minus_Ready" == pKey) {
		m_pNetwork->Set_Send(200, pKey, root);
	}
	if ("Set_Ready5" == pKey) {
		m_pNetwork->Set_Send(200, pKey, root);
	}
	if ("Plus_CamperCount" == pKey) {
		m_pNetwork->Set_Send(200, pKey, root);
	}
	if ("Minus_CamperCount" == pKey) {
		m_pNetwork->Set_Send(200, pKey, root);
	}
}

void CInfo::Update_Network(void) {
	list<Json::Value>& list = m_pNetwork->Get_Recv(200);
	for (auto& root : list) {
		string Key = root["Key"].asString();
		if ("Plus_CamperSlot" == Key) {
			++m_iCamperSlot;
		}
		if ("Minus_CamperSlot" == Key) {
			--m_iCamperSlot;
		}
		if ("Plus_Ready" == Key) {
			++m_iReady;
		}
		if ("Minus_Ready" == Key) {
			--m_iReady;
		}
		if ("Set_Ready5" == Key) {
			m_iReady = 5;
		}
		if ("Plus_CamperCount" == Key) {
			++m_iCamperCount;
		}
		if ("Minus_CamperCount" == Key) {
			--m_iCamperCount;
		}
	}
	list.clear();
}

class CCamper_Lobby** const CInfo::Get_pCamperSlot(void) {
	return m_pCamperSlot;
}

void CInfo::Set_pCamperSlot(const _uint& iIndex, class CCamper_Lobby* const pCamper) {
	m_pCamperSlot[iIndex] = pCamper;
}

const _uint& CInfo::Get_CamperSlot(void) {
	return m_iCamperSlot;
}

void CInfo::Add_CamperSlot(const _int& iAdd) {
	m_iCamperSlot += iAdd;
	if (iAdd > 0) 
		Update_Network("Plus_CamperSlot");
	if (iAdd < 0)
		Update_Network("Minus_CamperSlot");
}

const _uint& CInfo::Get_Ready(void) {
	return m_iReady;
}
void CInfo::Add_Ready(const _int& iAdd) {
	m_iReady += iAdd;
	if (iAdd > 0)
		Update_Network("Plus_Ready");
	if (iAdd < 0)
		Update_Network("Minus_Ready");
}

void CInfo::Set_Ready5(void) {
	m_iReady = 5;
	Update_Network("Set_Ready5");
}

const _bool& CInfo::Get_MyReady(void) {
	return m_bReady;
}

void CInfo::Set_MyReady(const _bool& bReady) {
	m_bReady = bReady;
}

const _uint& CInfo::Get_MyCamperSlot(void) {
	return m_iMyCamperSlot;
}

void CInfo::Set_MyCamperSlot(const _uint& iSlot) {
	m_iMyCamperSlot = iSlot;
}

void CInfo::Init_pCamperSlot(void) {
	ZeroMemory(m_pCamperSlot, sizeof(CCamper_Lobby*) * 4);
}

const _uint& CInfo::Get_SlasherSlot(void) {
	return m_iSlasherSlot;
}

const _bool& CInfo::Get_Player(void) {
	return m_bPlayer;
}

void CInfo::Set_Player(const _bool& bPlayer) {
	m_bPlayer = bPlayer;
}

const CInfo::ECamper& CInfo::Get_Camper(void) {
	return m_eCamepr;
}

void CInfo::Set_Camper(const ECamper& eCamper) {
	m_eCamepr = eCamper;
}

const CInfo::ESlasher& CInfo::Get_Slasher(void) {
	return m_eSlasher;
}

void CInfo::Set_Slasher(const ESlasher eSlasher) {
	m_eSlasher = eSlasher;
}

void CInfo::Init_CamperCount(void) {
	m_iCamperCount = m_iCamperSlot;
}

const _uint& CInfo::Get_CamperCount(void) {
	return m_iCamperCount;
}

void CInfo::Add_CamperCount(const _int& iAdd) {
	m_iCamperCount += iAdd;
	if (iAdd > 0)
		Update_Network("Plus_CamperCount");
	if (iAdd < 0)
		Update_Network("Minus_CamperCount");
}