#include "pch.h"
#include "UI_Lobby_Panel_Exit.h"
#include "Camera_Lobby.h"

CUI_Lobby_Panel_Exit* const CUI_Lobby_Panel_Exit::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Panel_Exit* pInstnace = new CUI_Lobby_Panel_Exit(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Panel_Exit::Clone(void* const& pArg) {
	CUI_Lobby_Panel_Exit* pInstnace = new CUI_Lobby_Panel_Exit(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Panel_Exit::CUI_Lobby_Panel_Exit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Panel_Exit::CUI_Lobby_Panel_Exit(const CUI_Lobby_Panel_Exit& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Panel_Exit::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Panel_Exit::Init_Create(void) {
	HRESULT hr = S_OK;
	m_iGroup = 1;
	return hr;
}

const HRESULT CUI_Lobby_Panel_Exit::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("exit_panel_back"));
	m_iGroup = 0;
	return hr;
}

const _ubyte CUI_Lobby_Panel_Exit::Update(const _double& dTimeDelta) {
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CUI_Lobby_Panel_Exit::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	static _int iTemp = m_iGroup;
	const char* combo_uitype = CCamera_Lobby::ECamState_ToString(static_cast<CCamera_Lobby::ECAMSTATE>(iTemp));
	if (ImGui::BeginCombo("group type", combo_uitype)) {
		_int iSize = static_cast<_uint>(CCamera_Lobby::ECAMSTATE::END);
		for (_int i = 0; i < iSize; ++i) {
			const bool is_selected = (iTemp == i);
			if (ImGui::Selectable(CCamera_Lobby::ECamState_ToString(static_cast<CCamera_Lobby::ECAMSTATE>(i)), is_selected))
				iTemp = i;
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("group")) {
		m_iGroup = iTemp;
		CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
		pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);
	}
	ImGui::SameLine();
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, (_uint)(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d", ImGuiInputTextFlags_ReadOnly);

	__super::Update_Edit_Images();
}

void CUI_Lobby_Panel_Exit::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Panel_Exit::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;

	if (0 > m_iSort) {
		if (hr == __super::Render(eOrder))
			hr |= m_pVBRect->Render(m_pShader, m_iPass);
	}
	else {
		_uint iIdx = 0;
		for (auto& iter : m_vecImages) {
			if (m_iSort == iIdx) {
				if (hr == __super::Render(eOrder))
					hr |= m_pVBRect->Render(m_pShader, m_iPass);
			}
			++iIdx;

			if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
				continue;
			if (iter.bCollider)
				iter.pCollider->Render();
			m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
			iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			m_pVBRect->Render(m_pShader, iter.iPass);
		}
	}
	return hr;
}

const HRESULT CUI_Lobby_Panel_Exit::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_Lobby_Panel_Exit::Load_Data(Json::Value& root) {
	__super::Load_Data(root);

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = Add_Image(L"exit_bar");
	pImage->pRectTransform->Set_Parent(m_pRectTransform);

	// frame
	//SImageDesc sDesc;
	//sDesc.Reset();
	//sDesc.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
	//sDesc.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
	//sDesc.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), __super::Get_TextureKey(L"exit_bar").c_str()));
	//CONVERT_WIDE_TO_MULTI(__super::Get_TextureKey(L"exit_bar").c_str(), sDesc.szTexKey);
	//sDesc.pRectTransform->Set_Parent(m_pRectTransform);
	//m_vecImages.emplace_back(sDesc);
}
