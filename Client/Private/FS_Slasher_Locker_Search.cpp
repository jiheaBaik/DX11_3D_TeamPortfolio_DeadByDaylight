#include "pch.h"
#include "FS_Slasher_Locker_Search.h"
#include "Slasher_Huntress.h"
#include "Inter_Locker.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Locker_Search::CFS_Slasher_Locker_Search(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Locker_Search::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Locker_Search::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Locker_Search::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(41);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(41);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pSlasher_Huntress->m_pTarObject);
		// set ui
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::FIND));
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_Locker*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Locker"), m_pSlasher_Huntress->m_iTarObjID));
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pSlasher_Huntress->m_pTarObject);
	}

	m_pLockerTransform = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform")));
	m_pLockerTransform_Slasher = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform_Slasher")));

	return S_OK;
}

const _ubyte  CFS_Slasher_Locker_Search::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f + XMVector3TransformCoord(m_pLockerTransform_Slasher->Get_Row(CTransform::ERow::POS), m_pLockerTransform->Get_Matrix()) * 0.2f);

		XMMATRIX LockerMatrix = m_pLockerTransform_Slasher->Get_Matrix() * m_pLockerTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(LockerMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");
			m_pSlasher_Huntress->m_eCollisionState = CSlasher_Huntress::ECollisionState::NONE;
			m_pLocker->Set_State(CInter_Locker::ESTATE::IDLE);
			// set ui
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(true, CUI_Play_Icon_Hud::EDiv::NONE, true);

			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS) * 0.8f + XMVector3TransformCoord(m_pLockerTransform_Slasher->Get_Row(CTransform::ERow::POS), m_pLockerTransform->Get_Matrix()) * 0.2f);

		XMMATRIX LockerMatrix = m_pLockerTransform_Slasher->Get_Matrix() * m_pLockerTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(LockerMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
