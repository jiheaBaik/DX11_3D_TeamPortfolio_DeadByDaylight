#include "pch.h"
#include "FS_Slasher_Mori.h"
#include "Slasher_Huntress.h"
#include "Weapon_Spirit.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Mori::CFS_Slasher_Mori(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Mori::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Mori::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Mori::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(26);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(26);
	}

	if (true == m_pSlasher_Huntress->m_bControl) {
		// Camper Pick Mori UI Off
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, -1);
	}

	if (false == m_pSlasher_Huntress->m_bSlasher) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
		pWeapon_Spirit->Render_Off();
	}

	return S_OK;
}

const _ubyte  CFS_Slasher_Mori::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (true == m_pSlasher_Huntress->m_bControl) {
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(60))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 5, -1, 0.3f, FMOD_2D);
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(161))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_swap_blade"), 5, -1, 0.3f, FMOD_2D);
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(233))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 5, -1, 0.3f, FMOD_2D);
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(263))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 3, -1, 0.3f, FMOD_2D);
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(314))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_swap_blade"), 3, -1, 0.3f, FMOD_2D);
		}
		else {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(72)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(178))
				m_pSlasher_Huntress->m_bTrail = true;
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(180)) {
				m_pSlasher_Huntress->m_bTrail = false;
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(221))
				m_pSlasher_Huntress->m_bTrail = true;
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(224)) {
				m_pSlasher_Huntress->m_bTrail = false;
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(243))
				m_pSlasher_Huntress->m_bTrail = true;
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(245)) {
				m_pSlasher_Huntress->m_bTrail = false;
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(263))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_scream_fx"), 3, -1, 0.3f, FMOD_2D);
		}

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {

			// 생존자 상태 바꿔주기

			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");

			m_pSlasher_Huntress->m_pFM->Set_CamMori(true);

			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(60))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(161))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_swap_blade"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(233))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(263))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_weapon_axe_attack"), 3, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(314))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_hatchet_swap_blade"), 3, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		}
		else {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(72)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(178))
				m_pSlasher_Huntress->m_bTrail = true;
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(180)) {
				m_pSlasher_Huntress->m_bTrail = false;
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(221))
				m_pSlasher_Huntress->m_bTrail = true;
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(224)) {
				m_pSlasher_Huntress->m_bTrail = false;
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(243))
				m_pSlasher_Huntress->m_bTrail = true;
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(245)) {
				m_pSlasher_Huntress->m_bTrail = false;
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_attack"), 5, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(263))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_scream_fx"), 3, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, { 0.f, 0.f, 0.f }, { 1.f, 5.f });
		}

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
