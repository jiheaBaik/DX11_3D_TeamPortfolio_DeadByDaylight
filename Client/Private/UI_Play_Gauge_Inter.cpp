#include "pch.h"
#include "UI_Play_Gauge_Inter.h"
#include "UI_Play_Icon_Hud.h"

CUI_Play_Gauge_Inter* const CUI_Play_Gauge_Inter::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Gauge_Inter* pInstnace = new CUI_Play_Gauge_Inter(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Gauge_Inter::Clone(void* const& pArg) {
	CUI_Play_Gauge_Inter* pInstnace = new CUI_Play_Gauge_Inter(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Gauge_Inter::CUI_Play_Gauge_Inter(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Gauge_Inter::CUI_Play_Gauge_Inter(const CUI_Play_Gauge_Inter& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Gauge_Inter::Delete(void) {
	__super::Delete();
	for (auto& iter : m_pFilledTexs) 
		Safe_Release(iter);
}

const HRESULT CUI_Play_Gauge_Inter::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Play_Gauge_Inter::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(/*L"play_gauge_inter_out"*/);
	m_iType = 0;
	// set filled
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pFilledTexs[0] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_gauge_inter_gray").c_str()));
	m_pFilledTexs[1] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_gauge_inter_yellow").c_str()));
	m_pFilledTexs[2] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_gauge_inter_red").c_str()));

	return hr;
}

const _ubyte CUI_Play_Gauge_Inter::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		// set parent
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
		this->Set_Parent(pParent, false);
		// init hud
		CInfo* pInfo = CInfo::Get_Instance();
		m_pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Icon_Hud"));
		if (pInfo->Get_Player()) {
			m_sHuds[0].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ESlasher::SONSANG));
			m_sHuds[1].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ESlasher::HANG));
			m_sHuds[2].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ESlasher::DESTROY));
			if (pInfo->Get_Slasher() == CInfo::ESlasher::SPIRIT) {
				m_sHuds[3].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ESlasher::PASSIVE1));
			}
		}
		else {
			m_sHuds[0].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::REPAIR));
			m_sHuds[1].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::UNLOCK));
			m_sHuds[2].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::TOTEM));
			m_sHuds[3].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::HEALSEL));		
			m_sHuds[4].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::HEALOTH));
			m_sHuds[5].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::RESCUE));
			m_sHuds[6].pHudImage = m_pHud->Get_HudImage(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::ESCAPE));
		}
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;
	if (!m_bPreActive) {
/*		for (auto& iter : m_sHuds) {
			if (nullptr == iter.pHudImage)
				continue;
			iter.bRender = false;
		}		*/	
	}
	return OBJ_NOEVENT;
}

void CUI_Play_Gauge_Inter::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	_uint i = 0;
	for (auto& iter : m_sHuds) {
		_char szLabel[100] = "";
		_char szBuf[8] = "";
		_itoa_s(i++, szBuf, 10);
		if (nullptr == iter.pHudImage)
			continue;
		IM_LABELING(szLabel, "render", szBuf);
		ImGui::Checkbox(szLabel, &iter.bRender);
		ImGui::SameLine();
		IM_LABELING(szLabel, "inter", szBuf);
		ImGui::Checkbox(szLabel, &iter.bInter);
		ImGui::SameLine();
		IM_LABELING(szLabel, "gauge", szBuf);
		ImGui::InputFloat(szLabel, &iter.fGauge);
	}

	Update_Edit_Images();
}	

void CUI_Play_Gauge_Inter::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Play_Gauge_Inter::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (m_bStart)
		m_bStart = false;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_sHuds) {
		if (!iter.bRender)
			continue;
		for (auto& value : m_vecImages) {
			// check inter
			_bool bGauge = true;
			if (value.iDepth == static_cast<_uint>(ETexType::ICON)) {
				value.bRender = iter.bInter;
				bGauge = false;
			}
			if (value.IsContinue())
				continue;
			// set alpha
			(!iter.bInter) ? (value.vColor.w = 0.6f) : (value.vColor.w = 0.8f);
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &value.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &value.vColor, sizeof(_float4));
			hr |= value.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, value.iPass);

			if (!bGauge || nullptr == iter.pHudImage)
				continue;
			// draw hud
			_float4x4 f4World = iter.pHudImage->pRectTransform->Get_Float4x4();
			_vector vPos = XMLoadFloat3(&value.pRectTransform->Get_Desc().vPosition) - XMVectorSet(value.pRectTransform->Get_DestSizeF3().x * 0.45f, 380.f, 0.f, 0.f);
			//_vector vPos = XMLoadFloat3((_float3*)&f4World.m[3][0]) - XMVectorSet(value.pRectTransform->Get_DestSizeF3().x * 0.5f, 140.f * m_fAtt, 0.f, 0.f);
			XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
			_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World) * XMMatrixScaling(0.7f, 0.7f, 0.7f));
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &iter.pHudImage->vColor, sizeof(_float4));
			hr |= iter.pHudImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, iter.pHudImage->iPass);

			if (0 > iter.iGauge)
				continue;
			// draw fx
			f4World = value.pRectTransform->Get_Float4x4();
			vPos = XMLoadFloat3((_float3*)&f4World.m[3][0]) + XMVectorSet(-10.f, 3.f, 0.f, 0.f);
			XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
			matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World) * XMMatrixScaling(0.9f, 1.f, 0.9f));
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &value.vColor, sizeof(_float4));
			_float fScroll = iter.fGauge * (1.0f / 100.f);
			hr |= m_pShader->Set_RawValue("g_fScrollX", &fScroll, sizeof(_float));
			hr |= m_pFilledTexs[iter.iGauge]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_DECREASE));
		}
		//iter.bRender = false;
	}

	// reset
	//for (auto& iter : m_sHuds) {
	//	iter.bRender = false;
	//}
	return hr;
}

