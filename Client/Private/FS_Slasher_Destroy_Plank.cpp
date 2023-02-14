#include "pch.h"
#include "FS_Slasher_Destroy_Plank.h"
#include "Slasher_Huntress.h"
#include "Weapon_Spirit.h"
#include "Inter_PlankAnim.h"
#include "Effect_Free3.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Score.h"

CFS_Slasher_Destroy_Plank::CFS_Slasher_Destroy_Plank(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Destroy_Plank::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Destroy_Plank::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Destroy_Plank::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(14);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(14);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pPlank = dynamic_cast<CInter_PlankAnim*>(m_pSlasher_Huntress->m_pTarObject);
		m_iPlankDir = m_pSlasher_Huntress->m_iTarObjDir;
		// set ui
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::DESTROY));
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_PlankAnim*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_PlankAnim"), m_pSlasher_Huntress->m_iTarObjID));
		m_pPlank = dynamic_cast<CInter_PlankAnim*>(m_pSlasher_Huntress->m_pTarObject);
		m_iPlankDir = m_pSlasher_Huntress->m_iTarObjDir;
	}

	m_pPlankTransform = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform")));

	enum Dir { FRONT, BACK, END };
	switch (m_iPlankDir) {
	case FRONT:
		m_pPlankTransform2 = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform_F")));
		break;
	case BACK:
		m_pPlankTransform2 = dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform_B")));
		break;
	}

	m_bBroken = false;
	return S_OK;
}

const _ubyte CFS_Slasher_Destroy_Plank::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	if (true == m_pSlasher_Huntress->m_bControl) {
		//lerp
		_vector lerpPos = m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f +
			XMVector3TransformCoord(m_pPlankTransform2->Get_Row(CTransform::ERow::POS), m_pPlankTransform->Get_Matrix()) * 0.2f;
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, lerpPos);

		XMMATRIX PlankMatrix = m_pPlankTransform2->Get_Matrix() * m_pPlankTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(PlankMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

		if ((_uint)m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_TimeAcc() >= 55)
			m_pSlasher_Huntress->Get_FM_Slasher()->Update_Network("Plank_Broken");

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_2D);
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(68)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_Off();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Disappear_Bounce"), 4, -1, 0.2f, FMOD_2D);
			}
		}

		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			m_pPlank->Set_State(CInter_PlankAnim::ESTATE::BROKEN);
			//CEffect_Free3* pEffect = dynamic_cast<CEffect_Free3*>(CGame_Instance::Get_Instance()->CreateGet_Object_Clone(TEXT("Effect_PlantBroken"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_PlantBroken")));
			//CTransform* pTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
			//pTransform->Set_Row(CTransform::ERow::POS, dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform")))->Get_Row(CTransform::ERow::POS) + XMVectorSet(0.f,0.7f,0.f,0.f) + dynamic_cast<CTransform*>(m_pPlank->Get_Component(TEXT("Transform")))->Get_Row(CTransform::ERow::LOOK));
			m_pSlasher_Huntress->m_eCollisionState = CSlasher_Huntress::ECollisionState::NONE;
			// set ui
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(true);
			//if (0 < m_pSlasher_Huntress->Get_WeaponCount())
			//	dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(true);

			//hwang - score
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::BRUTALLITY_DESTORY));
				pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::BRUTALITY), 100);
			}

			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		//lerp
		if (false == m_bBroken) {
			_vector lerpPos = m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f +
				XMVector3TransformCoord(m_pPlankTransform2->Get_Row(CTransform::ERow::POS), m_pPlankTransform->Get_Matrix()) * 0.2f;
			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, lerpPos);

			XMMATRIX PlankMatrix = m_pPlankTransform2->Get_Matrix() * m_pPlankTransform->Get_Matrix();
			_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
			_vector vChestQaut = XMQuaternionRotationMatrix(PlankMatrix);
			_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
			_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
			m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		}

		if (false == m_pSlasher_Huntress->m_bSlasher) {
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5)) {
				CWeapon_Spirit* pWeapon_Spirit = dynamic_cast<CWeapon_Spirit*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Weapon_Spirit")));
				pWeapon_Spirit->Render_On();
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("Haiti_WPN_Appear_Bounce"), 3, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
			}
			if (true == m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(68)) {
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

void CFS_Slasher_Destroy_Plank::Set_Broken(const _bool& bBroken) {
	m_bBroken = bBroken;
}