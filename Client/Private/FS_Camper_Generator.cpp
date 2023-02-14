#include "pch.h"
#include "FS_Camper_Generator.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Generator.h"
#include "Effect_Free3.h"
#include "Light_Generator.h"
#include "Light_GeneratorFail.h"
#include "UI_Play_Gauge_Inter.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Panel_Second.h"

CFS_Camper_Generator::CFS_Camper_Generator(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Generator::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Generator::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);
	m_dGauge = 0.;
	if (m_pCamper_GamePlay->m_bControl) {
		m_pGenerator = static_cast<CInter_Generator*>(m_pCamper_GamePlay->m_pTarObject);
		m_iDir = m_pCamper_GamePlay->m_iTarObjDir;
		// set hud
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		_float fTime = static_cast<_float>(100.f - m_pGenerator->Get_Gauge());
		if (m_bFailed) {
			dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_HitCount(5, fTime, CUI_Play_Icon_SkillCheck::ECheck::FAILED, 150);
			m_bFailed = false;
		}
		else {
			dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_HitCount(10, fTime, CUI_Play_Icon_SkillCheck::ECheck::START, 200);
		}
		//m_pGenerator
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Generator"), m_pCamper_GamePlay->m_iTarObjID);
		m_pGenerator = static_cast<CInter_Generator*>(m_pCamper_GamePlay->m_pTarObject);
		m_iDir = m_pCamper_GamePlay->m_iTarObjDir;
	}

	enum Dir { FRONT, BACK, LEFT, RIGHT, END };
	switch (m_iDir)
	{
	case FRONT:
		pGeneratorTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_F")));
		break;
	case BACK:
		pGeneratorTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_B")));
		break;
	case LEFT:
		pGeneratorTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_L")));
		break;
	case RIGHT:
		pGeneratorTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_R")));
		break;
	}

	m_pGenerator->Set_Exist(m_iDir, true);
	m_pGenerator->Update_Network("Set_PlaceID");
	m_pAudio = dynamic_cast<CAudio*>(m_pCamper_GamePlay->Get_Component(TEXT("Audio")));

	return S_OK;
}

