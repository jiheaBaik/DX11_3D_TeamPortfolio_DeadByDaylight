#include "pch.h"
#include "UI_Play_Icon_Propile.h"
#include "Camper_GamePlay.h"
#include "FM_Camper.h"

CUI_Play_Icon_Propile* const CUI_Play_Icon_Propile::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Icon_Propile* pInstnace = new CUI_Play_Icon_Propile(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Icon_Propile::Clone(void* const& pArg) {
	CUI_Play_Icon_Propile* pInstnace = new CUI_Play_Icon_Propile(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Icon_Propile::CUI_Play_Icon_Propile(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Icon_Propile::CUI_Play_Icon_Propile(const CUI_Play_Icon_Propile& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Icon_Propile::Delete(void) {
	__super::Delete();
#ifdef _DEBUG
	this->Delete_AddData(L"UI_Play_Icon_Propile");
#endif // _DEBUG
	for (auto& iter : m_sProfiles) {
		Safe_Release(iter.pTexture);
		Safe_Release(iter.pRectTransform);
	}
	for (auto& iter : m_pHpTextures)
		Safe_Release(iter);
}

const HRESULT CUI_Play_Icon_Propile::Init_Create(void) {
	HRESULT hr = S_OK;
	// 캐릭터 아이콘, ㅣㄹ시간 상태 연결
	// 아이콘 크기랑 위치 맞추기
	return hr;
}

const HRESULT CUI_Play_Icon_Propile::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(L"icon_circle3");

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_sProfiles[0].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"pro_pro_dwi").c_str()));
	m_sProfiles[1].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"pro_pro_meg").c_str()));
	m_sProfiles[2].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"pro_pro_bill").c_str()));
	m_sProfiles[3].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"pro_pro_cheryl").c_str()));

	m_pHpTextures[0] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_pro_gaugein2").c_str()));
	m_pHpTextures[1] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_pro_gaugefx").c_str()));
	return hr;
}

const _ubyte CUI_Play_Icon_Propile::Update(const _double& dTimeDelta) {
	if (!m_bStart) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
		this->Set_Parent(pParent, false);
		this->Set_Active(false);
		m_bStart = true;
	}
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	for (auto& iter : m_sProfiles) {
		if (iter.bState &&  0 < iter.iState) {
			SHudFX* pHud = &m_sHuds[iter.iState - 1];
			pHud->fAlpha += static_cast<_float>(dTimeDelta) /** 0.5f*/;
			if (1.f <= pHud->fAlpha)
				pHud->fAlpha = 1.f;

			//m_sProfiles->pGaugeImg->vColor.w = pHud->fAlpha;
			//m_sProfiles->pHookImg[0]->vColor.w = pHud->fAlpha;
			//m_sProfiles->pHookImg[1]->vColor.w = pHud->fAlpha;
		}
	}
	return OBJ_NOEVENT;
}

void CUI_Play_Icon_Propile::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	for (auto& iter : m_sHuds) {
		ImGui::InputFloat("color2", &iter.fAlpha);
	}

	static _int iTemp = 0;
	if (ImGui::Button("bind") && 0 <= iTemp) {
		m_sProfiles[0].iState = static_cast<_int>(iTemp);
	}
	ImGui::SameLine();
	ImGui::InputInt("state", &iTemp);

	Update_Edit_Images();
}

void CUI_Play_Icon_Propile::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
	for (auto& iter : m_sProfiles) {
		if (nullptr != iter.pRectTransform)
			iter.pRectTransform->Update_Desc();
	}

}

