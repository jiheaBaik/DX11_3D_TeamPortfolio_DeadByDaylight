#include "pch.h"
#include "FS_Camper_Closet_Idle.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Icon_Hud.h"

CFS_Camper_Closet_Idle::CFS_Camper_Closet_Idle(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Closet_Idle::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Closet_Idle::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName, 0);
	//m_pCamper_GamePlay->m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f), 1.f);
	m_pCamper_GamePlay->m_pModel->Update_Anim(0, true);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		m_pLocker = static_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
		// set ui
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ECamper::ESCAPE), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::CENTER);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Locker"), m_pCamper_GamePlay->m_iTarObjID);
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}

	m_pLockerTarnsform = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform")));
	m_pLockerTarnsform2 = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("ColTransform_Grab")));

	_vector vRight = m_pLockerTarnsform->Get_Row(CTransform::ERow::RIGHT);
	_vector vLook = m_pLockerTarnsform->Get_Row(CTransform::ERow::LOOK);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, -vRight);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK,-vLook);


	/*XMMATRIX LockerMatrix = m_pLockerTarnsform2->Get_Matrix() * m_pLockerTarnsform->Get_Matrix();
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, LockerMatrix.r[3]);*/

	return S_OK;
}

const _ubyte  CFS_Camper_Closet_Idle::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		if (CInter_Locker::ESTATE::PEEKING_LOCKE == m_pLocker->Get_State()) {
			return CFM_Camper::EState::CLOSSET_HIDE2;
		}
		/*if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_3)) {
			m_pLocker->Set_State(CInter_Locker::ESTATE::PEEKING_LOCKE);
			return CFM_Camper::EState::Closset_Hide2;
		}*/

		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));
		if (nullptr != pSlasher) {
			CCollider* pSlasherCollider = dynamic_cast<CCollider*>(pSlasher->Get_Component(TEXT("Collider")));
			CCollider* pLockCollider = nullptr;
			if (nullptr != m_pLocker) {

				pLockCollider = dynamic_cast<CCollider*>(m_pLocker->Get_Component(TEXT("Collider2")));
				if (pLockCollider->Is_Collision(pSlasherCollider)) {
					CInven* pInven = CInven::Get_Instance();
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_1) ||
						true == pInven->Have_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::LOCKER))) {
						CInter_Locker* pColObject = static_cast<CInter_Locker*>(m_pCamper_GamePlay->Get_ColObject());
						pColObject->Set_State(CInter_Locker::ESTATE::FULLSWING);

						pSlasher->Get_FM_Slasher()->Set_State(CFM_Slasher::EState::STUN_LOCKER);
						pSlasher->Get_FM_Slasher()->Update_Network("Set_State");

						return CFM_Camper::EState::CLOSSET_FULLSWING;
					}
				}
			}
		}



		if (pGame_Instance->Get_Input_KeyboardKey(DIK_SPACE)) {
			m_pLocker->Set_State(CInter_Locker::ESTATE::COMEOUT_CAMPER);
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false);
			return CFM_Camper::EState::CLOSSET_UNHIDE;

		}

		m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, true);
	}
	return CFM_Camper::EState::END;
}
