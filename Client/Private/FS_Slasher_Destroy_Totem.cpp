#include "pch.h"
#include "FS_Slasher_Destroy_Totem.h"
#include "Slasher_Huntress.h"
#include "Weapon_Spirit.h"
#include "Inter_Totem.h"
#include "Camper_GamePlay.h"
#include "UI_Play_Panel_Second.h"

CFS_Slasher_Destroy_Totem::CFS_Slasher_Destroy_Totem(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Destroy_Totem::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Destroy_Totem::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Destroy_Totem::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pTotem = dynamic_cast<CInter_Totem*>(m_pSlasher_Huntress->m_pTarObject);
		// Totem Destroy UI Off
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::DESTROY));
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_Totem*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Totem"), m_pSlasher_Huntress->m_iTarObjID));
		m_pTotem = dynamic_cast<CInter_Totem*>(m_pSlasher_Huntress->m_pTarObject);
	}

	m_pTotemTransform = dynamic_cast<CTransform*>(m_pTotem->Get_Component(TEXT("Transform")));

	XMStoreFloat4(&m_vCenterPos, m_pTotemTransform->Get_Row(CTransform::ERow::POS));

	return S_OK;
}

const _ubyte  CFS_Slasher_Destroy_Totem::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (true == m_pSlasher_Huntress->m_bControl) {
		/*XMMATRIX TotemMatrix = m_pTotemTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(TotemMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);*/

		if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(24))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_totem_break"), 5, -1, 0.3f, FMOD_3D, m_pTotemTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

		m_pSlasher_Huntress->m_pTransform->LookAt(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f + m_pTotemTransform->Get_Row(CTransform::ERow::POS) * 0.2f);
		
		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(30)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_2D);
			}
		}
		
		_double dTotemGauge = 0.;
		dTotemGauge = m_pSlasher_Huntress->m_pModel->Get_Anim_TimeAcc() / m_pSlasher_Huntress->m_pModel->Get_Anim_Duration() * 100.;
		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
		pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::DESTROY, static_cast<_float>(dTotemGauge));

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			m_pTotem->Set_State(CInter_Totem::ESTATE::BREAK);
			m_pSlasher_Huntress->m_eCollisionState = CSlasher_Huntress::ECollisionState::NONE;

			dTotemGauge = 100.;
			// set ui
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(true, CUI_Play_Icon_Hud::EDiv::NONE, true);
			pGauge->End_Gauge(CUI_Play_Gauge_Inter::EInter::DESTROY);

			//hwang - FindCamper//////////////			
			CInven* pInven = CInven::Get_Instance();
			list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
			if (nullptr != listCamper) {
				for (auto& iter : *listCamper) {
					CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
					if (true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), static_cast<_uint>(CInven::PERK_SLASHER::TOTEM_FIND))) 
						pCamper->Set_FindPerk(15.0, true);
					else
						pCamper->Set_FindPerk(3.0, true);
				}
			}
			//////////////////////
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(true);
			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		XMMATRIX TotemMatrix = m_pTotemTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(TotemMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(24))
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_totem_break"), 5, -1, 0.3f, FMOD_3D, m_pTotemTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		
		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(30)) {
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