const HRESULT CUI_Play_Icon_Propile::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf/*|| 0 > m_iType || !m_sHuds[m_iType].bRedner*/)
		return hr;
	hr |= __super::Set_RawValue(m_pShader);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	_uint i = 0;
	list<CObject*>* plistCamper = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Camper_GamePlay"));
	if (nullptr != plistCamper) {
		for (auto& iter : *plistCamper) {
			//back LB
			//_float4x4 f4World = m_pRectTransform->Get_Float4x4();
			//_vector vPos = XMLoadFloat3((_float3*)&f4World.m[3][0]) - XMVectorSet(0.f, XMVectorGetY(m_pRectTransform->Get_DestSize()) * i - 10.f, 0.f, 0.f);
			//XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
			//_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
			//hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));

			SProfile* pProfile = &m_sProfiles[i];
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pProfile->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
			hr |= m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, m_iPass);

			CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
			CCamper_GamePlay::ECamperKind eCamperKind = pCamper->Get_CamperKind();

			//한번만 들어와야 하는 곳		
			switch (pCamper->Get_FM_Camper()->Get_State())
			{
			case CFM_Camper::EState::MORI://죽을 때
			case CFM_Camper::EState::ENDGAME:
			case CFM_Camper::EState::SPIDER_KILLOUT:
				pProfile->iState = static_cast<_int>(EState::DEAD);
				break;
			case CFM_Camper::EState::ENDING_RUN://탈출
			case CFM_Camper::EState::HATCH_OUT:
				pProfile->iState = static_cast<_int>(EState::ESCAPE);
				break;
			case CFM_Camper::EState::HOOKIN:
			case CFM_Camper::EState::HOOKOUT:
			case CFM_Camper::EState::HOOKIDLE:
			case CFM_Camper::EState::HOOKFREE:
			case CFM_Camper::EState::HOOKSTRUGGLE:
			case CFM_Camper::EState::HOOKRESCUEIN_BEEING:
			case CFM_Camper::EState::SPIDER_IN:
			case CFM_Camper::EState::SPIDER_LOOP:
			case CFM_Camper::EState::SPIDER_OUT:
			case CFM_Camper::EState::SPIDER_KILLIN:
			case CFM_Camper::EState::SPIDER_KILLLOOP:
			case CFM_Camper::EState::SPIDER_STRUGGLE:
				pProfile->iState = static_cast<_int>(EState::HOOK);
				//pProfile->iHookCnt = pCamper->Get_SpiderRound();
				//pProfile->fSpiderTime = pCamper->Get_SpiderTime(); // spider 
				break;
			default:
				if (1 == pCamper->Get_Hp()) {
					pProfile->iState = static_cast<_int>(EState::HURT1);
				}			
				else if (0 == pCamper->Get_Hp()) {
					pProfile->iState = static_cast<_int>(EState::HURT2);
				}					
				else {
					pProfile->iState = static_cast<_int>(EState::PROPILE);
					pProfile->bState = false;
				}
				//pProfile->fSpiderTime = 0.f;
			}
			_uint iState = pProfile->iState;
			if (0 == iState) { // default
				switch (eCamperKind) {
				case CCamper_GamePlay::ECamperKind::CHERYL:
					hr |= m_sProfiles[3].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					hr |= m_sProfiles[1].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					hr |= m_sProfiles[2].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					hr |= m_sProfiles[0].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				}
				hr |= m_pVBRect->Render(m_pShader, m_iPass);

				//// hook : get fx alpha
				//if (0 < pProfile->iHookCnt) {
				//	for (_uint j = 0; j < 2; ++j) {
				//		_float4x4 f4World = pProfile->pHookImg[j]->pRectTransform->Get_Float4x4();
				//		_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
				//		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
				//		_float4 vColor = { 0.f, 0.f, 0.f, pProfile->pHookImg[0]->vColor.w };
				//		hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
				//		hr |= pProfile->pHookImg[j]->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//		hr |= m_pVBRect->Render(m_pShader, m_iPass);

				//		if (j == pProfile->iHookCnt) {
				//			hr |= m_pShader->Set_RawValue("g_vColor", &pProfile->pHookImg[j]->vColor, sizeof(_float4));
				//			_float fRatio = 1.f;
				//			hr |= m_pShader->Set_RawValue("g_fLoading", &fRatio, sizeof(_float));
				//			hr |= pProfile->pHookImg[j]->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//			hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_LOADING));
				//		}
				//	}
				//}
			}
			else if (1 == iState) { // hurt2
				switch (eCamperKind) {
				case CCamper_GamePlay::ECamperKind::CHERYL:
					hr |= m_sProfiles[3].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				case CCamper_GamePlay::ECamperKind::MEG:
					hr |= m_sProfiles[1].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				case CCamper_GamePlay::ECamperKind::BILL:
					hr |= m_sProfiles[2].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				case CCamper_GamePlay::ECamperKind::DWIGHT:
					hr |= m_sProfiles[0].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
					break;
				}
				hr |= m_pVBRect->Render(m_pShader, m_iPass);

				SHudFX* pHud = &m_sHuds[iState-1];
				if (!pProfile->bState) {
					pHud->fAlpha = 0.f;
					pProfile->bState = true;
				}
				// fx
				pHud->pFXImage->pRectTransform->Set_Parent(pProfile->pRectTransform, false);
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pHud->pFXImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				pHud->pFXImage->vColor.w = pHud->fAlpha;
				hr |= m_pShader->Set_RawValue("g_vColor", &pHud->pFXImage->vColor, sizeof(_float4));
				hr |= pHud->pFXImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				hr |= m_pVBRect->Render(m_pShader, m_iPass);

				//// hook : get fx alpha
				//if (0 < pProfile->iHookCnt) {
				//	for (_uint j = 0; j < 2; ++j) {
				//		_float4x4 f4World = pProfile->pHookImg[j]->pRectTransform->Get_Float4x4();
				//		_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
				//		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
				//		_float4 vColor = { 0.f, 0.f, 0.f, pProfile->pHookImg[0]->vColor.w };
				//		hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
				//		hr |= pProfile->pHookImg[j]->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//		hr |= m_pVBRect->Render(m_pShader, m_iPass);

				//		if (j == pProfile->iHookCnt) {
				//			hr |= m_pShader->Set_RawValue("g_vColor", &pProfile->pHookImg[j]->vColor, sizeof(_float4));
				//			_float fRatio = 1.f;
				//			hr |= m_pShader->Set_RawValue("g_fLoading", &fRatio, sizeof(_float));
				//			hr |= pProfile->pHookImg[j]->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//			hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_LOADING));
				//		}
				//	}
				//}
			}
			else { // oth
				SHudFX* pHud = &m_sHuds[iState-1];
				if (!pProfile->bState) {
					pHud->fAlpha = 0.f;
					pProfile->bState = true;
				}
				// fx
				pHud->pFXImage->pRectTransform->Set_Parent(pProfile->pRectTransform, false);
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pHud->pFXImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				pHud->pFXImage->vColor.w = pHud->fAlpha;
				hr |= m_pShader->Set_RawValue("g_vColor", &pHud->pFXImage->vColor, sizeof(_float4));
				hr |= pHud->pFXImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				hr |= m_pVBRect->Render(m_pShader, m_iPass);
				// icon
				pHud->pImage->pRectTransform->Set_Parent(pProfile->pRectTransform, false);
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pHud->pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
				hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
				hr |= pHud->pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				hr |= m_pVBRect->Render(m_pShader, m_iPass);

				//// hook : get fx alpha
				//if (0 < pProfile->iHookCnt) {
				//	for (_uint j = 0; j < 2; ++j) {
				//		_float4x4 f4World = pProfile->pHookImg[j]->pRectTransform->Get_Float4x4();
				//		_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
				//		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
				//		_float4 vColor = { 0.f, 0.f, 0.f, pProfile->pHookImg[0]->vColor.w };
				//		hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
				//		hr |= pProfile->pHookImg[j]->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//		hr |= m_pVBRect->Render(m_pShader, m_iPass);

				//		if (j == pProfile->iHookCnt) {
				//			hr |= m_pShader->Set_RawValue("g_vColor", &pProfile->pHookImg[j]->vColor, sizeof(_float4));
				//			_float fRatio = 1.f;
				//			hr |= m_pShader->Set_RawValue("g_fLoading", &fRatio, sizeof(_float));
				//			hr |= pProfile->pHookImg[j]->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//			hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_LOADING));
				//		}
				//	}
				//}
				//// gauge : get fx alpha / hook
				//if (0.f < pProfile->fSpiderTime && iState == static_cast<_int>(EState::HOOK)) {
				//	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pProfile->pGaugeImg->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				//	hr |= m_pShader->Set_RawValue("g_vColor", &pProfile->pGaugeImg->vColor, sizeof(_float4));
				//	hr |= pProfile->pGaugeImg->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//	hr |= m_pVBRect->Render(m_pShader, m_iPass);

				//	_float4x4 f4World = pProfile->pGaugeImg->pRectTransform->Get_Float4x4();
				//	_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World)) * XMMatrixTranspose(XMMatrixScaling(0.95f, 0.7f, 0.8f));
				//	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
				//	_float fScroll = pProfile->fSpiderTime * (1.0f / 60.f);
				//	hr |= m_pShader->Set_RawValue("g_fScrollX", &fScroll, sizeof(_float));
				//	hr |= m_pHpTextures[0]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				//	hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_DECREASE));
				//}
			}

			++i;
		}
	}
	return hr;
}

