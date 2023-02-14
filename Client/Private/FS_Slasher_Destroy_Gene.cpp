#include "pch.h"
#include "FS_Slasher_Destroy_Gene.h"
#include "Slasher_Huntress.h"
#include "Weapon_Spirit.h"
#include "Inter_Generator.h"
#include "Effect_Free3.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Score.h"

CFS_Slasher_Destroy_Gene::CFS_Slasher_Destroy_Gene(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Destroy_Gene::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Destroy_Gene::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Destroy_Gene::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pGenerator = dynamic_cast<CInter_Generator*>(m_pSlasher_Huntress->m_pTarObject);
		m_iGeneDir = m_pSlasher_Huntress->m_iTarObjDir;

		// set ui
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_int>(CUI_Play_Icon_Hud::ESlasher::SONSANG));
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Inter(CUI_Play_Gauge_Inter::EInter::SONSANG, true);
		//hwang - score
		CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
		if (pScore != nullptr) {
			pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::BRUTALLITY_GENE));
			pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::BRUTALITY), 100);
		}
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_Generator*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Generator"), m_pSlasher_Huntress->m_iTarObjID));
		m_pGenerator = dynamic_cast<CInter_Generator*>(m_pSlasher_Huntress->m_pTarObject);
		m_iGeneDir = m_pSlasher_Huntress->m_iTarObjDir;
	}

	m_pGeneTransform = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform")));
	m_pGeneTransform_C = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_S_C")));

	enum Dir { FRONT, BACK, LEFT, RIGHT, END };
	switch (m_iGeneDir) {
	case FRONT:
		m_pGeneTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_S_F")));
		break;
	case BACK:
		m_pGeneTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_S_B")));
		break;
	case LEFT:
		m_pGeneTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_S_L")));
		break;
	case RIGHT:
		m_pGeneTransform2 = dynamic_cast<CTransform*>(m_pGenerator->Get_Component(TEXT("Transform_S_R")));
		break;
	}

	return S_OK;
}

const _ubyte CFS_Slasher_Destroy_Gene::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		//lerp
		_vector lerpPos = m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f +
			XMVector3TransformCoord(m_pGeneTransform2->Get_Row(CTransform::ERow::POS), m_pGeneTransform->Get_Matrix()) * 0.2f;
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, lerpPos);

		enum Dir { FRONT, BACK, LEFT, RIGHT, END };
		if (m_iGeneDir == FRONT || m_iGeneDir == BACK) {
			XMMATRIX GeneMatrix = m_pGeneTransform2->Get_Matrix() * m_pGeneTransform->Get_Matrix();
			_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
			_vector vChestQaut = XMQuaternionRotationMatrix(GeneMatrix);
			_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
			_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		}
		else
			m_pSlasher_Huntress->m_pTransform->LookAt(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f + m_pGeneTransform->Get_Row(CTransform::ERow::POS) * 0.2f);

		_double dGeneGauge = 0.;
		dGeneGauge = m_pSlasher_Huntress->m_pModel->Get_Anim_TimeAcc() / m_pSlasher_Huntress->m_pModel->Get_Anim_Duration() * 100.;
		// set gauge
		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
		pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::SONSANG, static_cast<_float>(dGeneGauge));

		if (true == m_pSlasher_Huntress->m_pModel->Get_Anim_KeyTimeAcc(28))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_kick"), 2, -1, 0.2f, FMOD_3D, m_pGeneTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		if (true == m_pSlasher_Huntress->m_pModel->Get_Anim_KeyTimeAcc(50)) {
			//m_pGenerator->Set_State(CInter_Generator::EState::CRASH);
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_kick"), 2, -1, 0.2f, FMOD_3D, m_pGeneTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_explode"), 2, -1, 0.2f, FMOD_3D, m_pGeneTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		}

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(72)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_2D);
			}
		}

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {

			CEffect_Free3* pEffectCrash = dynamic_cast<CEffect_Free3*>(pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorCrashSpark"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorCrashSpark")));
			CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffectCrash->Get_Component(TEXT("Transform")));
			pEffectTransform->Set_Row(CTransform::ERow::POS, m_pGeneTransform->Get_Row(CTransform::ERow::POS));

			_float Minus_Gauge = -25.f * m_pSlasher_Huntress->m_fKickGenerator;	//hwang
			m_pGenerator->Add_Gauge(Minus_Gauge);

			m_pSlasher_Huntress->m_eCollisionState = CSlasher_Huntress::ECollisionState::NONE;

			dGeneGauge = 100.;
			// set ui
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(true, CUI_Play_Icon_Hud::EDiv::NONE, true);
			pGauge->End_Gauge(CUI_Play_Gauge_Inter::EInter::SONSANG);

			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		//lerp
		_vector lerpPos = m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f +
			XMVector3TransformCoord(m_pGeneTransform2->Get_Row(CTransform::ERow::POS), m_pGeneTransform->Get_Matrix()) * 0.2f;
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, lerpPos);

		enum Dir { FRONT, BACK, LEFT, RIGHT, END };
		if (m_iGeneDir == FRONT || m_iGeneDir == BACK) {
			XMMATRIX GeneMatrix = m_pGeneTransform2->Get_Matrix() * m_pGeneTransform->Get_Matrix();
			_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
			_vector vChestQaut = XMQuaternionRotationMatrix(GeneMatrix);
			_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
			_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		}
		else
			m_pSlasher_Huntress->m_pTransform->LookAt(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f + m_pGeneTransform->Get_Row(CTransform::ERow::POS) * 0.2f);

		if (true == m_pSlasher_Huntress->m_pModel->Get_Anim_KeyTimeAcc(28))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_kick"), 2, -1, 0.2f, FMOD_3D, m_pGeneTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		if (true == m_pSlasher_Huntress->m_pModel->Get_Anim_KeyTimeAcc(50)) {
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_kick"), 2, -1, 0.2f, FMOD_3D, m_pGeneTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_generator_explode"), 2, -1, 0.2f, FMOD_3D, m_pGeneTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		}

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(72)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
		}

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
