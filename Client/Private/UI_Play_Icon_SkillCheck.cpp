#include "pch.h"
#include "UI_Play_Icon_SkillCheck.h"
#include "UI_Play_Score.h"
#include "Camper_GamePlay.h"

CUI_Play_Icon_SkillCheck* const CUI_Play_Icon_SkillCheck::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Icon_SkillCheck* pInstnace = new CUI_Play_Icon_SkillCheck(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Icon_SkillCheck::Clone(void* const& pArg) {
	CUI_Play_Icon_SkillCheck* pInstnace = new CUI_Play_Icon_SkillCheck(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Icon_SkillCheck::CUI_Play_Icon_SkillCheck(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Icon_SkillCheck::CUI_Play_Icon_SkillCheck(const CUI_Play_Icon_SkillCheck& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Icon_SkillCheck::Delete(void) {
	__super::Delete();
	for (auto& iter : m_pFXTexture)
		Safe_Release(iter);
}

const HRESULT CUI_Play_Icon_SkillCheck::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Play_Icon_SkillCheck::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(L"icon_gen_back");

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pFXTexture[0] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"icon_gen_hitin").c_str()));
	m_pFXTexture[1] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"icon_gen_hitout").c_str()));
	m_vAngleRange = { 10.f, 60.f };

	//this->Set_HitCount(3, 30.0);
	//this->Set_HitCount(5, 30.0, ECheck::START, 4
	return hr;
}

