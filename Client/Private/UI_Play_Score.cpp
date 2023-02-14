#include "pch.h"
#include "UI_Play_Score.h"

CUI_Play_Score* const CUI_Play_Score::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Score* pInstnace = new CUI_Play_Score(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Score::Clone(void* const& pArg) {
	CUI_Play_Score* pInstnace = new CUI_Play_Score(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Score::CUI_Play_Score(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {

}

CUI_Play_Score::CUI_Play_Score(const CUI_Play_Score& rhs)
	: CUI_Image(rhs) {

}

void CUI_Play_Score::Delete(void) {
	__super::Delete();
#ifdef _DEBUG
	//this->Delete_AddData(L"UI_Play_Icon_Passive");
#endif // _DEBUG

}

const HRESULT CUI_Play_Score::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Play_Score::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone();

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr;
	CInfo* pInfo = CInfo::Get_Instance();
	m_iType = 0;
	if (pInfo->Get_Player()) {
		pImage = Add_Image(L"Brutality_Catch", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Brutality_Destory", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Brutality_Gene", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Brutality_Hit", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Deviousness_Hatch_Closed", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Deviousness_Haunting", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Deviousness_Precise_Shot", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Hunter_Catch", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Sacrifice_Entity_Summoned", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Sacrifice_Hooked", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Sacrifice_Sacrifice", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Sacrifice_TimesUp", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
	}
	else {
		pImage = Add_Image(L"Altruism_Good_Skill", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Altruism_Great_Skill", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Altruism_Heal", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Altruism_Hook_Rescue", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Boldness_Good_Skill", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Boldness_Great_Skill", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Boldness_Killer_Blind", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Boldness_Pallet_Drop", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Boldness_Totem", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Boldness_Vaulting", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Boldness_Wiggle", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Objectives_Chest_Search", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Objectives_Good_Skill", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Objectives_Great_Skill", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Objectives_Hatch_Escape", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Objectives_Repairs", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Objectives_Unlocking", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Survival_Heal", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Survival_Hook_Escape", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Survival_Killer_Escape", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
		pImage = Add_Image(L"Survival_Survived", static_cast<_uint>(m_iType++), false, CRectTransform::EEdge::RCOR);
	}

	this->Sort_Images();
	//이미지 비율잡기
	for (auto& iter : m_vecImages) {
		iter.pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.35f, 0.35f, 0.f), false);
	}

	//y -= 80.f

	return hr;
}

const _ubyte CUI_Play_Score::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//if (!m_bStart) {
	//	// set parent
	//	CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
	//	this->Set_Parent(pParent, false);
	//	this->Set_Active(false);
	//	m_bStart = true;
	//}

	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	/*if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_T))
		Add_Score(0);
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_Y))
		Add_Score(1);
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_U))
		Add_Score(2);
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_I))
		Add_Score(3);
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_O))
		Add_Score(4);
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_P))
		Add_Score(5);*/

	if (m_bStartErase) {
		m_dEraseTime += dTimeDelta;
		if (m_dEraseTime >= 0.5) {
			for (_uint i = 0; i < 6; ++i) {
				if (m_iScorePos[i] != 9999)
					m_vecImages[m_iScorePos[i]].vColor.w -= (_float)dTimeDelta;
			}
		}
		if (m_dEraseTime >= 1.5) {
			m_dEraseTime = 0.0;
			m_bStartErase = false;
			/*for (_uint i = 0; i < 6; ++i) {
				if (m_iScorePos[i] != 9999)
					m_vecImages[m_iScorePos[i]].vColor.w = 1.f;
			}*/
			for (auto& iter : m_vecImages) {
				iter.bRender = false;
				iter.vColor.w = 1.f;
			}

			for (auto& iter2 : m_iScorePos)
				iter2 = 9999;
		}
	}
	//m_vecImages[0].vColor.w += (dTimeDelta);

	__super::Update(dTimeDelta);
	return OBJ_NOEVENT;
}

void CUI_Play_Score::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	ImGui::InputInt4("test", (_int*)&m_iScorePos);

	Update_Edit_Images();
}

void CUI_Play_Score::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Play_Score::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

#ifdef _DEBUG

	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player())	//slasher
	{
		wsprintf(m_szOneScore, TEXT("OneScore : %d"), m_iBrutality);
		wsprintf(m_szTwoScore, TEXT("TwoScore : %d"), m_iDeviousness);
		wsprintf(m_szThreeScore, TEXT("ThreeScore : %d"), m_iHunter);
		wsprintf(m_szFourScore, TEXT("FourScore : %d"), m_iSacrifice);
	}
	else
	{
		wsprintf(m_szOneScore, TEXT("OneScore : %d"), m_iAltruismScore);
		wsprintf(m_szTwoScore, TEXT("TwoScore : %d"), m_iBlodnessScore);
		wsprintf(m_szThreeScore, TEXT("ThreeScore : %d"), m_iObjectiveScore);
		wsprintf(m_szFourScore, TEXT("FourScore : %d"), m_iSurvivalScore);
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	pGame_Instance->Render_Font(TEXT("Gothic10"), m_szOneScore, _float2(5.f, 30.f), XMVectorSet(20.f, 20.f, 1.f, 1.f));
	pGame_Instance->Render_Font(TEXT("Gothic10"), m_szTwoScore, _float2(5.f, 50.f), XMVectorSet(20.f, 20.f, 1.f, 1.f));
	pGame_Instance->Render_Font(TEXT("Gothic10"), m_szThreeScore, _float2(5.f, 70.f), XMVectorSet(20.f, 20.f, 1.f, 1.f));
	pGame_Instance->Render_Font(TEXT("Gothic10"), m_szFourScore, _float2(5.f, 90.f), XMVectorSet(20.f, 20.f, 1.f, 1.f));
#endif // _DEBUG

	for (auto& iter : m_vecImages) {
		if (iter.IsContinue())
			continue;

		if (m_bStartErase) {
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
			hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, iter.iPass);
//#ifdef DEBUG
//			if (iter.bCollider)
//				iter.pCollider->Render();
//#endif // DEBUG


		}

		//CInfo* pInfo = CInfo::Get_Instance();
	}

	return hr;
}

