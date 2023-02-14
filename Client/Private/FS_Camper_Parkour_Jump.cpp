#include "pch.h"
#include "FS_Camper_Parkour_Jump.h"
#include "Camper_Cheryl.h"
#include "FM_Camper.h"
#include "Inter_Parkour.h"
#include "UI_Play_Icon_Hud.h"

CFS_Camper_Parkour_Jump::CFS_Camper_Parkour_Jump(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_Cheryl* const& pCamper_Cheryl)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_Cheryl = pCamper_Cheryl;
}

void CFS_Camper_Parkour_Jump::Delete(void) {
	__super::Delete();
	m_pCamper_Cheryl = nullptr;
}

const HRESULT CFS_Camper_Parkour_Jump::Init(_char* ModelName) {
	m_pCamper_Cheryl->m_pModel->Set_Anim(ModelName);
	m_pCamper_Cheryl->m_bFlag = true;
	if (m_pCamper_Cheryl->m_bControl) {
		m_pParkour = static_cast<CInter_Parkour*>(m_pCamper_Cheryl->m_pTarObject);

		// set ui
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_AllHuds(false, static_cast<_uint>(CUI_Play_Icon_Hud::ECamper::PARKOUR));
	}
	else
		m_pParkour = static_cast<CInter_Parkour*>(m_pCamper_Cheryl->m_pTarObject);

	m_fRootOffsetMatrix = m_pCamper_Cheryl->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	m_fRootPrePos = m_pCamper_Cheryl->m_pTransform->Get_Float3(CTransform::ERow::POS);

	return S_OK;
}

const _ubyte  CFS_Camper_Parkour_Jump::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_Cheryl->m_bControl)
	{
		if (30.f <= m_pCamper_Cheryl->m_pModel->Get_Anim_TimeAcc()) {
			m_pCamper_Cheryl->m_bFlag = false;
		}
		if (28.f <= m_pCamper_Cheryl->m_pModel->Get_Anim_TimeAcc()) {
			if (31.f <= m_pCamper_Cheryl->m_pModel->Get_Anim_TimeAcc()) {
				if (!m_pCamper_Cheryl->m_bIsCollider_Foot)
					return CFM_Camper::EState::Fall;
				
				//hwang
				CInven* pInven = CInven::Get_Instance();
				if (true == pInven->Use_Perk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), static_cast<_uint>(CInven::PERK_CAMPER::FAST))) 
					m_pCamper_Cheryl->m_fFastPerk = 5.f;
				
				if (2 == m_pCamper_Cheryl->Get_Hp()) {
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {

						if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
							return CFM_Camper::EState::Run;

						}
					}
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						return CFM_Camper::EState::Walk_F;
					}
					else
						return CFM_Camper::EState::Idle;
				}
				else if (1 == m_pCamper_Cheryl->Get_Hp()) {


					if (pGame_Instance->Get_Input_KeyboardKey(DIK_LSHIFT)) {
						if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
							return CFM_Camper::EState::Hit_Run;
						}
					}
					if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
						m_pCamper_Cheryl->m_pFM->Set_State(CFM_Camper::EState::Hit_Walk);
					}
					else
						return CFM_Camper::EState::Hit_Idle;
				}
			}
		}
		else {
			_vector vPos;
			_float4x4 vSocket;
			CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_Cheryl->Get_Component(TEXT("Model")));
			_matrix matrix = m_pCamper_Cheryl->m_pTransform->Get_Matrix();
			matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
			vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);

			_float3 fPos;
			XMStoreFloat3(&fPos, vPos);
			m_pCamper_Cheryl->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(fPos.x, XMVectorGetY(m_pCamper_Cheryl->m_pTransform->Get_Row(CTransform::ERow::POS)), fPos.z, 1.f));
		}
	}
	else {
		if (28.f <= m_pCamper_Cheryl->m_pModel->Get_Anim_TimeAcc()) {
			m_pCamper_Cheryl->m_bFlag = false;
			if (31.f == m_pCamper_Cheryl->m_pModel->Get_Anim_TimeAcc()) {
				if (m_fDist >= 3) {
					m_fDist = 0.f;
					m_pCamper_Cheryl->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_Fall");
				}
				else {
					if (2 == m_pCamper_Cheryl->Get_Hp())
						m_pCamper_Cheryl->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_StandIdle");
					else if (1 == m_pCamper_Cheryl->Get_Hp())
						m_pCamper_Cheryl->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_InjuredIdle01_Treat");
				}

			}
		}
		else {
			_vector vPos;
			_float4x4 vSocket;
			CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_Cheryl->Get_Component(TEXT("Model")));
			_matrix matrix = m_pCamper_Cheryl->m_pTransform->Get_Matrix();
			matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
			vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);

			_float3 fPos;
			XMStoreFloat3(&fPos, vPos);
			m_pCamper_Cheryl->m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(fPos.x, XMVectorGetY(m_pCamper_Cheryl->m_pTransform->Get_Row(CTransform::ERow::POS)), fPos.z, 1.f));
		}
	}

	m_pCamper_Cheryl->m_pModel->Update_Anim(dTimeDelta, false);
	return CFM_Camper::EState::End;
}