const _ubyte CUI_Play_Icon_SkillCheck::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
		this->Set_Parent(pParent, false);
		m_bActiveByParent = false;
		this->Set_Active(false);
		m_bStart = true;
	}
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;
	if (!m_bPreActive) {
		//m_pAudio->Play_Sound_Rand(TEXT("sfx_hud_skillcheck_open"), 4, -1, 0.2f, FMOD_2D);
		m_eState = EState::STOP;
	}
	if (m_eState == EState::NONE) {
		m_bRun = false;
		m_bOneOff = false;
		m_fTime = 0.f;
		this->Reset_Info(false);
		this->Set_Active(false);
		return OBJ_NOEVENT;
	}

	_bool bHit = false;
	if (/*m_fTime < m_fMaxTime &&*/ m_iHit < m_iMaxHit) {
		if (EState::FAIL != m_eState) {
			m_fTime += static_cast<_float>(dTimeDelta);
			if (m_fTime >= static_cast<_float>(m_iHitTimes[m_iHit]) - 0.8f && !m_bOneOff) {
				m_pAudio->Play_Sound(TEXT("sfx_hud_skillcheck_open-1"), -1, 0.2f, FMOD_2D);
				m_bOneOff = true;
			}
			if (m_fTime >= static_cast<_float>(m_iHitTimes[m_iHit]))
				bHit = true;
		}
		switch (m_eState)
		{
		case Client::CUI_Play_Icon_SkillCheck::EState::STOP: {		 
			if (bHit) {
				this->Reset_Info(true);
				m_eState = EState::PLAY;
			}
		}
															   break;
		case Client::CUI_Play_Icon_SkillCheck::EState::PLAY: {
			if (m_fCurAngle <= 360.f) {				
				if (m_fCurAngle < m_fRanAngle) {
					if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_SPACE)) {
						_float fMax = Clamp_Angle(m_fRanAngle - m_vAngleRange.y, 0.f, 360.f);
						if (m_fCurAngle > fMax) {
							_float fMin = Clamp_Angle(m_fRanAngle - m_vAngleRange.x, 0.f, 360.f);
							(m_fCurAngle > fMin) ? m_eState = EState::GREAT : m_eState = EState::SUCCESS;						
							break;
						}
						else {
							m_eState = EState::FAIL;
							break;
						}
					}
				}
				else {
					m_eState = EState::FAIL;
					break;
				}
				//m_fCurAngle += (_float)dTimeDelta * m_fSpeed;
				m_fCurAngle += (_float)dTimeDelta * m_fPerAng;
				m_vecImages[static_cast<_uint>(ETexType::POINT)].pRectTransform->Set_State(CRectTransform::EState::ORB, _float3(0.f, 0.f, 360.f - m_fCurAngle));
			}
			else {
				m_eState = EState::FAIL;
				break;
			}			
		}
															   break;
		case Client::CUI_Play_Icon_SkillCheck::EState::FAIL: {
			if (m_fTime >= static_cast<_float>(m_iHitTimes[++m_iHit]))
				++m_iHit;
			m_bFailed = true;
			m_pAudio->Play_Sound_Rand(TEXT("sfx_hud_wiggle_skillcheck_fail"), 1, -1, 0.2f, FMOD_2D);
			m_bOneOff = false;
			m_eState = EState::NONE;
			break;
		}
															   break;
		case Client::CUI_Play_Icon_SkillCheck::EState::SUCCESS: {
			// good
			CInfo* pInfo = CInfo::Get_Instance();
			//hwang - score - generator
			if (pInfo->Get_Player() == false) {
				list<CObject*>* pListCamper = pGame_Instance->Get_Object_ListClone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
				if (nullptr != pListCamper) {
					for (auto& iter : *pListCamper) {
						if (iter->Get_Control() == true) {
							CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
							if (pScore != nullptr) {
								pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::OBJECTIVE_GOODSKILL));
								pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::OBJECTIVE), 50);
							}
						}
					}
				}
			}
			if (m_fTime >= static_cast<_float>(m_iHitTimes[++m_iHit]))
				++m_iHit;
			m_bFailed = false;
			this->Reset_Info(false);
			m_pAudio->Play_Sound_Rand(TEXT("sfx_hud_skillcheck_good"), 2, -1, 0.05f, FMOD_2D);
			m_bOneOff = false;
			m_eState = EState::STOP;
			break;
		}
		case Client::CUI_Play_Icon_SkillCheck::EState::GREAT: {
			// great
			CInfo* pInfo = CInfo::Get_Instance();
			//hwang - score - generator
			if (pInfo->Get_Player() == false) {
				list<CObject*>* pListCamper = pGame_Instance->Get_Object_ListClone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
				if (nullptr != pListCamper) {
					for (auto& iter : *pListCamper) {
						if (iter->Get_Control() == true) {
							CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("UI_Play_Score")));
							if (pScore != nullptr) {
								pScore->Add_Score(static_cast<_uint>(CUI_Play_Score::ECamperScore::OBJECTIVE_GREATSKILL));
								pScore->Plus_CamperScore(static_cast<_uint>(CUI_Play_Score::ECamperNum::OBJECTIVE), 300);
							}
						}
					}
				}
			}
			if (m_fTime >= static_cast<_float>(m_iHitTimes[++m_iHit]))
				++m_iHit;
			m_bFailed = false;
			this->Reset_Info(false);
			m_pAudio->Play_Sound_Rand(TEXT("sfx_hud_skillcheck_bonus"), 2, -1, 0.05f, FMOD_2D);
			m_bOneOff = false;
			m_eState = EState::STOP;
			break;
		}											  break;
		}

	}
	else {
		m_eState = EState::NONE;
	}

	return OBJ_NOEVENT;
}

void CUI_Play_Icon_SkillCheck::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	ImGui::DragInt("state", (_int*)&m_eState, 0, 0, 0, EState_ToString(m_eState));
	ImGui::InputInt("hit", (_int*)&m_iHit);
	ImGui::InputFloat("unit", &m_fUnit);
	ImGui::InputFloat("time", &m_fTime, 0.1f);
	ImGui::InputFloat("angle", &m_fCurAngle, 0.1f);

	ImGui::Checkbox("run", &m_bRun);

	ImGui::Separator();

	//for (_uint i = 0; i < m_iMaxHit; ++i) {
	//	_float fHitTime = static_cast<_float>(m_dHitTimes[i]);
	//	ImGui::InputFloat("times", &fHitTime, 0.1f);
	//}
	for (_uint i = 0; i < m_iMaxHit; ++i) {
		_float fHitTime = static_cast<_float>(m_iHitTimes[i]);
		ImGui::InputFloat("times", &fHitTime, 0.1f);
	}
			
	Update_Edit_Images();
}

void CUI_Play_Icon_SkillCheck::Late_Update(const _double& dTimeDelta) {
	__super::Update(dTimeDelta);
	__super::Late_Update(dTimeDelta);
	
	m_ePreState = m_eState;
}