const _ubyte  CFS_Camper_Generator::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	CTransform* pGeneratorTransform = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform")));

	XMMATRIX GeneratorMatrix = pGeneratorTransform2->Get_Matrix() * pGeneratorTransform->Get_Matrix();
	_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLerpPos = GeneratorMatrix.r[3] * 0.2f + vPos * 0.8f;
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

	_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
	_vector vGeneratorQaut = XMQuaternionRotationMatrix(GeneratorMatrix);
	_vector vResultQuat = XMQuaternionSlerp(vQuat, vGeneratorQaut, 0.2f);
	_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

	if (m_pCamper_GamePlay->m_bControl) {
		m_dGauge += dTimeDelta;
		if (m_dGauge > 1.f) {
			//m_pGenerator->Add_Gauge(100.f);
			m_pGenerator->Add_Gauge(1.f);
			m_dGauge = 0.;
		}

		_bool bFailed = (CUI_Play_Icon_SkillCheck::EState::FAIL == dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Get_State());

		if (bFailed || pGame_Instance->Get_Input_KeyboardKeyDown(DIK_1) || CInter_Generator::EState::LOCK_IN == m_pGenerator->Get_State()) {//Faile
			m_pCamper_GamePlay->m_iTarObjDir = m_pCamper_GamePlay->m_iColObjDir;
			m_pCamper_GamePlay->Update_Network("Set_TarObjDir");
			CEffect_Free3* pEffect1 = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorFailSpark"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorFailSpark")));
			CEffect_Free3* pEffect2 = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorFailSmoke"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorFailSmoke")));

			CTransform* pTransform0 = dynamic_cast<CTransform*>(pEffect1->Get_Component(TEXT("Transform")));
			CTransform* pTransform1 = dynamic_cast<CTransform*>(pEffect2->Get_Component(TEXT("Transform")));
			CTransform* pTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform")));

			pTransform0->Set_Matrix(pTransform2->Get_Matrix());
			pTransform1->Set_Matrix(pTransform2->Get_Matrix());

			CLight::SDesc sDesc;
			ZeroMemory(&sDesc, sizeof(CLight::SDesc));
			sDesc.eType = CLight::EType::POINT;
			sDesc.fRange = 1.5f;
			sDesc.vDiffuse = { 4.f,4.f,4.f,1.f };
			sDesc.vAmbient = { 4.f,4.f,4.f,1.f };
			sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
			CLight_GeneratorFail* pLight = dynamic_cast<CLight_GeneratorFail*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_GeneratorFail"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_GeneratorFail"), &sDesc));
			vPos += XMVectorSet(0.f, 0.8f, 0.f, 0.f);
			pLight->Set_LightOnPos(vPos, sDesc);

			// reset gauge ? 
			m_bFailed = true;

			enum Dir { FRONT, BACK, LEFT, RIGHT, END };
			switch (m_pCamper_GamePlay->m_iTarObjDir) {
			case FRONT:
				return CFM_Camper::EState::GENERATOR_IDLEF_FAILE;
			case BACK:
				return CFM_Camper::EState::GENERATOR_IDLEB_FAILE;
			case LEFT:
				return CFM_Camper::EState::GENERATOR_IDLEL_FAILE;
			case RIGHT:
				return CFM_Camper::EState::GENERATOR_IDLER_FAILE;
			}
		}
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L) || CInter_Generator::EState::ON == m_pGenerator->Get_State()) {

			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ECamper::CANCLE));
			dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_Active(false);

			if (CInter_Generator::EState::ON == m_pGenerator->Get_State()) {
				dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->End_Gauge(CUI_Play_Gauge_Inter::EInter::GEN);

				//hwang - perk
				CInven* pInven = CInven::Get_Instance();
				if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FIND_SLASHER))) {
					CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
					if (pSlasher != nullptr) {
						pSlasher->Set_FindPerk(5.0, true);
					}
				}

				//hwang - score
				CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
				if (pScore != nullptr) {
					pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::OBJECTIVE_REPAIRS));
					pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::OBJECTIVE), 1250);
				}
			}

			m_pGenerator->Set_Exist(m_iDir, false);
			m_pGenerator->Update_Network("Set_PlaceID");
			m_pCamper_GamePlay->Set_TarObject(nullptr);
			//// hud off
			//dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ECamper::CANCLE));
			//dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_Active(false);
			//if (CInter_Generator::EState::ON == m_pGenerator->Get_State()) {
			//	// hud off
			//	dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->End_Gauge(CUI_Play_Gauge_Inter::EInter::GEN);
			//}			

			

			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}
	}

	// effect particle - 221110 jsy
	m_dEffectTime += dTimeDelta;
	if (m_dEffectTime > static_cast<_double>(rand() % 5) + 2.) {
		//m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_repair_spark"), 12, -1, 1.f, FMOD_2D);
		m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_repair_spark"), 12, -1, 0.3f, FMOD_3D, dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform"))), _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		enum Dir { FRONT, BACK, LEFT, RIGHT, END };
		if (m_iDir == Dir::FRONT) {
			CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorProgressSparkF"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorProgressSparkF")));
			CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
			pEffectTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
			_vector vPos = pGeneratorTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(_float(rand() - rand()) * 0.1f / RAND_MAX, _float(rand() - rand()) * 0.1f / RAND_MAX, 0.f, 0.f);
			pEffectTransform->Set_Row(CTransform::ERow::POS, vPos);
			CLight::SDesc sDesc;
			ZeroMemory(&sDesc, sizeof(CLight::SDesc));
			sDesc.eType = CLight::EType::POINT;
			sDesc.fRange = 0.8f;
			sDesc.vDiffuse = { 1.245f,0.f,1.7f,2.f };
			sDesc.vAmbient = { 1.245f,0.f,1.7f,2.f };
			sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
			CLight_Generator* pLight = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
			vPos += XMVectorSet(0.f, 0.8f, 0.f, 0.f) - pGeneratorTransform->Get_Row(CTransform::ERow::LOOK) * 0.7f;
			pLight->Set_LightOnPos(vPos, sDesc);
		}
		else if (m_iDir == Dir::BACK) {
			CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorProgressSparkF"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorProgressSparkF")));
			CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
			_vector vPos = pGeneratorTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(_float(rand() - rand()) * 0.1f / RAND_MAX, _float(rand() - rand()) * 0.1f / RAND_MAX, 0.f, 0.f);
			pEffectTransform->Set_Row(CTransform::ERow::POS, vPos);
			CLight::SDesc sDesc;
			ZeroMemory(&sDesc, sizeof(CLight::SDesc));
			sDesc.eType = CLight::EType::POINT;
			sDesc.fRange = 0.8f;
			sDesc.vDiffuse = { 1.245f,0.f,1.7f,2.f };
			sDesc.vAmbient = { 1.245f,0.f,1.7f,2.f };
			sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
			CLight_Generator* pLight = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
			vPos += XMVectorSet(0.f, 0.8f, 0.f, 0.f) + pGeneratorTransform->Get_Row(CTransform::ERow::LOOK) * 0.5f;
			pLight->Set_LightOnPos(vPos, sDesc);
		}
		else if (m_iDir == Dir::RIGHT) {
			if (rand() % 10 < 5) {
				CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorProgressSpark1"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorProgressSpark1")));
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
				_vector vPos = pGeneratorTransform->Get_Row(CTransform::ERow::POS) + pGeneratorTransform->Get_Row(CTransform::ERow::RIGHT) * 0.2f + XMVectorSet(0.f, _float(rand() - rand()) * 0.1f / RAND_MAX, _float(rand() - rand()) * 0.1f / RAND_MAX, 0.f);
				pEffectTransform->Set_Row(CTransform::ERow::POS, vPos);
				CLight::SDesc sDesc;
				ZeroMemory(&sDesc, sizeof(CLight::SDesc));
				sDesc.eType = CLight::EType::POINT;
				sDesc.fRange = 0.8f;
				sDesc.vDiffuse = { 1.245f,0.f,1.7f,2.f };
				sDesc.vAmbient = { 1.245f,0.f,1.7f,2.f };
				sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
				CLight_Generator* pLight = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
				vPos += XMVectorSet(0.f, 0.8f, 0.f, 0.f) + pGeneratorTransform->Get_Row(CTransform::ERow::RIGHT) * 0.1f;
				pLight->Set_LightOnPos(vPos, sDesc);
			}
			else {
				CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorProgressSpark2"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorProgressSpark2")));
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
				_vector vPos = pGeneratorTransform->Get_Row(CTransform::ERow::POS) + pGeneratorTransform->Get_Row(CTransform::ERow::RIGHT) * 0.2f + XMVectorSet(0.f, _float(rand() - rand()) * 0.1f / RAND_MAX, _float(rand() - rand()) * 0.1f / RAND_MAX, 0.f);
				pEffectTransform->Set_Row(CTransform::ERow::POS, vPos);
				CLight::SDesc sDesc;
				ZeroMemory(&sDesc, sizeof(CLight::SDesc));
				sDesc.eType = CLight::EType::POINT;
				sDesc.fRange = 0.8f;
				sDesc.vDiffuse = { 1.245f,0.f,1.7f,2.f };
				sDesc.vAmbient = { 1.245f,0.f,1.7f,2.f };
				sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
				CLight_Generator* pLight = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
				vPos += XMVectorSet(0.f, 0.8f, 0.f, 0.f) + pGeneratorTransform->Get_Row(CTransform::ERow::RIGHT) * 0.1f;
				pLight->Set_LightOnPos(vPos, sDesc);
			}
		}
		else if (m_iDir == Dir::LEFT) {
			if (rand() % 10 < 5) {
				CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorProgressSpark1"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorProgressSpark1")));
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
				pEffectTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
				_vector vPos = pGeneratorTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, _float(rand() - rand()) * 0.1f / RAND_MAX, _float(rand() - rand()) * 0.1f / RAND_MAX, 0.f);
				pEffectTransform->Set_Row(CTransform::ERow::POS, vPos);
				CLight::SDesc sDesc;
				ZeroMemory(&sDesc, sizeof(CLight::SDesc));
				sDesc.eType = CLight::EType::POINT;
				sDesc.fRange = 0.8f;
				sDesc.vDiffuse = { 1.245f,0.f,1.7f,2.f };
				sDesc.vAmbient = { 1.245f,0.f,1.7f,2.f };
				sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
				CLight_Generator* pLight = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
				vPos += XMVectorSet(0.f, 0.8f, 0.f, 0.f) - pGeneratorTransform->Get_Row(CTransform::ERow::RIGHT) * 0.1f;
				pLight->Set_LightOnPos(vPos, sDesc);
			}
			else {
				CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorProgressSpark2"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorProgressSpark2")));
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
				pEffectTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
				_vector vPos = pGeneratorTransform->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f, _float(rand() - rand()) * 0.1f / RAND_MAX, _float(rand() - rand()) * 0.1f / RAND_MAX, 0.f);
				pEffectTransform->Set_Row(CTransform::ERow::POS, vPos);
				CLight::SDesc sDesc;
				ZeroMemory(&sDesc, sizeof(CLight::SDesc));
				sDesc.eType = CLight::EType::POINT;
				sDesc.fRange = 0.8f;
				sDesc.vDiffuse = { 1.245f,0.f,1.7f,2.f };
				sDesc.vAmbient = { 1.245f,0.f,1.7f,2.f };
				sDesc.vSpecular = { 0.f,0.f,0.f,0.f };
				CLight_Generator* pLight = dynamic_cast<CLight_Generator*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Light_Generator"), static_cast<_int>(EScene::GAMEPLAY), TEXT("Light_Generator"), &sDesc));
				vPos += XMVectorSet(0.f, 0.8f, 0.f, 0.f) - pGeneratorTransform->Get_Row(CTransform::ERow::RIGHT) * 0.1f;
				pLight->Set_LightOnPos(vPos, sDesc);
			}
		}
		m_dEffectTime = 0.;
	}

	m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	return CFM_Camper::EState::END;
}
