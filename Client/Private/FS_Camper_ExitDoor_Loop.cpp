#include "pch.h"
#include "FS_Camper_ExitDoor_Loop.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_ExitDoor.h"
#include "Effect_Free3.h"
#include "Light_Generator.h"
#include "UI_Play_Score.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"

CFS_Camper_ExitDoor_Loop::CFS_Camper_ExitDoor_Loop(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_ExitDoor_Loop::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_ExitDoor_Loop::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	m_dGauge = 0.;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pExitDoor = static_cast<CInter_ExitDoor*>(m_pCamper_GamePlay->m_pTarObject);
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Inter(CUI_Play_Gauge_Inter::EInter::EXIT, true);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_ExitDoor"), m_pCamper_GamePlay->m_iTarObjID);
		m_pExitDoor = static_cast<CInter_ExitDoor*>(m_pCamper_GamePlay->m_pTarObject);
	}
	m_pAudio = dynamic_cast<CAudio*>(m_pCamper_GamePlay->Get_Component(TEXT("Audio")));
	CEffect_Free3* pEffectElec = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_EscapeLeverElectricity"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Effect_EscapeLeverElectricity")));
	CEffect_Free3* pEffectSpark = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_EscapeLeverSpark"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Effect_EscapeLeverSpark")));
	CTransform* pTransform1 = dynamic_cast<CTransform*>(pEffectElec->Get_Component(TEXT("Transform")));
	CTransform* pTransform2 = dynamic_cast<CTransform*>(pEffectSpark->Get_Component(TEXT("Transform")));
	pTransform1->Set_Row(CTransform::ERow::POS, XMVectorSet(224.695f, 1.358f, 173.360f, 0.f));
	pTransform1->Rotation(pTransform1->Get_Row(CTransform::ERow::UP), XMConvertToRadians(90.f));
	pTransform2->Set_Row(CTransform::ERow::POS, XMVectorSet(224.695f, 1.358f, 173.360f, 0.f));
	m_pAudio->Play_Sound_Rand(TEXT("sfx_exitdoor_spark_short"), 4, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

	CLight::SDesc sDesc;
	ZeroMemory(&sDesc, sizeof(CLight::SDesc));
	sDesc.eType = CLight::EType::POINT;
	sDesc.fRange = 1.f;
	sDesc.vDiffuse = { 1.f,1.f,1.f,1.f };
	sDesc.vAmbient = { 1.f,1.f,1.f,1.f };
	sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
	CLight_Generator* pLight1 = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
	pLight1->Set_LightOnPos(XMVectorSet(224.650f, 1.428f, 177.117f, 0.f), sDesc);
	CLight_Generator* pLight2 = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
	pLight2->Set_LightOnPos(XMVectorSet(224.650f, 1.428f, 176.917f, 0.f), sDesc);
	return S_OK;
}

const _ubyte  CFS_Camper_ExitDoor_Loop::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (m_pCamper_GamePlay->m_bControl) {

		m_dGauge += dTimeDelta;
		if (m_dGauge >= 1.) {
			m_pExitDoor->Add_Gauge(5.f);
			m_dGauge = 0.;
		}

		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			m_pExitDoor->Set_State(CInter_ExitDoor::ESTATE::CLOSED, true);

			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}
		else if (CInter_ExitDoor::ESTATE::OPENING == m_pExitDoor->Get_State()) {
			m_pCamper_GamePlay->Set_TarObject(nullptr);

			//dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(false, static_cast<_int>(CUI_Play_Icon_Hud::ECamper::ESCAPE));
			//dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::EXIT, false, false);

			//hwang - score
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::OBJECTIVE_UNLOCKING));
				pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::OBJECTIVE), 1250);
			}
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}
	}
	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);

	return CFM_Camper::EState::END;
}