void CUI_Play_Gauge_Inter::Set_Gauge(const EInter& eInter, const _float& fGauge, const _float& fAccel, const _bool& bAcc) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	SHud* pHud = &m_sHuds[iIdx];

	if (!pHud->bRender)
		return;
	_uint iQuot = (_uint)fAccel / 1;
	for (_uint i = 0; i < 3; ++i) {
		if (i == iQuot - 1) {
			pHud->iGauge = i;
			break;
		}
	}
	(!bAcc) ? (pHud->fGauge = fGauge) : (pHud->fGauge += (fGauge * fAccel));
	if (100.f <= pHud->fGauge || 100.f <= fGauge)
		pHud->Reset();
}

void CUI_Play_Gauge_Inter::Set_Gauge(const EInter& eInter, const _float& fGauge, const _bool& bReset) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	SHud* pHud = &m_sHuds[iIdx];
	if (!pHud->bRender)
		return;
	pHud->iGauge = 0;
	pHud->fGauge = fGauge;
	//(!bAcc) ? (pHud->fGauge = fGauge) : (pHud->fGauge += (fGauge * fAccel));
	if (bReset) {
		if (100.f <= pHud->fGauge || 100.f <= fGauge)
			pHud->Reset();
	}
}

void CUI_Play_Gauge_Inter::End_Gauge(const EInter& eInter) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	m_sHuds[iIdx].Reset();
}

// first gauge, end then icon
const _bool CUI_Play_Gauge_Inter::Get_Render(const EInter& eInter, const _bool& bInter) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return false;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	_bool bRender = false;
	(!bInter) ? (bRender = m_sHuds[iIdx].bRender) : (bRender = m_sHuds[iIdx].bInter);
	return bRender;
}

const _bool CUI_Play_Gauge_Inter::Get_Inter(const EInter& eInter) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return false;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	return m_sHuds[iIdx].bInter;
}

const _float CUI_Play_Gauge_Inter::Get_Gauge(const EInter& eInter) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return false;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	return m_sHuds[iIdx].fGauge;
}

void CUI_Play_Gauge_Inter::Set_Render(const EInter& eInter, const _bool& bRender, const _bool& bInter) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	m_sHuds[iIdx].bRender = bRender;
	m_sHuds[iIdx].bInter = bInter;
}

void CUI_Play_Gauge_Inter::Set_RenderAll(const _bool& bRender, const _bool& bInter) {
	for (auto& iter : m_sHuds) {
		iter.bRender = bRender;
		iter.bInter = bInter;
	}
}

void CUI_Play_Gauge_Inter::Set_Inter(const EInter& eInter, const _bool& bInter) {
	if (EInter::NONE == eInter || EInter::END <= eInter)
		return;
	_uint iIdx = (EInter::NONE < eInter) ? (static_cast<_uint>(eInter) - 1) : (abs(static_cast<_int>(eInter)) - 1);
	m_sHuds[iIdx].bInter = bInter;
}

void CUI_Play_Gauge_Inter::Set_InterAll(const _bool& bInter) {
	for (auto& iter : m_sHuds)
		iter.bInter = bInter;
}

const HRESULT CUI_Play_Gauge_Inter::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	//Save_AddData(L"UI_Play_Gauge_Inter");
	return S_OK;
}

void CUI_Play_Gauge_Inter::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	this->Set_Active(true);
	//Load_AddData(L"UI_Play_Gauge_Inter");

	//CInfo* pInfo = CInfo::Get_Instance();
	// only camper
	//if (pInfo->Get_Player())
	//	return;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr;
	// back
	pImage = Add_Image(L"play_gauge_inter_out", static_cast<_int>(ETexType::BACK), true, CRectTransform::EEdge::CBOT, false);
	pImage->pRectTransform->Set_Desc(m_pRectTransform->Get_Desc());
	CRectTransform* pParent = pImage->pRectTransform;
	// icon
	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player() && pInfo->Get_Slasher() == CInfo::ESlasher::SPIRIT) {
		pImage = Add_Image(L"play_skill_spirit", static_cast<_int>(ETexType::ICON), false, CRectTransform::EEdge::LCOR, false);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-50.f, -10.f, 0.f));
	}
	else {
		pImage = Add_Image(L"play_icon_hub_hand", static_cast<_int>(ETexType::ICON), false, CRectTransform::EEdge::LCOR, false);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-30.f, -10.f, 0.f));
	}
	pImage->pRectTransform->Set_Parent(pParent, false);
	pImage->pRectTransform->Set_Size(40.f, 50.f);
}
