#include "pch.h"
#include "FS_Camper_Chest_Loop.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_ChestBox.h"

CFS_Camper_Chest_Loop::CFS_Camper_Chest_Loop(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Chest_Loop::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Chest_Loop::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	m_dChestTime = 0.;
	if (m_pCamper_GamePlay->m_bControl) {
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		m_pChestBox = static_cast<CInter_ChestBox*>(m_pCamper_GamePlay->m_pTarObject);
	}
	return S_OK;
}

const _ubyte  CFS_Camper_Chest_Loop::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			m_dChestTime = 0.;
			m_pChestBox->Set_State(CInter_ChestBox::ESTATE::IDLE);
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;

		}

		m_dChestTime += dTimeDelta;
		if (m_dChestTime > 0.1) {
			m_pChestBox->Add_Gauge(1.f);
			m_dChestTime = 0.;
		}

		//랜덤 아이템 줍기
		if (m_pChestBox->Get_Gauge() >= 100.) {
			m_dChestTime = 0.;
			m_pChestBox->Set_State(CInter_ChestBox::ESTATE::OPEN_END);
			return CFM_Camper::EState::CHEST_OUT;
		}
	}
	else {
		//랜덤 아이템 줍기
		if (m_pChestBox->Get_Gauge() >= 100.) {
			m_dChestTime = 0.;

			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
			m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Chest_Out");
			break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_Chest_Out");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_Chest_Out");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_Chest_Out");
				break;
			}
		}
	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
