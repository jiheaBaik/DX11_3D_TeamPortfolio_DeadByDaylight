#include "pch.h"
#include "FS_Slasher_Locker_Grab.h"
#include "Slasher_Huntress.h"
#include "Inter_Locker.h"
#include "Camper_GamePlay.h"
#include "UI_Play_Panel_Second.h"

CFS_Slasher_Locker_Grab::CFS_Slasher_Locker_Grab(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Locker_Grab::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Locker_Grab::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Locker_Grab::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(19);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(19);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pSlasher_Huntress->m_pTarObject);
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::FIND));
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_Locker*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Locker"), m_pSlasher_Huntress->m_iTarObjID));
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pSlasher_Huntress->m_pTarObject);
	}

	m_fRootOffsetMatrix = m_pSlasher_Huntress->m_pModel->Get_BoneOffsetMatrix("joint_Char");

	m_pLockerTransform = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform")));
	m_pLockerTransform_Slasher = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform_Slasher")));

	XMStoreFloat3(&m_fRootPrePos, m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS));

	return S_OK;
}

const _ubyte  CFS_Slasher_Locker_Grab::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		//lerp
		_vector lerpPos = XMLoadFloat3(&m_fRootPrePos) * 0.8f + XMVector3TransformCoord(m_pLockerTransform_Slasher->Get_Row(CTransform::ERow::POS), m_pLockerTransform->Get_Matrix()) * 0.2f;
		XMStoreFloat3(&m_fRootPrePos, lerpPos);

		XMMATRIX LockerMatrix = m_pLockerTransform_Slasher->Get_Matrix() * m_pLockerTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(LockerMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		//rootbone
		_vector vPos;
		_float4x4 vSocket;
		CModel* pSlasherModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model")));

		_matrix matrix = m_pSlasher_Huntress->m_pTransform->Get_Matrix();
		matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pSlasherModel->Get_RootBoneMatrix("joint_Char")) * matrix);

		vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, vPos);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		if (ANIM_END == m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta)) {
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"), m_pLocker->Get_CamperID()));
			m_pSlasher_Huntress->m_iTarObjID = m_pSlasher_Huntress->m_pTarObject->Get_Id();
			m_pSlasher_Huntress->Update_Network("Set_TarObjID");

			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::CARRY;
			m_pSlasher_Huntress->Update_Network("Set_Type");
			m_pSlasher_Huntress->m_eCollisionState = CSlasher_Huntress::ECollisionState::NONE;
			//m_pLocker->Set_IsInCamper(false);
			m_pLocker->Set_State(CInter_Locker::ESTATE::IDLE);

			//m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("mu_killer_carrying"), 2, 2, 0.3f, FMOD_2D | FMOD_LOOP_NORMAL);

			//hwang - score
			CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
			if (pScore != nullptr) {
				pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ESlasherScore::HUNTER_CATCH));
				pScore->Plus_SlasherScore(static_cast<_uint>(CUI_Play_Score::ESlasherNum::HUNTER), 600);
			}
			return CFM_Slasher::EState::IDLE_CARRY;
		}
	}
	else {
		//lerp
		_vector lerpPos = XMLoadFloat3(&m_fRootPrePos) * 0.8f + XMVector3TransformCoord(m_pLockerTransform_Slasher->Get_Row(CTransform::ERow::POS), m_pLockerTransform->Get_Matrix()) * 0.2f;
		XMStoreFloat3(&m_fRootPrePos, lerpPos);

		XMMATRIX LockerMatrix = m_pLockerTransform_Slasher->Get_Matrix() * m_pLockerTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(LockerMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vChestQaut, 0.2f);
		_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
		//rootbone
		_vector vPos;
		_float4x4 vSocket;
		CModel* pSlasherModel = dynamic_cast<CModel*>(m_pSlasher_Huntress->Get_Component(TEXT("Model")));

		_matrix matrix = m_pSlasher_Huntress->m_pTransform->Get_Matrix();
		matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pSlasherModel->Get_RootBoneMatrix("joint_Char")) * matrix);

		vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
		m_pSlasher_Huntress->m_pTransform->Set_Row(CTransform::ERow::POS, vPos);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta);
	}

	return CFM_Slasher::EState::END;
}
