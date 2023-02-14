#include "pch.h"
#include "UI_End_Panel_Second.h"
#include "UI_Play_Score.h"

CUI_End_Panel_Second* const CUI_End_Panel_Second::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_End_Panel_Second* pInstnace = new CUI_End_Panel_Second(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_End_Panel_Second::Clone(void* const& pArg) {
	CUI_End_Panel_Second* pInstnace = new CUI_End_Panel_Second(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_End_Panel_Second::CUI_End_Panel_Second(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_End_Panel_Second::CUI_End_Panel_Second(const CUI_End_Panel_Second& rhs)
	: CUI_Image(rhs) {
}

void CUI_End_Panel_Second::Delete(void) {
	__super::Delete();
	for (auto& iter : m_sHuds) {
		iter.sFont.Release();
		for (auto& value : iter.sFonts)
			value.Release();
	}
}

const HRESULT CUI_End_Panel_Second::Init_Create(void) {
	HRESULT hr = S_OK;
	m_iGroup = 1;
	return hr;
}

const HRESULT CUI_End_Panel_Second::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone();

	// 패널 등장시간 조정 + 결과창 Set State 확인 (CFS_Camper_Ending)
	// 관전/계속하기 버튼 만들기 -> 관전 후 돌아올시 점수판 X
	m_iType = 0;
	return hr; 
}

const _ubyte CUI_End_Panel_Second::Update(const _double& dTimeDelta) {
	if (!m_bStart) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CInfo* pInfo = CInfo::Get_Instance();
		CUI_Play_Score* pScore = dynamic_cast<CUI_Play_Score*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Score"));

		_float fRatio = _float(g_iWinSizeX) / 1920.f;
		if (pInfo->Get_Player()) {
			for (_uint i = 0; i < 4; ++i) {
				_tchar szKey[MAX_PATH] = L"end_sec_btn_sla";
				_tchar szBuf[8] = L"";
				_itow_s(i + 1, szBuf, 10);
				lstrcat(szKey + 1, szBuf);
				// set image
				m_sHuds[i].pIconImg = Add_Image(szKey, static_cast<_uint>(ETexType::ICON), true, CRectTransform::EEdge::LTOP);
				CRectTransform* pParent = m_sHuds[i].pIconImg->pRectTransform;
				_float3 vDist = _float3(110.f + ((m_sHuds[i].pIconImg->pRectTransform->Get_Desc().fWidth + 30.f) * i), 200.f, 0.f);
				pParent->Set_State(CRectTransform::EState::POS, vDist);
				m_sHuds[i].pIconImg->vColor.w = 0.f;
				m_sHuds[i].pIconImg->bSave = false;
				// set font
				m_sHuds[i].pScore = pScore->Get_SlasherScorePtr(i);
				for (_uint j = 0; j < 5; ++j) {
					m_sHuds[i].sFonts[4 - j] = Add_Font(L"Gothic22", true, _float2(44.f * fRatio, 44.f * fRatio));
					m_sHuds[i].sFonts[4 - j].pRectTransform->Set_Parent(pParent);
					m_sHuds[i].sFonts[4 - j].pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-10.f + (8.f * j), 50.f, 0.f));
				}
			}
			m_sHuds[4].pScore = pScore->Get_SumScorePtr();
			for (_uint j = 0; j < 5; ++j) {
				m_sHuds[4].sFonts[4 - j] = Add_Font(L"Gothic24", true, _float2(48.f * fRatio, 48.f * fRatio), CRectTransform::EEdge::LCOR);
				m_sHuds[4].sFonts[4 - j].pRectTransform->Set_State(CRectTransform::EState::POS, _float3(100.f + (13.f * j), 0.f, 0.f));
			}
		}
		else {
			for (_uint i = 0; i < 4; ++i) {
				_tchar szKey[MAX_PATH] = L"end_sec_btn_cam";
				_tchar szBuf[8] = L"";
				_itow_s(i + 1, szBuf, 10);
				lstrcat(szKey + 1, szBuf);
				// set image
				m_sHuds[i].pIconImg = Add_Image(szKey, static_cast<_uint>(ETexType::ICON), true, CRectTransform::EEdge::LTOP);
				CRectTransform* pParent = m_sHuds[i].pIconImg->pRectTransform;
				_float3 vDist = _float3(110.f + ((m_sHuds[i].pIconImg->pRectTransform->Get_Desc().fWidth + 30.f) * i), 200.f, 0.f);
				pParent->Set_State(CRectTransform::EState::POS, vDist);
				m_sHuds[i].pIconImg->vColor.w = 0.f;
				m_sHuds[i].pIconImg->bSave = false;
				// set font
				//m_sHuds[i].sFont = Add_Font(L"Gothic24", true, _float2(44.f * fRatio, 44.f * fRatio));
				//m_sHuds[i].sFont.pRectTransform->Set_Parent(pParent);
				//m_sHuds[i].sFont.pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-10.f, 60.f, 0.f));
				m_sHuds[i].pScore = pScore->Get_CamperScorePtr(i);
				for (_uint j = 0; j < 5; ++j) {
					m_sHuds[i].sFonts[4 - j] = Add_Font(L"Gothic22", true, _float2(44.f * fRatio, 44.f * fRatio));
					m_sHuds[i].sFonts[4 - j].pRectTransform->Set_Parent(pParent);
					m_sHuds[i].sFonts[4 - j].pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-10.f+ (8.f * j), 50.f, 0.f));
				}
			}
			//m_sHuds[4].sFont = Add_Font(L"Gothic26", true, _float2(48.f * fRatio, 48.f * fRatio), CRectTransform::EEdge::LCOR);
			//m_sHuds[4].sFont.pRectTransform->Set_State(CRectTransform::EState::POS, _float3(100.f, 0.f, 0.f));
			m_sHuds[4].pScore = pScore->Get_SumScorePtr();
			for (_uint j = 0; j < 5; ++j) {
				m_sHuds[4].sFonts[4 - j] = Add_Font(L"Gothic24", true, _float2(48.f * fRatio, 48.f * fRatio), CRectTransform::EEdge::LCOR);
				m_sHuds[4].sFonts[4 - j].pRectTransform->Set_State(CRectTransform::EState::POS, _float3(100.f + (13.f * j), 0.f, 0.f));
			}
		}

		this->Sort_Images();
		this->Set_Active(false);
		m_bStart = true;
	}

	__super::Update(dTimeDelta);
	if (!m_bActiveSelf) {
		if (m_bPreActive && !m_vecImages.empty()) {
			for (auto& iter : m_vecImages) {
				iter.vColor.w = 0.f;
				iter.bRender = false;
				//if (iter.iDepth < static_cast<_uint>(ETexType::ICON)) {
				//	iter.vColor.w = 0.f;
				//	iter.bRender = false;
				//}
			}	
			m_pAlpha = nullptr;
			m_fBtnAlpha = -1.f;
		}
		return OBJ_NOEVENT;
	}	

	if (!m_bPreActive) {
		m_iType = 0;
		this->Set_FontCnt(EScoreType::END, 0);
	}

	if (!m_vecImages.empty()) {
		if (!m_vecImages[m_iType].bRender) {
			m_vecImages[m_iType].bRender = true;
			m_pAlpha = &(m_vecImages[m_iType].vColor.w = 0.f);
		}
		else {
			if (nullptr != m_pAlpha) {
				*m_pAlpha += (_float)dTimeDelta;
				if (1.0f <= *m_pAlpha) {
					*m_pAlpha = 1.f;
					m_pAlpha = nullptr;
					if ((m_iType + 1) < static_cast<_uint>(ETexType::ICON)) {
						++m_iType;
						if (m_iType == static_cast<_uint>(ETexType::PANEL3)) 
							m_fBtnAlpha = 0.f;
					}					
				}
			}
		}
	}

	_uint iIdx = 0;
	for (auto& iter : m_sHuds) {
		SImageDesc* pImage = iter.pIconImg;
		if (nullptr != pImage) {
			// hover
			if (IsCollisionStay(pImage->pCollider, pImage->pRectTransform)) {
				if (nullptr != iter.pExpImg)
					iter.pExpImg->bRender = true;
			}
			else {
				if (nullptr != iter.pExpImg)
					iter.pExpImg->bRender = false;
			}

		}
		//if (nullptr != iter.sFont.pRectTransform) 
		//	iter.sFont.pRectTransform->Update_Desc();
		for (auto& value : iter.sFonts)
			value.pRectTransform->Update_Desc();

		if (0.f <= m_fBtnAlpha/* && iter.iScore <= *iter.pScore*/) {
			if (!iter.bOneOff) {
				_bool bRenders[5] = { true, true, true, true, true };
				this->Calculate_Score(iter.iDigits, *iter.pScore, bRenders);
				// ex. 0304
				for (_uint j = 0; j < 5; ++j) {
					iter.sFonts[j].bRender = bRenders[j];
				}	
				iter.bOneOff = true;
			}

			// 0점 띄우기
			SFontDesc* pFont = &iter.sFonts[iter.iIdx];
			if (pFont->Invoke(dTimeDelta, 0.1) && 5 > iter.iIdx) {
				_uint iDigit = _wtoi(pFont->szText);			
				if (0 == iDigit)
					__super::Set_FontCnt(pFont, iDigit, false); // default 0
				if (iDigit < iter.iDigits[iter.iIdx]) {
					__super::Set_FontCnt(pFont, iDigit + 1, false);
				}
				else {
					++iter.iIdx;
				}
			}
			
			//__super::Set_FontCnt(&iter.sFont, iter.iScore++, false);
		}
	}
	return OBJ_NOEVENT;
}

