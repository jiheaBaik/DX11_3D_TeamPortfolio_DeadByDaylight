#include "pch.h"
#include "FS_Slasher_Stun_Drop_In.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Stun_Drop_In::CFS_Slasher_Stun_Drop_In(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Stun_Drop_In::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Stun_Drop_In::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Stun_Drop_In::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(42);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(42);
	}

	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, -1);

		m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pallet_killer_hit"), 3, -1, 0.3f, FMOD_2D);
		if (true == m_pSlasher_Huntress->m_bSlasher)
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_hurt"), 11, -1, 0.3f, FMOD_2D);
		else
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_grunt_hurt"), 4, -1, 0.3f, FMOD_2D);
	}
	else {
		m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pallet_killer_hit"), 3, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		if (true == m_pSlasher_Huntress->m_bSlasher)
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_huntress_hurt"), 11, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		else
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("vo_killer_haiti_grunt_hurt"), 4, -1, 0.3f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
	}

	m_pSlasher_Huntress->m_pTarObject = nullptr;

	return S_OK;
}

const _ubyte  CFS_Slasher_Stun_Drop_In::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			return CFM_Slasher::EState::STUN_DROP_OUT;
		}
	}
	else {
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}