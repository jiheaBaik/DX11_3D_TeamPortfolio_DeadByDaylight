#include "pch.h"
#include "UI_Lobby_Icon_Ready_Camper.h"
#include "Camera_Lobby.h"

CUI_Lobby_Icon_Ready_Camper* const CUI_Lobby_Icon_Ready_Camper::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Icon_Ready_Camper* pInstnace = new CUI_Lobby_Icon_Ready_Camper(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Icon_Ready_Camper::Clone(void* const& pArg) {
	CUI_Lobby_Icon_Ready_Camper* pInstnace = new CUI_Lobby_Icon_Ready_Camper(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Icon_Ready_Camper::CUI_Lobby_Icon_Ready_Camper(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Icon_Ready_Camper::CUI_Lobby_Icon_Ready_Camper(const CUI_Lobby_Icon_Ready_Camper& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Icon_Ready_Camper::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Icon_Ready_Camper::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Icon_Ready_Camper::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	hr |= __super::Init_Clone(L"sel_camper_ready");
	m_iType = 0;
	return hr;
}

const _ubyte CUI_Lobby_Icon_Ready_Camper::Update(const _double& dTimeDelta) {
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CUI_Lobby_Icon_Ready_Camper::Update_Edit(void) {
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
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, static_cast<_int>(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d");
}

void CUI_Lobby_Icon_Ready_Camper::Late_Update(const _double& dTimeDelta) {
	if (!m_bActiveSelf)
		return;
	__super::Late_Update(dTimeDelta);

}

const HRESULT CUI_Lobby_Icon_Ready_Camper::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);
	return hr;
}

const HRESULT CUI_Lobby_Icon_Ready_Camper::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	//root["Type"] = m_iType;
	return S_OK;
}
 
void CUI_Lobby_Icon_Ready_Camper::Load_Data(Json::Value& root){
	__super::Load_Data(root);
	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);

	//m_iType = root["Type"].asInt();
}