void CUI_End_Panel_Second::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	//Update_Edit_Font(&m_sHuds[4].sFont);		
	//for (auto& iter : m_sHuds[0].sFonts) {
	//	Update_Edit_Font(&iter);
	//}

	Update_Edit_Images();
}

void CUI_End_Panel_Second::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_End_Panel_Second::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf || !m_bRender)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (_uint i = 0; i < static_cast<_uint>(ETexType::ICON); ++i) {
		SImageDesc* pImage = &m_vecImages[i];
		if (pImage->IsContinue())
			continue;
		// last alpha
		if (i == static_cast<_uint>(ETexType::PANEL3) /*&& (0.f < pImage->vColor.w)*/) {
			m_fBtnAlpha = pImage->vColor.w;
		}
			
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &pImage->vColor, sizeof(_float4));
		hr |= pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pImage->iPass);
#ifdef _DEBUG
		if (pImage->bCollider)
			hr |= pImage->pCollider->Render();
#endif // _DEBUG
	}

	if (0.f <= m_fBtnAlpha) {
		_uint iIdx = 0;
		for (auto& iter : m_sHuds) {
			if (nullptr != iter.pIconImg) {
				iter.pIconImg->vColor.w = m_fBtnAlpha;
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pIconImg->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_vColor", &iter.pIconImg->vColor, sizeof(_float4));
				hr |= iter.pIconImg->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				hr |= m_pVBRect->Render(m_pShader, iter.pIconImg->iPass);
#ifdef _DEBUG
				if (iter.pIconImg->bCollider)
					hr |= iter.pIconImg->pCollider->Render();
#endif // _DEBUG
			}
			iter.sFont.vColor.w = m_fBtnAlpha;

			//hr |= Render_Font(&iter.sFont);
			for (auto& value : iter.sFonts) {
				if (!value.bRender)
					continue;
				hr |= Render_Font(&value);
			}

			++iIdx;
		}
	}


	return hr;
}

