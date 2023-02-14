#include "pch.h"
#include "FS_Camper_Chest_PickOut.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_ChestBox.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_Chest_PickOut::CFS_Camper_Chest_PickOut(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Chest_PickOut::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Chest_PickOut::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
		m_pCamper_GamePlay->m_bItem[(_ubyte)m_pChestBox->Get_ItemKind()] = 1;

		_uint iIdx = static_cast<_uint>(m_pChestBox->Get_ItemKind());
		dynamic_cast<CUI_Play_Icon_Passive*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("UI_Play_Icon_Passive")))->Push_Slot(iIdx);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_ChestBox"), m_pCamper_GamePlay->m_iTarObjID);
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
		m_pCamper_GamePlay->m_bItem[(_ubyte)m_pChestBox->Get_ItemKind()] = 1;
	}

	return S_OK;
}

const _ubyte  CFS_Camper_Chest_PickOut::Update(const _double& dTimeDelta) {

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			if (2 == m_pCamper_GamePlay->Get_Hp()) {
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_StandIdle");
					break;
				}
			}
			else if (1 == m_pCamper_GamePlay->Get_Hp()) {
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_inj_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_inj_StandIdle");
					break;
				}
			}
		}
			
	}
	return CFM_Camper::EState::END;


}
