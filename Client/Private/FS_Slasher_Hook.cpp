#include "pch.h"
#include "FS_Slasher_Hook.h"
#include "Slasher_Huntress.h"
#include "Inter_Hook.h"
#include "Camper_GamePlay.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Score.h"

CFS_Slasher_Hook::CFS_Slasher_Hook(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Hook::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Hook::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Hook::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(24);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(24);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pHook = dynamic_cast<CInter_Hook*>(m_pSlasher_Huntress->m_pTarObject);
		// Hook Hang UI Off
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::HANG));
		dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Inter(CUI_Play_Gauge_Inter::EInter::HANG, true);
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_Hook*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Hook"), m_pSlasher_Huntress->m_iTarObjID));
		m_pHook = dynamic_cast<CInter_Hook*>(m_pSlasher_Huntress->m_pTarObject);
	}

	m_pHookTransform = dynamic_cast<CTransform*>(m_pHook->Get_Component(TEXT("Transform")));

	CModel* pHookModel = dynamic_cast<CModel*>(m_pHook->Get_Component(TEXT("Model_Hook")));
	_matrix SocketMatrix = pHookModel->Get_BoneMatrix("joint_KillerAttach") * m_pHookTransform->Get_Matrix();

	XMStoreFloat4(&m_vCenterPos, m_pHookTransform->Get_Row(CTransform::ERow::POS));
	XMStoreFloat4(&m_vPos, SocketMatrix.r[3]);

	return S_OK;
}

const _ubyte  CFS_Slasher_Hook::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

		CModel* pHookModel = dynamic_cast<CModel*>(m_pHook->Get_Component(TEXT("Model_Hook")));
		XMMATRIX HookMatrix = pHookModel->Get_BoneMatrix("joint_KillerAttach") * m_pHookTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(HookMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f + XMLoadFloat4(&m_vPos) * 0.2f);

		_double dHookGauge = 0.;
		dHookGauge = m_pSlasher_Huntress->m_pModel->Get_Anim_TimeAcc() / m_pSlasher_Huntress->m_pModel->Get_Anim_Duration() * 100.;
		// set gauge
		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
		pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::HANG, static_cast<_float>(dHookGauge));

		// sound frame test
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(25))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_hook_put_on_chain"), 3, -1, 0.3f, FMOD_3D, m_pHookTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");
			CInven* pInven = CInven::Get_Instance();
			if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::MORI_CAMPER))
				&& m_pSlasher_Huntress->m_iHookCount < 3)
				++m_pSlasher_Huntress->m_iHookCount;

			//hwang/////////////
			Check_Perk(dTimeDelta);
			//////////////////////

			dHookGauge = 100.;

			// set ui
			if (0 < m_pSlasher_Huntress->Get_WeaponCount())
				dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->
				Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1), CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::EDiv::CENTER);
			pGauge->End_Gauge(CUI_Play_Gauge_Inter::EInter::HANG);

			m_pSlasher_Huntress->m_pAudio->Stop_Sound(2);

			//hwang - score
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::SACRIFICE_HOOKED));
				pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::SACRIFICE), 500);
			}
			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		CModel* pHookModel = dynamic_cast<CModel*>(m_pHook->Get_Component(TEXT("Model_Hook")));
		XMMATRIX HookMatrix = pHookModel->Get_BoneMatrix("joint_KillerAttach") * m_pHookTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(HookMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f + XMLoadFloat4(&m_vPos) * 0.2f);

		// sound frame test
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(25))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_hook_put_on_chain"), 3, -1, 0.3f, FMOD_3D, m_pHookTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}

void CFS_Slasher_Hook::Check_Perk(_double dTimeDelta) {
	//hwang - FindCamper Perk//////////////
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CInven* pInven = CInven::Get_Instance();
	if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::FIND_CAMPER))) {
		list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
		if (m_pHook->Get_SlasherPerk() == true) {
			for (auto& iter : *listCamper) {
				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
				if (pCamper->m_pFM->Get_State() != CFM_Camper::EState::HOOKIN ||
					pCamper->m_pFM->Get_State() != CFM_Camper::EState::HOOKOUT ||
					pCamper->m_pFM->Get_State() != CFM_Camper::EState::HOOKIDLE) {
					pCamper->Set_FindPerk(5.0, true);
				}
			}
		}
	}
	//Kick_Generator
	if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::KICK_GENERATOR))) {
		m_pSlasher_Huntress->m_fKickGenerator = 1.25f;
	}
}