void CUI_End_Panel_Second::Set_FontCnt(const EScoreType& eScore, const _int& iCnt, const _bool& bAcc) {
	__super::Set_FontCnt(&m_sHuds[static_cast<_uint>(eScore)].sFont, iCnt, bAcc);
}

void CUI_End_Panel_Second::Set_FontStr(const EScoreType& eScore, const _tchar* pText, const _bool& bAcc) {
	__super::Set_FontStr(&m_sHuds[static_cast<_uint>(eScore)].sFont, pText, bAcc);
}

void CUI_End_Panel_Second::Calculate_Score(_uint* const& pDigit, const _uint& iScore, _bool* const& bRenders) {
	//_bool bRenders[4] = { true, };
	_uint iQuot = 0, iRemain = 0;

	iQuot = iScore / 10000;
	iRemain = iScore % 10000;
	pDigit[4] = iQuot;
	if (0 == iQuot)
		bRenders[4] = false;

	iQuot = iRemain / 1000;
	iRemain = iRemain % 1000;
	pDigit[3] = iQuot;
	if (0 == iQuot && !bRenders[4])
		bRenders[3] = false;

	iQuot = iRemain / 100;
	iRemain = iRemain % 100;
	pDigit[2] = iQuot;
	if (0 == iQuot && !bRenders[3])
		bRenders[2] = false;

	iQuot = iRemain / 10;
	iRemain = iRemain % 10;
	pDigit[1] = iQuot;
	if (0 == iQuot && !bRenders[2])
		bRenders[1] = false;

	iQuot = iRemain / 1;
	pDigit[0] = iQuot;
}

const HRESULT CUI_End_Panel_Second::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	Save_AddData(L"UI_End_Panel_Second");
	return S_OK;
}

void CUI_End_Panel_Second::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_pRectTransform->Set_Size(1920.f, 1080.f);

	Load_AddData(L"UI_End_Panel_Second", 8);
	for (_uint i = 0; i < m_vecImages.size(); ++i) {
		if (i != 1) {
			m_vecImages[i].pRectTransform->Set_Size(1920.f, 1080.f);
		}		
		else {
			//_float fRatio = (1.f / 1920.f) * _float(g_iWinSizeX);
			//_float2 vSize = _float2(m_vecImages[i].pRectTransform->Get_Desc().fWidth * fRatio, m_vecImages[i].pRectTransform->Get_Desc().fHeight * fRatio);
			m_vecImages[i].pRectTransform->Set_Size();
		}
	}
}