const HRESULT CUI_Play_Icon_Propile::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	Save_AddData(L"UI_Play_Icon_Propile");
	return S_OK;
}

void CUI_Play_Icon_Propile::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_pRectTransform->Update_Desc();

	Load_AddData(L"UI_Play_Icon_Propile", 20);
	//m_vecImages.reserve(static_cast<_uint>(EState::END) * 2);
	//SImageDesc* pImage = nullptr;
	//m_sHuds[0].Init(static_cast<_uint>(EState::HURT1), Add_Image(L"pro_fxpro", static_cast<_uint>(EState::HURT1), false, CRectTransform::EEdge::LBOT));
	//Add_Image(L"pro_fxpro", static_cast<_int>(EState::HURT1) * -1, false, CRectTransform::EEdge::LBOT);
	//m_sHuds[1].Init(static_cast<_uint>(EState::HURT2), Add_Image(L"pro_sta_hurt", static_cast<_uint>(EState::HURT2), false, CRectTransform::EEdge::LBOT));
	//Add_Image(L"pro_fxhurt", static_cast<_int>(EState::HURT2) * -1, false, CRectTransform::EEdge::LBOT);
	//m_sHuds[2].Init(static_cast<_uint>(EState::HOOK), Add_Image(L"pro_sta_hook", static_cast<_uint>(EState::HOOK), false, CRectTransform::EEdge::LBOT));
	//Add_Image(L"pro_fxhook", static_cast<_int>(EState::HOOK) * -1, false, CRectTransform::EEdge::LBOT);
	//m_sHuds[3].Init(static_cast<_uint>(EState::DEAD), Add_Image(L"pro_sta_dead", static_cast<_uint>(EState::DEAD), false, CRectTransform::EEdge::LBOT));
	//Add_Image(L"pro_fxdead", static_cast<_int>(EState::DEAD) * -1, false, CRectTransform::EEdge::LBOT);
	//m_sHuds[4].Init(static_cast<_uint>(EState::ESCAPE), Add_Image(L"pro_sta_esc", static_cast<_uint>(EState::ESCAPE), false, CRectTransform::EEdge::LBOT));
	//Add_Image(L"pro_fxesc", static_cast<_int>(EState::ESCAPE) * -1, false, CRectTransform::EEdge::LBOT);
	//this->Sort_Images();

	for (auto& iter : m_vecImages) {
		if (0 > iter.iDepth) {
			for (_uint i = 5; i > 0; --i) {
				if (iter.iDepth == -1 * i) {
					m_sHuds[i - 1].pFXImage = &iter;
					m_sHuds[i - 1].pFXImage->pRectTransform->Set_Parent(m_pRectTransform, false);
					break;
				}
			}
		}
		else {
			for (_uint i = 0; i < 5; ++i) {
				if (iter.iDepth == i + 1) {
					m_sHuds[i].Init(i + 1, &iter);
					m_sHuds[i].pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
					break;
				}
			}
		}
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	for (_uint i = 0; i < 4; ++i) {
		// profile : lbot 90 -270
		SProfile* pProfile = &m_sProfiles[i];
		pProfile->pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		pProfile->pRectTransform->Set_Desc(m_pRectTransform->Get_Desc());
		_vector vPos = XMLoadFloat3(&m_pRectTransform->Get_Desc().vPosition) + XMVectorSet(0.f, -XMVectorGetY(m_pRectTransform->Get_DestSize()) * i - 10.f, 0.f, 0.f);
		pProfile->pRectTransform->Set_State(CRectTransform::EState::POS, vPos);
		//// gauge
		//pProfile->pGaugeImg = Add_Image(L"play_pro_gaugeback3", static_cast<_uint>(EState::END), true, CRectTransform::EEdge::RBOT);
		//pProfile->pGaugeImg->pRectTransform->Set_Parent(m_sProfiles[i].pRectTransform, false);
		//pProfile->pGaugeImg->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.7f, 0.5f, 0.6f));
		//pProfile->pGaugeImg->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(85.f, -10.f, 0.f));
		//// hook
		//for (_uint j = 0; j < 2; ++j) {
		//	pProfile->pHookImg[j] = Add_Image(L"pro_icon_slash2", static_cast<_uint>(EState::END), true, CRectTransform::EEdge::RCOR);
		//	pProfile->pHookImg[j]->pRectTransform->Set_Parent(m_sProfiles[i].pRectTransform, false);
		//	pProfile->pHookImg[j]->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.3f, 0.3f, 0.3f));
		//	pProfile->pHookImg[j]->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(10.f * (j + 1), 0.f, 0.f));
		//}
	}
}
