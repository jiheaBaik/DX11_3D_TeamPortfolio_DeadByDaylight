#include "pch.h"
#include "FS_Slasher_Run_Carry.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "Inter_Locker.h"
#include "Inter_PlankAnim.h"
#include "Inter_Hatch.h"
#include "Inter_Generator.h"

CFS_Slasher_Run_Carry::CFS_Slasher_Run_Carry(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Run_Carry::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Run_Carry::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Run_Carry::Init(_uint iAnimIndex) {
	if (true == m_pSlasher_Huntress->m_bSlasher)
		m_pSlasher_Huntress->m_pModel2->Set_Anim(61);
	else
		m_pSlasher_Huntress->m_pModel2->Set_Anim(45);

	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	return S_OK;
}

const _ubyte  CFS_Slasher_Run_Carry::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(16))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(27))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(38))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_2D);
		}
		else {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(6))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(18))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_2D);
		}

		// Input
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
			m_pSlasher_Huntress->m_bMove[0] = true;
			m_eCurRun = ERun::RUN_FT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_W)) {
			m_pSlasher_Huntress->m_bMove[0] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_S)) {
			m_pSlasher_Huntress->m_bMove[1] = true;
			m_eCurRun = ERun::RUN_BK;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_S)) {
			m_pSlasher_Huntress->m_bMove[1] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_D)) {
			m_pSlasher_Huntress->m_bMove[2] = true;
			m_eCurRun = ERun::RUN_RT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D)) {
			m_pSlasher_Huntress->m_bMove[2] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A)) {
			m_pSlasher_Huntress->m_bMove[3] = true;
			m_eCurRun = ERun::RUN_LT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A)) {
			m_pSlasher_Huntress->m_bMove[3] = false;
		}

		if ((true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[1]) || (true == m_pSlasher_Huntress->m_bMove[2] && true == m_pSlasher_Huntress->m_bMove[3]))
			return CFM_Slasher::EState::IDLE_CARRY;

		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
			m_eCurRun = ERun::RUN_FTRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
			m_eCurRun = ERun::RUN_FTLT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
			m_eCurRun = ERun::RUN_BKRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
			m_eCurRun = ERun::RUN_BKLT;
		}

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L))
			return CFM_Slasher::EState::ATTACK_CARRY_IN;
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_R)) {
			CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
			pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::DROP);
			pCamper->Get_FM_Camper()->Update_Network("Set_State");
			return CFM_Slasher::EState::DROP;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_SPACE)) {
			if (CSlasher_Huntress::ECollisionState::HOOK == m_pSlasher_Huntress->m_eCollisionState) {
				CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(m_pSlasher_Huntress->m_pTarObject);
				pCamper->Set_TarObject(m_pSlasher_Huntress->m_pColObject);
				pCamper->Set_TarObjID(m_pSlasher_Huntress->m_pColObject->Get_Id());
				pCamper->Update_Network("Set_TarObjID");
				pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::HOOKIN);
				pCamper->Get_FM_Camper()->Update_Network("Set_State");

				m_pSlasher_Huntress->m_pTarObject = m_pSlasher_Huntress->m_pColObject;
				m_pSlasher_Huntress->Update_Network("Set_TarObjID");
				CInter_Hook* pHook = dynamic_cast<CInter_Hook*>(m_pSlasher_Huntress->m_pTarObject);
				pHook->Set_State(CInter_Hook::ESTATE::HANGING);
				return CFM_Slasher::EState::HOOK;
			}
		}
		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (false == m_pSlasher_Huntress->m_bCollider_Move) {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
		}
		else {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
			}
		}
		if (false == m_pSlasher_Huntress->m_bMove[0] && false == m_pSlasher_Huntress->m_bMove[1] && false == m_pSlasher_Huntress->m_bMove[2] && false == m_pSlasher_Huntress->m_bMove[3])
			return CFM_Slasher::EState::IDLE_CARRY;
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
		//m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);

		if (m_ePreRun == m_eCurRun)
			return CFM_Slasher::EState::END;
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_FTRT;
			return CFM_Slasher::EState::RUN_CARRY_FTRT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_FTLT;
			return CFM_Slasher::EState::RUN_CARRY_FTLT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_BKRT;
			return CFM_Slasher::EState::RUN_CARRY_BKRT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_BKLT;
			return CFM_Slasher::EState::RUN_CARRY_BKLT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true) {
			m_ePreRun = ERun::RUN_FT;
			return CFM_Slasher::EState::RUN_CARRY_FT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true) {
			m_ePreRun = ERun::RUN_BK;
			return CFM_Slasher::EState::RUN_CARRY_BK;
		}
		if (m_pSlasher_Huntress->m_bMove[2] == true) {
			m_ePreRun = ERun::RUN_RT;
			return CFM_Slasher::EState::RUN_CARRY_RT;
		}
		if (m_pSlasher_Huntress->m_bMove[3] == true) {
			m_ePreRun = ERun::RUN_LT;
			return CFM_Slasher::EState::RUN_CARRY_LT;
		}
	}
	else {
		if (true == m_pSlasher_Huntress->m_bSlasher) {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(5))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(16))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(27))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(38))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_heavy"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}
		else {
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(6))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
			if (m_pSlasher_Huntress->m_pModel_FPV->Get_Anim_KeyTimeAcc(18))
				m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_k00_common_add_medium"), 9, -1, 0.2f, FMOD_3D, m_pSlasher_Huntress->m_pTransform, _float3{ 0, 0, 0 }, _float2{ 1.f, 5.f });
		}

		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (CFM_Slasher::EState::RUN_CARRY_FTRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_CARRY_FTLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_CARRY_BKRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_CARRY_BKLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_CARRY_FT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_CARRY_BK == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_CARRY_RT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
		else if (CFM_Slasher::EState::RUN_CARRY_LT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
			m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel2->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
		//m_pSlasher_Huntress->m_pModel->Change_Bone(m_pSlasher_Huntress->m_pModel2, m_pSlasher_Huntress->m_vecChange_Bone);
	}

	return CFM_Slasher::EState::END;
}
