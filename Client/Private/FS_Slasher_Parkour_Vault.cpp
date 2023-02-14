#include "pch.h"
#include "FS_Slasher_Parkour_Vault.h"
#include "Slasher_Huntress.h"
#include "Inter_Parkour_Vault.h"
#include "UI_Play_Icon_Hud.h"

CFS_Slasher_Pakour_Vault::CFS_Slasher_Pakour_Vault(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Pakour_Vault::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Pakour_Vault::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Pakour_Vault::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher) {
		m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);
	}
	else {
		m_pSlasher_Huntress->m_pModel->Set_Anim(46);
		m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(46);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == m_pSlasher_Huntress->m_bControl) {
		m_pParkour_Vault = dynamic_cast<CInter_Parkour_Vault*>(m_pSlasher_Huntress->m_pTarObject);
		m_iParkourDir = m_pSlasher_Huntress->m_iTarObjDir;
		// set ui
		dynamic_cast<CUI_Play_Icon_Hud*>(CGame_Instance::Get_Instance()->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_int>(CUI_Play_Icon_Hud::ESlasher::PARKOUR));

		if (true == m_pSlasher_Huntress->m_bSlasher)
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_killer_bodymove_default"), 10, -1, 0.4f, FMOD_2D);
	}
	else {
		m_pSlasher_Huntress->m_pTarObject = dynamic_cast<CInter_Parkour_Vault*>(pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Parkour_Vault"), m_pSlasher_Huntress->m_iTarObjID));
		m_pParkour_Vault = dynamic_cast<CInter_Parkour_Vault*>(m_pSlasher_Huntress->m_pTarObject);
		m_iParkourDir = m_pSlasher_Huntress->m_iTarObjDir;

		if (true == m_pSlasher_Huntress->m_bSlasher)
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_killer_bodymove_default"), 10, -1, 0.4f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
	}

	m_fRootOffsetMatrix = m_pSlasher_Huntress->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_pParkourTransform = dynamic_cast<CTransform*>(m_pParkour_Vault->Get_Component(TEXT("Transform")));

	enum Dir { FRONT, BACK, END };
	switch (m_iParkourDir) {
	case FRONT:
		m_pParkourTransform2 = dynamic_cast<CTransform*>(m_pParkour_Vault->Get_Component(TEXT("Transform_F")));
		break;
	case BACK:
		m_pParkourTransform2 = dynamic_cast<CTransform*>(m_pParkour_Vault->Get_Component(TEXT("Transform_B")));
		break;
	}

	m_pSlasher_Huntress->m_bFlag = true;
	XMStoreFloat3(&m_fRootPrePos, m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::POS));

	return S_OK;
}

const _ubyte  CFS_Slasher_Pakour_Vault::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(12)) {
			if (false == m_pSlasher_Huntress->m_bSlasher)
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_killer_bodymove_default"), 10, -1, 0.4f, FMOD_2D);
		}

		//lerp
		_vector lerpPos = XMLoadFloat3(&m_fRootPrePos) * 0.8f + XMVector3TransformCoord(m_pParkourTransform2->Get_Row(CTransform::ERow::POS), m_pParkourTransform->Get_Matrix()) * 0.2f;
		XMStoreFloat3(&m_fRootPrePos, lerpPos);

		XMMATRIX ParkourMatrix = m_pParkourTransform2->Get_Matrix() * m_pParkourTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vChestQaut = XMQuaternionRotationMatrix(ParkourMatrix);
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
			// set ui
			//dynamic_cast<CUI_Play_Icon_Hud*>(CGame_Instance::Get_Instance()->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(true);

			m_pSlasher_Huntress->m_eCollisionState = CSlasher_Huntress::ECollisionState::NONE;
			return CFM_Slasher::EState::IDLE_SLASH;
		}
	}
	else {
		if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(12)) {
			if (false == m_pSlasher_Huntress->m_bSlasher)
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_killer_bodymove_default"), 10, -1, 0.4f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
		}
		//lerp
		_vector lerpPos = XMLoadFloat3(&m_fRootPrePos) * 0.8f + XMVector3TransformCoord(m_pParkourTransform2->Get_Row(CTransform::ERow::POS), m_pParkourTransform->Get_Matrix()) * 0.2f;
		XMStoreFloat3(&m_fRootPrePos, lerpPos);

		XMMATRIX ParkourMatrix = m_pParkourTransform2->Get_Matrix() * m_pParkourTransform->Get_Matrix();
		_vector vQuat = XMQuaternionRotationMatrix(m_pSlasher_Huntress->m_pTransform->Get_Matrix());
		_vector vParkourQaut = XMQuaternionRotationMatrix(ParkourMatrix);
		_vector vResultQuat = XMQuaternionSlerp(vQuat, vParkourQaut, 0.2f);
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
