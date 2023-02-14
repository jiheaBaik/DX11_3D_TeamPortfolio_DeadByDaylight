#include "pch.h"
#include "FS_Camper_Chest_PickIn.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_ChestBox.h"
#include "UI_Play_Icon_Hud.h"

CFS_Camper_Chest_PickIn::CFS_Camper_Chest_PickIn(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Chest_PickIn::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Chest_PickIn::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, CUI_Play_Icon_Hud::EDiv::CENTER);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_ChestBox"), m_pCamper_GamePlay->m_iTarObjID);
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
	}
	return S_OK;
}

const _ubyte  CFS_Camper_Chest_PickIn::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_pChestBox->Set_IsUse(true);
			
			//CTransform* m_pItemTarnsform = dynamic_cast<CTransform*>(m_pCamper_GamePlay->Get_Component(TEXT("Transform2")));
			//m_pItemTarnsform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(10.f));
			return CFM_Camper::EState::CHEST_PICKOUT;
		}
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {

			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Chest_Loot_Perk_Out");
			break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Chest_Loot_Perk_Out");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Chest_Loot_Perk_Out");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Chest_Loot_Perk_Out");
				break;
			}
		}
	}
	return CFM_Camper::EState::END;
}
