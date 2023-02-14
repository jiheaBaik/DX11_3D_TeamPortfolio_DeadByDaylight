#include "pch.h"
#include "UI_Play_Icon_Perk.h"

CUI_Play_Icon_Perk* const CUI_Play_Icon_Perk::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Icon_Perk* pInstnace = new CUI_Play_Icon_Perk(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Icon_Perk::Clone(void* const& pArg) {
	CUI_Play_Icon_Perk* pInstnace = new CUI_Play_Icon_Perk(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Icon_Perk::CUI_Play_Icon_Perk(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Icon_Perk::CUI_Play_Icon_Perk(const CUI_Play_Icon_Perk& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Icon_Perk::Delete(void) {
	__super::Delete();

	for (auto& iter : m_pGaugeTexs)
		Safe_Release(iter);
	for (auto& iter : m_sHuds) 
		Safe_Release(iter.pIconTex);
}

const HRESULT CUI_Play_Icon_Perk::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Play_Icon_Perk::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(/*L"play_icon_perk_back"*/);
	m_iGroup = 1;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pGaugeTexs[0] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_icon_perk_gauge1").c_str()));
	m_pGaugeTexs[1] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_icon_perk_gauge2").c_str()));
	return hr;
}

const _ubyte CUI_Play_Icon_Perk::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
		this->Set_Parent(pParent, false);
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	//if (pGame_Instance->Get_Input_KeyboardKey(DIK_Q)) {
	//	m_sHuds[0].pPerk->Set_Start();
	//}
	return OBJ_NOEVENT;
}

void CUI_Play_Icon_Perk::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();
	ImGui::InputInt("group", &m_iGroup);

	Update_Edit_Images();
}

void CUI_Play_Icon_Perk::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Play_Icon_Perk::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	_int i = -1;
	for (auto& iter : m_sHuds) {
		++i;
		if (nullptr == iter.pImage || iter.pImage->IsContinue() || nullptr == iter.pIconTex)
			continue;
		if (iter.pImage->bCollider)
			iter.pImage->pCollider->Render();

		// back
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		hr |= iter.pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
		
		_float4x4 f4World = iter.pImage->pRectTransform->Get_Float4x4();
		_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
		matWorldTP *= XMMatrixTranspose(XMMatrixScaling(0.95f, 0.95f, 0.95f));
		// icon
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
		hr |= iter.pIconTex->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);

		// use perk?
		if (!iter.pPerk->Get_IsUsePerk())
			continue;

		if (9999 != iter.pPerk->Get_CoolTime()) {
			_float4 vColor = { 1.f, 1.f, 1.f, 0.5f };
			_double dRatio = 1.0 - iter.pPerk->Get_StartTime() * (1.0 / iter.pPerk->Get_CoolTime());
			_float fAngle = static_cast<_float>(dRatio) * XM_2PI; // r * 2PI
			hr |= m_pShader->Set_RawValue("g_fAngle", &fAngle, sizeof(_float));
			hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
			hr |= m_pGaugeTexs[0]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_COOLDOWN));
		}
		else {
			_float4 vColor = { 1.f, 1.f, 1.f, 0.9f };
			hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
			hr |= m_pGaugeTexs[1]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_ALPHABLEND));
		}
	}

	return hr;
}

const HRESULT CUI_Play_Icon_Perk::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	Save_AddData(L"UI_Play_Icon_Perk");
	return S_OK;
}

void CUI_Play_Icon_Perk::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	//m_pRectTransform->Set_Size((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	m_pRectTransform->Set_Size(1920.f, 1080.f);
	Load_AddData(L"UI_Play_Icon_Perk");

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player()) {
		CInven* pInven = CInven::Get_Instance();
		for (_uint i = 0; i < 4; ++i) {
			_uint iIndex = pInven->Get_PerkSlotIndex(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), i);
			if (iIndex == 9999)
				continue;
			m_sHuds[i].pImage = &m_vecImages[i];
			_tchar szKey[_MAX_PATH] = L"loadout_perk_sla";
			_tchar szBuf[8] = L"";
			_itow_s(iIndex + 1, szBuf, 10);
			lstrcat(szKey, szBuf);
			m_sHuds[i].pIconTex = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(szKey).c_str()));
			m_sHuds[i].pPerk = pInven->Ready_UIPerk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_SLASHER), i);
		}
	}
	else {
		CInven* pInven = CInven::Get_Instance();
		for (_uint i = 0; i < 4; ++i) {
			_uint iIndex = pInven->Get_PerkSlotIndex(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), i);
			if (iIndex == 9999)
				continue;
			m_sHuds[i].pImage = &m_vecImages[i];
			_tchar szKey[_MAX_PATH] = L"loadout_perk_cam";
			_tchar szBuf[8] = L"";
			_itow_s(iIndex + 1, szBuf, 10);
			lstrcat(szKey, szBuf);
			m_sHuds[i].pIconTex = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(szKey).c_str()));
			m_sHuds[i].pPerk = pInven->Ready_UIPerk(static_cast<_ubyte>(CInven::INVENTYPE::PERKS_CAMPER), i);
		}
	}

}
