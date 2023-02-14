#include "pch.h"
#include "FS_Camper_Carry_Idle.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Slasher_Huntress.h"
#include "Inter_Hook.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_Carry_Idle::CFS_Camper_Carry_Idle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_Carry_Idle::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Carry_Idle::Init(_char* pModelName) {

	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	//m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f), 1.f);
	//
	_matrix m_fRootOffsetMatrix = XMLoadFloat4x4(&m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char"));
	_matrix pivot = m_fRootOffsetMatrix * m_pCamper_GamePlay->m_pModel->Get_BoneMatrix("joint_Char") * matrix;
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, -pivot.r[0]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, -pivot.r[2]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, -pivot.r[1]);
	/*m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName,0.f);
	m_pCamper_GamePlay->m_pModel->Update_Anim(0, true);*/
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName, 0.);
	m_pCamper_GamePlay->m_pModel->Update_Anim(0.f);
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pHuntress = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
	if (m_pCamper_GamePlay->m_bControl) {
		// set hud
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, -1);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Gauge_Inter")))->Set_RenderAll(false, false);
	}

	return S_OK;
}

const _ubyte  CFS_Camper_Carry_Idle::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (nullptr != m_pHuntress) {
		CModel* pHuntressModel = dynamic_cast<CModel*>(m_pHuntress->Get_Component(TEXT("Model_FPV")));
		CTransform* pHuntressTransform = dynamic_cast<CTransform*>(m_pHuntress->Get_Component(TEXT("Transform")));
		_matrix SocketMatrix = pHuntressModel->Get_BoneMatrix("joint_Char") * pHuntressTransform->Get_Matrix();
		m_pCamper_GamePlay->m_pTransform->Set_Matrix(SocketMatrix);
	}
	if (m_pCamper_GamePlay->m_bControl) {
		m_dtime += dTimeDelta;
		if (m_dtime > 2.) {
			m_dtime = 0.;
			dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_HitCount(4, 25, CUI_Play_Icon_SkillCheck::ECheck::START, 200);
			return CFM_Camper::EState::CARRY_WIGGLE;
		}

		//if (m_pCamper_GamePlay->m_bControl) {
		//	if (!m_bFlag) {
		//		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_A)) {//UIÇÊ¿ä ¸öºÎ¸²
		//			m_bFlag = !m_bFlag;
		//			return CFM_Camper::EState::Carry_Wiggle;
		//		}
		//	}
		//	else {
		//		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_D)) {
		//			m_bFlag = !m_bFlag;
		//			return CFM_Camper::EState::Carry_Wiggle;
		//		}
		//	}
			//hwang
		CInven* pInven = CInven::Get_Instance();
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_R)) {
			if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::ESCAPE_CARRY))) {
				m_pHuntress->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_DROP_IN);
				m_pHuntress->Get_FM_Slasher()->Update_Network("Set_State");
				m_pCamper_GamePlay->m_bEscapeCarry = true;
				//hwang - score
				CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
				if (pScore != nullptr) {
					pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::SURVIVAL_KILLER_ESCAPE));
					pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::SURVIVAL),500);
				}
				return CFM_Camper::EState::STUNDROP;
			}
		}
	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}


