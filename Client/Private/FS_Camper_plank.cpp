#include "pch.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_PlankAnim.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Score.h"

CFS_Camper_plank::CFS_Camper_plank(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;

}

void CFS_Camper_plank::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_plank::Init(_char* ModelName) {
	//sfx_body_rush_slide_bounce
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pPlank = static_cast<CInter_PlankAnim*>(m_pCamper_GamePlay->m_pTarObject);
		m_iPlankDir = m_pCamper_GamePlay->m_iTarObjDir;
		// set hud
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(CUI_Play_Icon_Hud::EDiv::CENTER, true);
	
		//hwang - score
		CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
		if (pScore != nullptr) {
			if (m_pCamper_GamePlay->m_pFM->Get_State() == CFM_Camper::EState::PULLDOWN_PLANK_L
				|| m_pCamper_GamePlay->m_pFM->Get_State() == CFM_Camper::EState::PULLDOWN_PLANK_R){
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::BOLDNESS_PALLET_DROP));
				pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::BOLDNESS), 200);
			}
			
		}
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_PlankAnim"), m_pCamper_GamePlay->m_iTarObjID);
		m_pPlank = static_cast<CInter_PlankAnim*>(m_pCamper_GamePlay->m_pTarObject);
		m_iPlankDir = m_pCamper_GamePlay->m_iTarObjDir;
	}

	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);
	m_flag = false;

	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);

	m_pPlankTransform = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform")));
	enum Dir { LEFT, RIGHT, END };
	switch (m_iPlankDir) {
	case LEFT:
		m_pPlankTransform2 = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform_L")));
		break;
	case RIGHT:
		m_pPlankTransform2 = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform_R")));
		break;
	}
	XMStoreFloat3(&m_fRootPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));
	
	
	return S_OK;
}

const _ubyte  CFS_Camper_plank::Update(const _double& dTimeDelta) {
	XMMATRIX PlankMatrix = m_pPlankTransform2->Get_Matrix() * m_pPlankTransform->Get_Matrix();
	_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLerpPos = PlankMatrix.r[3] * 0.2f + XMLoadFloat3(&m_fRootPrePos) * 0.8f;
	XMStoreFloat3(&m_fRootPrePos, vLerpPos);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

	_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
	_vector vChestQaut = XMQuaternionRotationMatrix(PlankMatrix);
	_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
	_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

	_vector vRootPos;
	_float4x4 vSocket;
	CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));

	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);

	vRootPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vRootPos);


	if (true == m_pCamper_GamePlay->m_bControl) {
		
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(7)) {
			if (m_pCamper_GamePlay->m_pFM->Get_State() == m_pCamper_GamePlay->m_pFM->_JUMPOVER_FASTL ||
				m_pCamper_GamePlay->m_pFM->Get_State() == m_pCamper_GamePlay->m_pFM->_JUMPOVER_FASTR)
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_body_rush_slide_bounce"), 4, -1, 0.2f, FMOD_2D);
		}
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			//hwang
			CInven* pInven = CInven::Get_Instance();
			if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FAST))) {
				m_pCamper_GamePlay->m_bFast = true;
				m_pCamper_GamePlay->m_bStopFast = true;
			}
				
			if (2 == m_pCamper_GamePlay->Get_Hp()) {
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)){
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						return CFM_Camper::EState::RUN;
					}
				}
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
					return CFM_Camper::EState::WALK_F;
				}
				else
					return CFM_Camper::EState::IDLE;
			}

			else if (1 == m_pCamper_GamePlay->Get_Hp()) {

				if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						return CFM_Camper::EState::HIT_RUN;
					}
				}
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
					return CFM_Camper::EState::HIT_WALK;
				}
				else
					return CFM_Camper::EState::HIT_IDLE;
			}
		}
	}
	else {
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(7)) {
			if (m_pCamper_GamePlay->m_pFM->Get_State() == m_pCamper_GamePlay->m_pFM->_JUMPOVER_FASTL ||
				m_pCamper_GamePlay->m_pFM->Get_State() == m_pCamper_GamePlay->m_pFM->_JUMPOVER_FASTR)
				m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_body_rush_slide_bounce"), 4, -1, 0.2f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
		}
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
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
