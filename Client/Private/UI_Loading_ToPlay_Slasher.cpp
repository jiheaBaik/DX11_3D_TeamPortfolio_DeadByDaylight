#include "pch.h"
#include "UI_Loading_ToPlay_Slasher.h"

CUI_Loading_ToPlay_Slasher* const CUI_Loading_ToPlay_Slasher::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Loading_ToPlay_Slasher* pInstnace = new CUI_Loading_ToPlay_Slasher(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Loading_ToPlay_Slasher::Clone(void* const& pArg) {
	CUI_Loading_ToPlay_Slasher* pInstnace = new CUI_Loading_ToPlay_Slasher(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Loading_ToPlay_Slasher::CUI_Loading_ToPlay_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Loading_ToPlay_Slasher::CUI_Loading_ToPlay_Slasher(const CUI_Loading_ToPlay_Slasher& rhs)
	: CUI_Image(rhs) {
}

void CUI_Loading_ToPlay_Slasher::Delete(void) {
	__super::Delete();
	m_vecTips.clear();
}

const HRESULT CUI_Loading_ToPlay_Slasher::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Loading_ToPlay_Slasher::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone();
	__super::Add_Component(static_cast<_ubyte>(EScene::STATIC), Get_TextureKey(L"back").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));	
	//m_pRectTransform->Set_Size((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	m_pRectTransform->Set_Size(1920.f, 1080.f);
	m_iPass = 1;
	m_iDepth = 50;

	//memcpy(&m_iType, pArg, sizeof(_int));
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr; 
	m_vecImages.reserve(10);
	m_vecTips.reserve(6);
	// image
	pImage = this->Add_Image(L"tim_camper1", ++m_iType, _float3(330.f, 150.f, 0.f), CRectTransform::EEdge::LTOP);
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.7f, 0.7f, 0.7f));
	pImage = this->Add_Image(L"tim_camper2", m_iType++, _float3(0.f, 150.f, 0.f), CRectTransform::EEdge::CTOP);
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.7f, 0.7f, 0.7f));
	pImage = this->Add_Image(L"tim_camper3", m_iType++, _float3(-330.f, 150.f, 0.f), CRectTransform::EEdge::RTOP);
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.7f, 0.7f, 0.7f));

	// gauge
	//pImage = this->Add_Image(L"loading_gauge_back", m_iType++, _float3(0.f, -40.f, 0.f), CRectTransform::EEdge::CBOT);
	//pImage = this->Add_Image(L"loading_gauge_in", m_iType++, _float3(0.f, -28.f, 0.f), CRectTransform::EEdge::CBOT);
	//pImage->iPass = static_cast<_uint>(EUIPass::PASS_DECREASE);
	// text
	pImage = this->Add_Image(L"txt_loading", m_iType++, _float3(-40.f, -30.f, 0.f), CRectTransform::EEdge::RBOT);
	pImage->vColor = { 0.6f, 0.6f, 0.6f, 0.8f };

	m_iNumImages = static_cast<_uint>(m_vecImages.size());
	// tip
	for (_uint i = 0; i < 6; ++i) {
		_tchar szKey[100] = L"tip_sla";
		_tchar szBuf[8] = L"";
		_itow_s(i + 1, szBuf, 10);
		lstrcat(szKey, szBuf);
		pImage = this->Add_Image(szKey, m_iType++, _float3(0.f, -100.f, 0.f), CRectTransform::EEdge::CBOT);
		pImage->bRender = false;
		pImage->vColor.w = 0.f;
		m_vecTips.emplace_back(pImage);
	}
	return hr;
}

const _ubyte CUI_Loading_ToPlay_Slasher::Update(const _double& dTimeDelta) {
	__super::Update(dTimeDelta);

	//this->Set_Fade(true, 0.f, 1.f, (_float)dTimeDelta * .5f, false);
	//m_fGauge += (_float)dTimeDelta * .5f;
	//if (100.f <= m_fGauge)
	//	m_fGauge = 100.f;

	SImageDesc* pTip = m_vecTips[m_iTip];
	if (!pTip->bRender && 0 == pTip->vColor.w && this->Invoke(dTimeDelta, 3.0)) {
		m_iTip = rand() % (_int)m_vecTips.size();
		_uint i = 0;
		for (auto& iter : m_vecTips) {
			if (m_iTip == i++) {
				iter->bRender = true;
				iter->vColor.w = 0.f;
			}
			else {
				iter->bRender = false;
				iter->vColor.w = 1.f;
			}
		}
	}

	if (pTip->bRender) {
		pTip->vColor.w += static_cast<_float>(dTimeDelta) * 0.5f;
		if (1.f <= pTip->vColor.w) {
			pTip->vColor.w = 1.f;
			if (this->Invoke(dTimeDelta, 3.0))
				pTip->bRender = false;
		}		
	}
	else {
		pTip->vColor.w -= static_cast<_float>(dTimeDelta) * 0.5f;
		if (0.f >= pTip->vColor.w) {
			pTip->vColor.w = 0.f;
		}
	}

	return OBJ_NOEVENT;
}

void CUI_Loading_ToPlay_Slasher::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();
	__super::Update_Edit_Images();
}

void CUI_Loading_ToPlay_Slasher::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Loading_ToPlay_Slasher::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	hr |= __super::Set_RawValue(m_pShader);
	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &m_pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
	hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
	hr |= m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	hr |= m_pVBRect->Render(m_pShader, m_iPass);


	for (_uint i = 0; i < m_iNumImages; ++i) {
		SImageDesc* pImage = &m_vecImages[i];
		if (pImage->IsContinue())
			continue;
		//if (pImage->iPass == static_cast<_uint>(EUIPass::PASS_DECREASE)) {
		//	_float fScroll = m_fGauge * (1.0f / 100.f);
		//	hr |= m_pShader->Set_RawValue("g_fScrollX", &fScroll, sizeof(_float));
		//}

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &pImage->vColor, sizeof(_float4));
		hr |= pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pImage->iPass);
#ifdef _DEBUG
		if (pImage->bCollider)
			pImage->pCollider->Render();
#endif // _DEBUG
	}

	SImageDesc* pTip = m_vecTips[m_iTip];
	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pTip->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
	hr |= m_pShader->Set_RawValue("g_vColor", &pTip->vColor, sizeof(_float4));
	hr |= pTip->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	hr |= m_pVBRect->Render(m_pShader, pTip->iPass);

	return hr;
}

CUI_Image::SImageDesc* const CUI_Loading_ToPlay_Slasher::Add_Image(const _tchar* pTexKey, const _int& iDepth, const _float3& vPos, const CRectTransform::EEdge& eEdge, const _bool& bSizeOfTex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc sImage;
	sImage.Reset();
	sImage.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
	sImage.pRectTransform->Set_Edge(eEdge);
	sImage.pRectTransform->Set_State(CRectTransform::EState::POS, vPos);
	sImage.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
	sImage.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), this->Get_TextureKey(pTexKey).c_str()));
	lstrcpy(sImage.szTexture, this->Get_TextureKey(pTexKey).c_str());
	//CONVERT_WIDE_TO_MULTI(this->Get_TextureKey(pTexKey).c_str(), sImage.szTexKey);
	if (true == (sImage.bSizeOfTex = bSizeOfTex))
		this->Set_SizeOfTexture(sImage.pTexture, sImage.pRectTransform);
	sImage.bSizeOfTex = bSizeOfTex;
	sImage.iDepth = iDepth;
	m_vecImages.emplace_back(sImage);
	return &m_vecImages.back();
}


