#include "pch.h"
#include "FS_Camper_TotemOut.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Totem.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Score.h"

CFS_Camper_TotemOut::CFS_Camper_TotemOut(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_TotemOut::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_TotemOut::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pTotem = static_cast<CInter_Totem*>(m_pCamper_GamePlay->m_pTarObject);

	return S_OK;
}

const _ubyte  CFS_Camper_TotemOut::Update(const _double& dTimeDelta) {

	if (m_pCamper_GamePlay->m_bControl)
	{
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
		{
			//hwang - perk
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			CInven* pInven = CInven::Get_Instance();
			if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::PURIFICATION))) {
				CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
				if (pSlasher != nullptr) {
					pSlasher->Set_FindPerk(15.0, true);
				}
			}
			//hwang - score
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::BOLDNESS_TOTEM));
				pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::BOLDNESS), 1000);
			}
			m_pTotem->Set_State(CInter_Totem::ESTATE::ON);
			//m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_totem_destroy"), 2, -1, 0.5f, FMOD_2D);

			//m_pCamper_GamePlay->m_TotemTime = 0.;
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}

	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			m_pTotem->Set_State(CInter_Totem::ESTATE::ON);
		//	m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_totem_destroy"), 2, -1, 0.5f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

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
