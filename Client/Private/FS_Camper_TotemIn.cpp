#include "pch.h"
#include "FS_Camper_TotemIn.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Totem.h"

CFS_Camper_TotemIn::CFS_Camper_TotemIn(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
	//Init();
}

void CFS_Camper_TotemIn::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_TotemIn::Init(_char* pModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(pModelName);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (m_pCamper_GamePlay->m_bControl) {
		
		m_pTotem = static_cast<CInter_Totem*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Totem"), m_pCamper_GamePlay->m_iTarObjID);
		m_pTotem = dynamic_cast<CInter_Totem*>(m_pCamper_GamePlay->m_pTarObject);
	}
	m_pTotemTarnsform = dynamic_cast<CTransform*>(m_pTotem->Get_Component(TEXT("Transform")));
	return S_OK;
}

const _ubyte  CFS_Camper_TotemIn::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	_vector vTotmePos = m_pTotemTarnsform->Get_Row(CTransform::ERow::POS);
	_vector vCampPos = m_pCamper_GamePlay->m_pTransform->Get_Row(CTransform::ERow::POS);
	_vector vLook = vCampPos - vTotmePos;
	_vector vLook2 = XMVectorSetY(vTotmePos - vCampPos, 0.f);
	_vector vDist = XMVector3Normalize(vLook) * 0.6f;
	_vector vLerpPos = (vTotmePos + vDist) * 0.2f + vCampPos * 0.8f;

	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, vLerpPos);
	m_pCamper_GamePlay->m_pTransform->LookAt(vCampPos + vLook2);

	if (m_pCamper_GamePlay->m_bControl) {
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			//토템정화 ui렌더 x
			if (2 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::IDLE;
			else if (1 == m_pCamper_GamePlay->Get_Hp())
				return CFM_Camper::EState::HIT_IDLE;
		}

		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false))
			return CFM_Camper::EState::TOTEM_LOOP;

	}
	else {
		if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta, false)) {
			switch (m_pCamper_GamePlay->m_eCampKind)
			{
			case CCamper_GamePlay::ECamperKind::CHERYL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("S22_DSkeleton_REF.ao|F_BlessTotem_Loop");
				break;
			case CCamper_GamePlay::ECamperKind::MEG:
				m_pCamper_GamePlay->m_pModel->Set_Anim("MT_DSkeleton_REF.ao|F_BlessTotem_Loop");
				break;
			case CCamper_GamePlay::ECamperKind::BILL:
				m_pCamper_GamePlay->m_pModel->Set_Anim("BO_DSkeleton_REF.ao|M_BlessTotem_LOOP");
				break;
			case CCamper_GamePlay::ECamperKind::DWIGHT:
				m_pCamper_GamePlay->m_pModel->Set_Anim("DF_DSkeleton_REF.ao|M_BlessTotem_LOOP");
				break;
			}
		}
	}
	return CFM_Camper::EState::END;
}