const HRESULT CUI_Play_Icon_SkillCheck::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf || !m_bRender)
		return hr;
	hr |= __super::Set_RawValue(m_pShader);
	// back
	SImageDesc* pImage = &m_vecImages[0];
	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
	hr |= m_pShader->Set_RawValue("g_vColor", &pImage->vColor, sizeof(_float4));
	hr |= pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	hr |= m_pVBRect->Render(m_pShader, pImage->iPass);

	// frame
	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &m_pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
	hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
	hr |= m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	hr |= m_pVBRect->Render(m_pShader, m_iPass);

	// fx ///////////////////////////////////
	_float4x4 f4World = m_pRectTransform->Get_Float4x4();
	_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World) * XMMatrixScaling(1.1f, 1.1f, 1.1f));

	_float fAngle = XMConvertToRadians(360.f - (!m_bFailed ? m_vAngleRange.x : 30.f));
	hr |= m_pShader->Set_RawValue("g_fAngle", &fAngle, sizeof(_float));
	//_bool bBoth = true;
	//hr |= m_pShader->Set_RawValue("g_bBothRange", &bBoth, sizeof(_float));
	hr |= m_pFXTexture[0]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_COOLDOWN));

	f4World = m_pRectTransform->Get_Float4x4();
	matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World) * XMMatrixScaling(1.05f, 1.05f, 1.05f));
	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
	fAngle = XMConvertToRadians(360.f - m_vAngleRange.y);
	hr |= m_pShader->Set_RawValue("g_fAngle", &fAngle, sizeof(_float));
	hr |= m_pFXTexture[1]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_COOLDOWN));
	///////////////////////////////////

	for (_uint i = 1; i < (_uint)m_vecImages.size(); ++i) {
		pImage = &m_vecImages[i];
		if (pImage->IsContinue())
			continue;
		// point
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &pImage->vColor, sizeof(_float4));
		hr |= pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pImage->iPass);
#ifdef _DEBUG
		if (pImage->bCollider)
			pImage->pCollider->Render();
#endif // _DEBUG
	}

	return hr;
}

void CUI_Play_Icon_SkillCheck::Set_Off() {
	m_eState = EState::NONE;
	this->Set_Active(false);
}

#define SWAP(x, y, temp)	((temp)=(x), (x)=(y), (y)=(temp))
void CUI_Play_Icon_SkillCheck::Set_HitCount(const _uint& iHitCnt, const _double& dTime, const ECheck& eCheck, const _float& fPerAng) {
	if (0 >= iHitCnt)
		return;
	// into first
	if (ECheck::RUN != eCheck/* m_ePreState == EState::NONE*/) {
		for (_uint i = 0; i < m_iMaxHit; ++i) {
			m_iHitTimes[i] = 0;
		}
		//m_fCurAngle = 360.f / fPerAng;
		// 360 < (delta * speed)/s 
		// ex. 200 * 0.02면 1프레임당 4도로 360도 -> 90프레임이니까 1.5초?
		// detault가 30도니까 1프레임당 4도로 330도 -> 82.5 

		// 랜덤뽑기
		_uint iCnt = 0, iRan = 0;
		// 한바퀴 당 시간
		//_uint iMin = static_cast<_uint>(m_fUnit = ((360.f / fPerAng) * (1.f / 60.f))); // static_cast<_uint>(ceil(m_fRanAngle / (fSpeed * 0.02f)) * (1.f / 60.f));
		_uint iUnit = static_cast<_uint>(m_fUnit = (360.f / fPerAng));
		// 전체 시간의 8할
		//(ECheck::START == eCheck) ? (m_fTime = 0.f) : (fMaxTime -= m_fTime);
		_float fMaxTime = (ECheck::START == eCheck) ? static_cast<_float>(dTime) : (static_cast<_float>(dTime) - m_fTime);

		_float fTemp = (1.f <= m_fUnit) ? (1.f / m_fUnit) : m_fUnit;
		_uint iMax = static_cast<_int>(fMaxTime * 0.8f * fTemp);  //  (50->40/1.5)

		//if ((iMin >= iMax) || ((_uint)iMin * iHitCnt >= iMax)) {
		//	m_bRender = false;
		//	this->Set_Active(true);
		//	return;
		//}
		_uint iMaxCnt = iMax / 4;
		//_uint iMaxCnt = rand() % (iMax / 2);		
		if (0 == iMaxCnt || 30 <= iMaxCnt || (iUnit >= iMax) || ((_uint)iUnit * iMaxCnt >= static_cast<_uint>(fMaxTime * 0.8f))) {
			m_bRender = false;
			this->Set_Active(false);
			return;
		}
		while (iCnt < /*iHitCnt*/ iMaxCnt) {
			_bool bSame = false;
			iRan = rand() % iMax;
			iRan *= iUnit;
			for (_uint i = 0; i < iCnt; ++i) {
				if (iRan == m_iHitTimes[i]) {
					bSame = true;
					break;
				}
			}
			if (!bSame)
				m_iHitTimes[iCnt++] = iRan;
			if (iCnt >= iMaxCnt)
				break;
		}
		// bobble
		for (_uint i = 0; i < iMaxCnt - 1; ++i) {
			for (_uint j = 0; j < (iMaxCnt - 1) - i; ++j) {
				if (m_iHitTimes[j] > m_iHitTimes[j + 1]) {
					_uint iTemp = 0;
					SWAP(m_iHitTimes[j], m_iHitTimes[j + 1], iTemp);
				}
			}
		}

		m_iHit = 0;
		m_fTime = 0.f;
		m_iMaxHit = iMaxCnt;
		m_fMaxTime = fMaxTime;
	}

	m_bRun = true;
	m_fPerAng = fPerAng;
	m_eState = EState::STOP;
	m_bRender = false;
	this->Set_Active(true);
}


