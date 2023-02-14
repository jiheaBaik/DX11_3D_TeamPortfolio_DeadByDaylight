#include "pch.h"
#include "FS_Camper_Closet_Hide2.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"
#include "Inter_Locker.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Icon_Hud.h"

CFS_Camper_Closet_Hide2::CFS_Camper_Closet_Hide2(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CCamper_GamePlay* const& pCamper_GamePlay)
	: CFS(pDevice, pContext, pNetwork) {
	m_pCamper_GamePlay = pCamper_GamePlay;
}

void CFS_Camper_Closet_Hide2::Delete(void) {
	__super::Delete();
	m_pCamper_GamePlay = nullptr;
}

const HRESULT CFS_Camper_Closet_Hide2::Init(_char* ModelName) {
	m_pCamper_GamePlay->m_pModel->Set_Anim(ModelName, 0);
	m_pCamper_GamePlay->m_pModel->Update_Anim(0, true);

	if (m_pCamper_GamePlay->m_bControl) {
		m_pLocker = static_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}
	else {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		m_pCamper_GamePlay->m_pTarObject = pGame_Instance->Get_Object_CloneID(static_cast<_int>(EScene::GAMEPLAY), TEXT("Inter_Locker"), m_pCamper_GamePlay->m_iTarObjID);
		m_pLocker = dynamic_cast<CInter_Locker*>(m_pCamper_GamePlay->m_pTarObject);
	}
	m_pLockerTarnsform = dynamic_cast<CTransform*>(m_pLocker->Get_Component(TEXT("Transform")));
	XMMATRIX LockerMatrix = m_pLockerTarnsform->Get_Matrix();
	m_pCamper_GamePlay->m_pTransform->Set_Row(CTransform::ERow::POS, LockerMatrix.r[3]);

	m_pCamper_GamePlay->m_bFlag = true;
	return S_OK;
}

const _ubyte  CFS_Camper_Closet_Hide2::Update(const _double& dTimeDelta) {

	if (ANIM_END == m_pCamper_GamePlay->m_pModel->Update_Anim(dTimeDelta * 0.6f , true)) {
		m_pLocker->Set_State(CInter_Locker::ESTATE::HIDE_CAMPER);
		return CFM_Camper::EState::CLOSSET_IDLE;
	}
	return CFM_Camper::EState::END;
}
