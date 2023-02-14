#include "pch.h"
#include "FS_Slasher_Grab_Parkour_FT.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Score.h"

CFS_Slasher_Grab_Parkour_FT::CFS_Slasher_Grab_Parkour_FT(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Grab_Parkour_FT::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Grab_Parkour_FT::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Grab_Parkour_FT::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(22);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(22);
	}
	return S_OK;
}

const _ubyte  CFS_Slasher_Grab_Parkour_FT::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::CARRY;
			m_pSlasher_Huntress->Update_Network("Set_Type");

			//m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("mu_killer_carrying"), 2, 2, 0.3f, FMOD_2D | FMOD_LOOP_NORMAL);

			//hwang - score
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::BRUTALLITY_CATCH));
				pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::BRUTALITY), 600);
			}
			return CFM_Slasher::EState::IDLE_CARRY;
		}
	}
	else {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
