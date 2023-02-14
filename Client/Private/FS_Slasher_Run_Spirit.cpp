#include "pch.h"
#include "FS_Slasher_Run_Spirit.h"
#include "Slasher_Huntress.h"
#include "UI_Play_Icon_Hud.h"
#include "UI_Play_Gauge_Inter.h"

CFS_Slasher_Run_Spirit::CFS_Slasher_Run_Spirit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, CSlasher_Huntress* const& pSlasher_Huntress)
	: CFS(pDevice, pContext, pNetwork) {
	m_pSlasher_Huntress = pSlasher_Huntress;
	//Init();
}

void CFS_Slasher_Run_Spirit::Delete(void) {
	__super::Delete();
}

const HRESULT CFS_Slasher_Run_Spirit::Init(_char* pModelName) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(pModelName);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(pModelName);
	return S_OK;
}

const HRESULT CFS_Slasher_Run_Spirit::Init(_uint iAnimIndex) {
	m_pSlasher_Huntress->m_pModel->Set_Anim(iAnimIndex);
	m_pSlasher_Huntress->m_pModel_FPV->Set_Anim(iAnimIndex);

	return S_OK;
}

const _ubyte  CFS_Slasher_Run_Spirit::Update(const _double& dTimeDelta) {
	if (true == m_pSlasher_Huntress->m_bControl) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

		CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
		pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, m_pSlasher_Huntress->m_fSpiritGauge, false);

		if (m_pSlasher_Huntress->m_fSpiritGauge <= 0.f) {
			m_pSlasher_Huntress->m_bSpirit = false;
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_activephasing_flash"), 2, -1, 0.3f, FMOD_2D);
			pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, false, false);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Stop_Sound(6);
			return CFM_Slasher::EState::SPIRIT_OUT;
		}
		else
			m_pSlasher_Huntress->m_fSpiritGauge -= static_cast<_float>(dTimeDelta) * 35.f;

		_vector vLook = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 0.f));
		_vector vRight = XMVector3Normalize(XMVectorSetY(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 0.f));

		_vector vDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		// Input
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_W)) {
			vDir += vLook;
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
			vDir += vRight;
			m_pSlasher_Huntress->m_bMove[2] = true;
			m_eCurRun = ERun::RUN_RT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_D)) {
			m_pSlasher_Huntress->m_bMove[2] = false;
		}
		if (pGame_Instance->Get_Input_KeyboardKey(DIK_A)) {
			vDir -= vRight;
			m_pSlasher_Huntress->m_bMove[3] = true;
			m_eCurRun = ERun::RUN_LT;
		}
		if (pGame_Instance->Get_Input_KeyboardKeyUp(DIK_A)) {
			m_pSlasher_Huntress->m_bMove[3] = false;
		}

		if ((true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[1]) || (true == m_pSlasher_Huntress->m_bMove[2] && true == m_pSlasher_Huntress->m_bMove[3]))
			return CFM_Slasher::EState::IDLE_SPIRIT;

		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
			vDir += vLook;
			vDir += vRight;
			m_eCurRun = ERun::RUN_FTRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
			vDir += vLook;
			vDir -= vRight;
			m_eCurRun = ERun::RUN_FTLT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
			vDir += vRight;
			m_eCurRun = ERun::RUN_BKRT;
		}
		if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
			vDir -= vRight;
			m_eCurRun = ERun::RUN_BKLT;
		}

		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			m_pSlasher_Huntress->m_bSpirit = false;
			m_pSlasher_Huntress->m_eType = CSlasher_Huntress::EType::SLASH;
			m_pSlasher_Huntress->Update_Network("Set_Type");
			vDir = XMVector3Normalize(vDir);
			XMStoreFloat3(&m_vDir, vDir);
			m_pSlasher_Huntress->m_pFM->Update_Network("Set_MoveDir");
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_activephasing_flash"), 2, -1, 0.3f, FMOD_2D);
			pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, false, false);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Stop_Sound(6);
			return CFM_Slasher::EState::ATTACK_SLASH_IN;
		}
		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::R)) {
			m_pSlasher_Huntress->m_bSpirit = false;
			vDir = XMVector3Normalize(vDir);
			XMStoreFloat3(&m_vDir, vDir);
			m_pSlasher_Huntress->m_pFM->Update_Network("Set_S_MoveDir");
			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_activephasing_flash"), 2, -1, 0.3f, FMOD_2D);
			pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::SLA_SKILL, false, false);

			m_pSlasher_Huntress->m_pAudio->Play_Sound_Rand(TEXT("sfx_pwr_haiti_glass"), 4, -1, 0.3f, FMOD_2D);
			m_pSlasher_Huntress->m_pAudio->Stop_Sound(6);
			return CFM_Slasher::EState::SPIRIT_OUT;
		}

		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (false == m_pSlasher_Huntress->m_bCollider_Move) {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[0])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 7.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[1])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -7.4f - fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[2])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (true == m_pSlasher_Huntress->m_bMove[3])
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -7.4f - fPerkSpeed, dTimeDelta);
		}
		else {
			if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[0]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 7.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[1]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -7.4f - fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[2]) {
				_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			}
			else if (true == m_pSlasher_Huntress->m_bMove[3]) {
				_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
				_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
				if (fDotTarget < 0)
					m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -7.4f - fPerkSpeed, dTimeDelta);
			}
		}

		if (false == m_pSlasher_Huntress->m_bMove[0] && false == m_pSlasher_Huntress->m_bMove[1] && false == m_pSlasher_Huntress->m_bMove[2] && false == m_pSlasher_Huntress->m_bMove[3])
			return CFM_Slasher::EState::IDLE_SPIRIT;
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);

		if (m_ePreRun == m_eCurRun)
			return CFM_Slasher::EState::END;
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_FTRT;
			return CFM_Slasher::EState::RUN_SPIRIT_FTRT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_FTLT;
			return CFM_Slasher::EState::RUN_SPIRIT_FTLT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[2]) {
			m_ePreRun = ERun::RUN_BKRT;
			return CFM_Slasher::EState::RUN_SPIRIT_BKRT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true && m_pSlasher_Huntress->m_bMove[3]) {
			m_ePreRun = ERun::RUN_BKLT;
			return CFM_Slasher::EState::RUN_SPIRIT_BKLT;
		}
		if (m_pSlasher_Huntress->m_bMove[0] == true) {
			m_ePreRun = ERun::RUN_FT;
			return CFM_Slasher::EState::RUN_SPIRIT_FT;
		}
		if (m_pSlasher_Huntress->m_bMove[1] == true) {
			m_ePreRun = ERun::RUN_BK;
			return CFM_Slasher::EState::RUN_SPIRIT_BK;
		}
		if (m_pSlasher_Huntress->m_bMove[2] == true) {
			m_ePreRun = ERun::RUN_RT;
			return CFM_Slasher::EState::RUN_SPIRIT_RT;
		}
		if (m_pSlasher_Huntress->m_bMove[3] == true) {
			m_ePreRun = ERun::RUN_LT;
			return CFM_Slasher::EState::RUN_SPIRIT_LT;
		}
	}
	else {
		_float fPerkSpeed = m_pSlasher_Huntress->m_fPerkSpeed;
		if (false == m_pSlasher_Huntress->m_bCollider_Move) {
			if (CFM_Slasher::EState::RUN_SPIRIT_FTRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SPIRIT_FTLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SPIRIT_BKRT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SPIRIT_BKLT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SPIRIT_FT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 7.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SPIRIT_BK == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -7.4f - fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SPIRIT_RT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 7.4f + fPerkSpeed, dTimeDelta);
			else if (CFM_Slasher::EState::RUN_SPIRIT_LT == m_pSlasher_Huntress->Get_FM_Slasher()->Get_State())
				m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -7.4f - fPerkSpeed, dTimeDelta);
		}
		else {
			//if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[2]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[0] && true == m_pSlasher_Huntress->m_bMove[3]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[2]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[1] && true == m_pSlasher_Huntress->m_bMove[3]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK) + -m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[0]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[1]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::LOOK), -4.4f - fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[2]) {
			//	_vector vMove = XMVector3Normalize(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), 4.4f + fPerkSpeed, dTimeDelta);
			//}
			//else if (true == m_pSlasher_Huntress->m_bMove[3]) {
			//	_vector vMove = XMVector3Normalize(-m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT));
			//	_float fDotTarget = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSlasher_Huntress->m_vCollider_Move), vMove));
			//	if (fDotTarget < 0)
			//		m_pSlasher_Huntress->m_pTransform->Translate(m_pSlasher_Huntress->m_pTransform->Get_Row(CTransform::ERow::RIGHT), -4.4f - fPerkSpeed, dTimeDelta);
			//}
		}
		// Anim
		m_pSlasher_Huntress->m_pModel->Update_Anim(dTimeDelta, true);
		m_pSlasher_Huntress->m_pModel_FPV->Update_Anim(dTimeDelta, true);
	}

	return CFM_Slasher::EState::END;
}

const _float3& CFS_Slasher_Run_Spirit::Get_Dir(void) {
	return m_vDir;
}