void CUI_Play_Score::Add_Score(_uint iScoreType) {
	m_dEraseTime = 0.0;
	_uint iIndex = 6;
	_uint iType = iScoreType;
	_bool bOneTime = false;
	for (_uint j = 0; j < 6; ++j)
	{
		if (m_iScorePos[j] == 9999) {
			iIndex = j;
			bOneTime = true;
			break;
		}
	}
	_uint iTemp[7]{};
	if (iIndex < 6)
	{
		for (_uint k = 4; k >= 0;)
		{
			if (k == 0)
			{
				iTemp[k] = m_iScorePos[k];
				m_iScorePos[k + 1] = iTemp[k];
				m_iScorePos[k] = iScoreType;
				break;
			}
			else {
				iTemp[k] = m_iScorePos[k];
				m_iScorePos[k + 1] = iTemp[k];
				m_iScorePos[k] = 9999;
				--k;
			}
		}
	}

	_float3 fRectPos{};
	_float3 fRectNextPos{};

	for (_uint i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0:
			fRectPos = _float3(-50.f, -300.f, 0.f);
			break;
		case 1:
			fRectPos = _float3(-50.f, -220.f, 0.f);
			break;
		case 2:
			fRectPos = _float3(-50.f, -140.f, 0.f);
			break;
		case 3:
			fRectPos = _float3(-50.f, -60.f, 0.f);
			break;
		case 4:
			fRectPos = _float3(-50.f, 20.f, 0.f);
			break;
		case 5:
			fRectPos = _float3(-50.f, 100.f, 0.f);
			break;
		}

		if (m_iScorePos[i] != 9999)
		{
			m_vecImages[m_iScorePos[i]].pRectTransform->Set_State(CRectTransform::EState::POS, fRectPos, false);
			m_vecImages[m_iScorePos[i]].bRender = true;
			m_bStartErase = true;
		}
	}

	/*if (iIndex == 0)
	{
		m_iScorePos[0] = iScoreType;
		m_vecImages[m_iScorePos[0]].pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-50.f, -300.f, 0.f), false);
		m_vecImages[m_iScorePos[0]].bRender = true;
		m_bStartErase = true;
		return;
	}*/
}

void CUI_Play_Score::Plus_SlasherScore(_uint iScoreNum, _uint iScore) {
	switch (iScoreNum)
	{
	case 0:
		m_iBrutality += iScore;
		break;
	case 1:
		m_iDeviousness += iScore;
		break;
	case 2:
		m_iHunter += iScore;
		break;
	case 3:
		m_iSacrifice += iScore;
		break;
	}
	m_iSumScore = (m_iBrutality + m_iDeviousness + m_iHunter + m_iSacrifice);
}

void CUI_Play_Score::Plus_CamperScore(_uint iScoreNum, _uint iScore) {
	switch (iScoreNum)
	{
	case 0:
		m_iAltruismScore += iScore;
		break;
	case 1:
		m_iBlodnessScore += iScore;
		break;
	case 2:
		m_iObjectiveScore += iScore;
		break;
	case 3:
		m_iSurvivalScore += iScore;
		break;
	}
	m_iSumScore = (m_iAltruismScore + m_iBlodnessScore + m_iObjectiveScore + m_iSurvivalScore);
}

_uint CUI_Play_Score::Get_SlasherScore(_uint iScoreNum) {
	switch (iScoreNum)
	{
	case 0:
		return m_iBrutality;
	case 1:
		return m_iDeviousness;
	case 2:
		return m_iHunter;
	case 3:
		return m_iSacrifice;
	default:
		return 9999;
	}
}

_uint CUI_Play_Score::Get_CamperScore(_uint iScoreNum) {
	switch (iScoreNum)
	{
	case 0:
		return m_iBlodnessScore;
	case 1:
		return m_iObjectiveScore;
	case 2:
		return m_iSurvivalScore;
	case 3:
		return m_iAltruismScore;
	default:
		return 9999;
	}
}

_uint* const CUI_Play_Score::Get_SlasherScorePtr(_uint iScoreNum) {
	switch (iScoreNum)
	{
	case 0:
		return &m_iBrutality;
	case 1:
		return &m_iDeviousness;
	case 2:
		return &m_iHunter;
	case 3:
		return &m_iSacrifice;
	default:
		return nullptr;
	}
}

_uint* const CUI_Play_Score::Get_CamperScorePtr(_uint iScoreNum) {
	switch (iScoreNum)
	{
	case 0:
		return &m_iBlodnessScore;
	case 1:
		return &m_iObjectiveScore;
	case 2:
		return &m_iSurvivalScore;
	case 3:
		return &m_iAltruismScore;
	default:
		return nullptr;
	}
}

//const HRESULT CUI_Play_Score::Save_Data(Json::Value& root) {
//	__super::Save_Data(root);
//	//this->Save_AddData(L"UI_Play_Score");
//	return S_OK;
//}
//
//void CUI_Play_Score::Load_Data(Json::Value& root) {
//	__super::Load_Data(root);
//	//this->Load_AddData(L"UI_Play_Score");
//}
