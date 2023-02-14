#include "pch.h"
#include "UI_Lobby_Icon_Ready_Back.h"
#include "Camera_Lobby.h"

CUI_Lobby_Icon_Ready_Back* const CUI_Lobby_Icon_Ready_Back::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Icon_Ready_Back* pInstnace = new CUI_Lobby_Icon_Ready_Back(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Icon_Ready_Back::Clone(void* const& pArg) {
	CUI_Lobby_Icon_Ready_Back* pInstnace = new CUI_Lobby_Icon_Ready_Back(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Icon_Ready_Back::CUI_Lobby_Icon_Ready_Back(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Icon_Ready_Back::CUI_Lobby_Icon_Ready_Back(const CUI_Lobby_Icon_Ready_Back& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Icon_Ready_Back::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Icon_Ready_Back::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Icon_Ready_Back::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(L"sel_icon_ready_back");
	return hr;
}

const _ubyte CUI_Lobby_Icon_Ready_Back::Update(const _double& dTimeDelta) {
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CUI_Lobby_Icon_Ready_Back::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	static _uint iTemp = m_iGroup;
	const char* combo_uitype = CCamera_Lobby::ECamState_ToString(static_cast<CCamera_Lobby::ECAMSTATE>(iTemp));
	if (ImGui::BeginCombo("group type", combo_uitype)) {
		_uint iSize = static_cast<_uint>(CCamera_Lobby::ECAMSTATE::END);
		for (_uint i = 0; i < iSize; ++i) {
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
	ImGui::SliderInt("group", &m_iGroup, 0, static_cast<_int>(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d");
}

void CUI_Lobby_Icon_Ready_Back::Late_Update(const _double& dTimeDelta) {
	if (!m_bActiveSelf)
		return;
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Icon_Ready_Back::Render(const CRenderer::EOrder& eOrder) {
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

const HRESULT CUI_Lobby_Icon_Ready_Back::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}
 
void CUI_Lobby_Icon_Ready_Back::Load_Data(Json::Value& root){
	__super::Load_Data(root);

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	//pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);
	pCamera->Add_UIGroup(CCamera_Lobby::ECAMSTATE::START_CAMPER, this);
	pCamera->Add_UIGroup(CCamera_Lobby::ECAMSTATE::SELECT_SLASHER , this);

}
