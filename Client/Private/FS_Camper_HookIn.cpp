#include "pch.h"
#include "FS_Camper_HookIn.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Camper_GamePlay.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Icon_SkillCheck.h"
#include "Effect_Blood_Free.h"

CFS_Camper_HookIn::CFS_Camper_HookIn(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_HookIn::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_HookIn::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);
	m_pCamper_GamePlay->m_bFlag = true;

	//m_pCamper_GamePlay->m_fSpiderMaxTime -= 30.f;
	//m_pCamper_GamePlay->m_fSpiderTime = m_pCamper_GamePlay->m_fSpiderMaxTime;
	if (m_pCamper_GamePlay->m_fSpiderRound == 2) {
		m_pCamper_GamePlay->m_fSpiderTime -= m_pCamper_GamePlay->m_fSpiderTime * 0.5f;//50프로 깎기
		m_pCamper_GamePlay->Update_Network("Set_SpiderTime");
		m_pCamper_GamePlay->m_fSpiderRound += 1;
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pHuntress = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Slasher_Huntress")));


	//테스트 아닐 시 주석하기**************************************************
	//if (m_pCamper_GamePlay->m_bControl)
	//	m_pHook = static_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	//m_pHook->Set_State(CInter_Hook::ESTATE::HANGING);
	//**********************************************************


	////테스트 아닐 시 주석 풀어야함 **************************************************
	if (true == m_pCamper_GamePlay->m_bControl) {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Hook"), m_pCamper_GamePlay->m_iTarObjID);
		m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
		dynamic_cast<CUI_Play_Icon_SkillCheck*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_SkillCheck"))->Set_Active(false);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Hook"), m_pCamper_GamePlay->m_iTarObjID);
		m_pHook = dynamic_cast<CInter_Hook*>(m_pCamper_GamePlay->m_pTarObject);
	}
	////**********************************************************


	m_pHookTransform = dynamic_cast<CTransform*>(m_pHook->Get_Component(TEXT("Transform")));
	m_pHookModel = dynamic_cast<CModel*>(m_pHook->Get_Component(TEXT("Model_Hook")));

	m_fRootOffsetMatrix = m_pCamper_GamePlay->m_pModel->Get_BoneOffsetMatrix("joint_Char");
	XMStoreFloat3(&m_fRootPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));

	//hwang
	if (m_pCamper_GamePlay->m_bControl == true) {
		list<CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
		if (nullptr != listCamper) {
			for (auto& iter : *listCamper) {
				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
				if (pCamper != m_pCamper_GamePlay)
					pCamper->Set_FindPerk(999.0, true);
			}
		}
	}

	m_pAudio = dynamic_cast<CAudio*>(m_pCamper_GamePlay->Get_Component(TEXT("Audio")));

	return S_OK;
}

const _ubyte  CFS_Camper_HookIn::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	//테스트 아닐 시 주석 풀기
	CTransform* pHuntressTransform = dynamic_cast<CTransform*>(m_pHuntress->Get_Component(TEXT("Transform")));
	CModel* pHuntressModel = dynamic_cast<CModel*>(m_pHuntress->Get_Component(TEXT("Model_FPV")));
	_matrix CurSlasherMatrix = pHuntressModel->Get_BoneMatrix("joint_Char") * pHuntressTransform->Get_Matrix();
	m_pCamper_GamePlay->m_pTransform->Set_Matrix(CurSlasherMatrix);
	XMStoreFloat3(&m_fRootPrePos, m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS));

	// effect && sound
	if (m_pCamper_GamePlay->m_pModel->Get_Anim_KeyTimeAcc(30)) {
		CEffect_Blood_Free* pEffect = dynamic_cast<CEffect_Blood_Free*>(CGame_Instance::Get_Instance()->CreateGet_Object_Clone(TEXT("Effect_Blood"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_Blood")));
		if (nullptr != m_pHook) {
			CModel* pHookModel = dynamic_cast<CModel*>(m_pHook->Get_Component(TEXT("Model_Hook")));
			CTransform* pHookTransform = dynamic_cast<CTransform*>(m_pHook->Get_Component(TEXT("Transform")));
			_matrix SocketMatrix = pHookModel->Get_BoneMatrix("joint_SurvivorHook") * pHookTransform->Get_Matrix();
			_vector vEffectPos = SocketMatrix.r[3] + XMVectorSet(0.f, 2.f, 0.f, 0.f);
			CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffect->Get_Component(TEXT("Transform")));
			pEffectTransform->Set_Row(CTransform::ERow::POS, vEffectPos);
		}
		m_pAudio->Play_Sound_Rand(TEXT("sfx_blood_hook_put"), 3, -1, 1.f, FMOD_3D, m_pCamper_GamePlay->m_pTransform, _float3{ 0.f, 0.f, 0.f }, _float2{ 1.f, 5.f });
	}

	//rootbone
	_vector vPos;
	_float4x4 vSocket;
	CModel* pCamperModel = dynamic_cast<CModel*>(m_pCamper_GamePlay->Get_Component(TEXT("Model")));
	_matrix matrix = m_pCamper_GamePlay->m_pTransform->Get_Matrix();
	matrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	XMStoreFloat4x4(&vSocket, (XMLoadFloat4x4(&m_fRootOffsetMatrix) * pCamperModel->Get_RootBoneMatrix("joint_Char")) * matrix);
	vPos = XMLoadFloat4x4(&vSocket).r[3] + XMLoadFloat3(&m_fRootPrePos);
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vPos);

	if (m_pCamper_GamePlay->m_bControl) {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			return CFM_Camper::EState::HOOKOUT;
		}
	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_TT_Hook_OUT");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_TT_Hook_OUT");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_TT_Hook_OUT");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_TT_Hook_OUT");
				break;
			}
		}
	}

	return CFM_Camper::EState::END;
}
