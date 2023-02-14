#include "pch.h"
#include "FS_Camper_ParkourVault_Jump.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "UI_Play_Score.h"

CFS_Camper_ParkourVault_Jump::CFS_Camper_ParkourVault_Jump(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_ParkourVault_Jump::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_ParkourVault_Jump::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName);

	if (m_pCamper_GamePlay->m_bControl) {
		m_pParkour = m_pCamper_GamePlay->m_pTarObject;
		m_iDir = m_pCamper_GamePlay->m_iTarObjDir;
		//hwang - score
		/*CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
		if (m_pCamper_GamePlay->m_pFM->Get_State() == CFM_Camper::EState::ParkourVault_Jump) {
			pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::BOLDNESS_VAULTING));
			pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::BOLDNESS), 100);
		} */
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Parkour_Vault"), m_pCamper_GamePlay->m_iTarObjID);

		if (m_pCamper_GamePlay->m_pTarObject != nullptr)
			m_pParkour = m_pCamper_GamePlay->m_pTarObject;
		else {
			m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Parkour"), m_pCamper_GamePlay->m_iTarObjID);
			m_pParkour = m_pCamper_GamePlay->m_pTarObject;
		}
		m_iDir = m_pCamper_GamePlay->m_iTarObjDir;

	}

	if (0 == m_pCamper_GamePlay->m_iTarObjDir)
		pParkourTransform2 = dynamic_cast<CTransform*>(m_pParkour->Get_Component(TEXT("Transform_F")));
	else
		pParkourTransform2 = dynamic_cast<CTransform*>(m_pParkour->Get_Component(TEXT("Transform_B")));

	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = m_pCamper_GamePlay->m_pTransform->Get_Float3(CTransform::ERow::POS);
	m_pCamper_GamePlay->m_bFlag = true;

	XMStoreFloat3(&m_fRootPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));

	return S_OK;
}

const _ubyte  CFS_Camper_ParkourVault_Jump::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(8)) {
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_windows_hit"), 4, -1, 0.1f, FMOD_2D);
		}
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			//hwang
			CInven* pInven = CInven::Get_Instance();
			if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FAST))) {
				m_pCamper_GamePlay->m_bFast = true;
				m_pCamper_GamePlay->m_bStopFast = true;
			}

			if (!m_pCamper_GamePlay->m_bIsCollider_Foot && XMVectorGetY(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS)) > 1.f)
				m_pCamper_GamePlay->m_pFM->Set_State(CFM_Camper::EState::FALL);

			if (2 == m_pCamper_GamePlay->Get_Hp()) {
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						return CFM_Camper::EState::RUN;
					}
				}
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
					return CFM_Camper::EState::WALK_F;
				}
				else
					return CFM_Camper::EState::IDLE;
			}
			else if (1 == m_pCamper_GamePlay->Get_Hp()) {
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						return CFM_Camper::EState::HIT_RUN;
					}
				}
				if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
					return CFM_Camper::EState::HIT_WALK;
				}
				else
					return CFM_Camper::EState::HIT_IDLE;
			}
		}
		else {
			CTransform* pParkourTransform = dynamic_cast<CTransform*>(m_pParkour->Get_Component(TEXT("Transform")));

			XMMATRIX ParkourMatrix = pParkourTransform2->Get_Matrix() * pParkourTransform->Get_Matrix();
			_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
			_vector vLerpPos = ParkourMatrix.r[3] * 0.2f + XMLoadFloat3(&m_fRootPrePos) * 0.8f;
			XMStoreFloat3(&m_fRootPrePos, vLerpPos);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

			_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
			_vector vPakourQaut = XMQuaternionRotationMatrix(ParkourMatrix);
			_vector vResultQuat = XMQuaternionSlerp(vQuat, vPakourQaut, 0.2f);
			_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);

			_vector vRootPos;
			_float4x4 vSocket;
			CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
			_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
			matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
			vRootPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);

			_float3 fPos;
			XMStoreFloat3(&fPos, vRootPos);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(fPos.x, XMVectorGetY(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS)), fPos.z, 1.f));
		}

	}
	else {
		if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(8)) {
			m_pCamper_GamePlay->m_pAudio->Play_Sound_Rand(TEXT("sfx_windows_hit"), 4, -1, 0.1f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 10.f });
		}
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			if (2 == m_pCamper_GamePlay->Get_Hp()) {
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_StandIdle");
					break;
				}
			}
			else if (1 == m_pCamper_GamePlay->Get_Hp()) {
				switch (m_pCamper_GamePlay->m_eCampKind)
				{
				case CCamper_GamePlay::ECamperKind::CHERYL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_inj_StandIdle");
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_inj_StandIdle");
					break;
				}
			}
		}
		else {

			CTransform* pParkourTransform = dynamic_cast<CTransform*>(m_pParkour->Get_Component(TEXT("Transform")));

			XMMATRIX ParkourMatrix = pParkourTransform2->Get_Matrix() * pParkourTransform->Get_Matrix();
			_vector vPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
			_vector vLerpPos = ParkourMatrix.r[3] * 0.2f + XMLoadFloat3(&m_fRootPrePos) * 0.8f;
			XMStoreFloat3(&m_fRootPrePos, vLerpPos);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);

			_vector vQuat = XMQuaternionRotationMatrix(m_pCamper_GamePlay->m_pTransform->Get_Matrix());
			_vector vPakourQaut = XMQuaternionRotationMatrix(ParkourMatrix);
			_vector vResultQuat = XMQuaternionSlerp(vQuat, vPakourQaut, 0.2f);
			_matrix ResultMatrix = XMMatrixRotationQuaternion(vResultQuat);

			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::RIGHT, ResultMatrix.r[0]);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::UP, ResultMatrix.r[1]);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::LOOK, ResultMatrix.r[2]);
			_uint fDistance = (_uint)fabs(XMVectorGetX(XMVector3Length(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS) - ParkourMatrix.r[3])));

			_vector vRootPos;
			_float4x4 vSocket;
			CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
			_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
			matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
			vRootPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);

			_float3 fPos;
			XMStoreFloat3(&fPos, vRootPos);
			m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(fPos.x, XMVectorGetY(m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS)), fPos.z, 1.f));

		}
	}
	return CFM_Camper::EState::END;
}