void CUI_Play_Icon_SkillCheck::Reset_Info(const _bool& bActive) {
	if (true == bActive) {
		m_fCurAngle = 0.f;
		m_vecImages[static_cast<_uint>(ETexType::POINT)].pRectTransform->Set_State(CRectTransform::EState::ORB, _float3(0.f, 0.f, 0.f));

		_float fRand = (_float)(rand() % 12);
		m_fRanAngle = Clamp_Angle(120.f + 30.f * fRand, 30.f, 360.f);
		m_pRectTransform->Set_State(CRectTransform::EState::ORB, _float3(0.f, 0.f, 360.f - m_fRanAngle));
		m_vecImages[static_cast<_uint>(ETexType::DIV)].pRectTransform->Set_State(CRectTransform::EState::ORB, _float3(0.f, 0.f, m_vAngleRange.y));
	}
	m_bRender = bActive;
	for (auto& iter : m_vecImages)
		iter.bRender = bActive;
}

// fmod(source, divider) => source - floor(source / divider) * divider
_float CUI_Play_Icon_SkillCheck::Clamp_Angle(const _float& fAng, const _float& fMin, const _float& fMax) {
	_float fMinAng = fMin - fAng;
	_float fMaxAng = fMax - fAng;
	if (0 >= fMinAng && 0 <= fMaxAng)
		return fAng;
	if (fabs(fMinAng) < fabs(fMaxAng))
		return fMin;
	return fMax;
}

const HRESULT CUI_Play_Icon_SkillCheck::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_Play_Icon_SkillCheck::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.8f, 0.8f, 0.8f));
	//this->Set_Active(false);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr;
	// back
	pImage = Add_Image(L"icon_circle3", static_cast<_uint>(ETexType::BACK), false);
	pImage->pRectTransform->Set_Desc(m_pRectTransform->Get_Desc());
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.9f, 0.9f, 0.9f));
	pImage->vColor.w = 0.8f;
	// frame
	pImage = Add_Image(L"play_hud_space", static_cast<_uint>(ETexType::HUD), false);
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.5f, 0.5f, 0.5f));
	// point
	pImage = Add_Image(L"icon_gen_point", static_cast<_uint>(ETexType::POINT), false);
	_float fY = m_pRectTransform->Get_Desc().fHeight * 0.3f;
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(0.f, -fY, 0.f), true);
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.7f, 0.7f, 0.7f));
	// ?
	pImage = Add_Image(L"icon_gen_divide", static_cast<_uint>(ETexType::DIV), true, CRectTransform::EEdge::CTOP);
	pImage->pRectTransform->Set_Parent(m_pRectTransform);
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(0.f, 22.f, 0.f));
	pImage->pRectTransform->Set_Size(2.f, 6.f);

	this->Sort_Images();
}
