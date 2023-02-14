#include "pch.h"
#include "UI_Lobby_Panel_Slasher.h"
#include "Camera_Lobby.h"

CUI_Lobby_Panel_Slasher* const CUI_Lobby_Panel_Slasher::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Panel_Slasher* pInstnace = new CUI_Lobby_Panel_Slasher(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Panel_Slasher::Clone(void* const& pArg) {
	CUI_Lobby_Panel_Slasher* pInstnace = new CUI_Lobby_Panel_Slasher(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Panel_Slasher::CUI_Lobby_Panel_Slasher(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Panel_Slasher::CUI_Lobby_Panel_Slasher(const CUI_Lobby_Panel_Slasher& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Panel_Slasher::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Panel_Slasher::Init_Create(void) {
	HRESULT hr = S_OK;
	m_iGroup = 2;
	return hr;
}

const HRESULT CUI_Lobby_Panel_Slasher::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(/*TEXT("sel_panel")*/);
	return hr;
}

const _ubyte CUI_Lobby_Panel_Slasher::Update(const _double& dTimeDelta) {
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;
	this->Set_Fade(true, 0.f, 0.5f, (_float)dTimeDelta, !m_bPreActive);
	return OBJ_NOEVENT;
}

void CUI_Lobby_Panel_Slasher::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	static _int iTemp = m_iGroup;
	const char* combo_uitype = CCamera_Lobby::ECamState_ToString(static_cast<CCamera_Lobby::ECAMSTATE>(iTemp));
	if (ImGui::BeginCombo("Group Type", combo_uitype)) {
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
	
	Update_Edit_Images();
}

void CUI_Lobby_Panel_Slasher::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Panel_Slasher::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_vecImages) {
		if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
			continue;
#ifdef _DEBUG
		if (iter.bCollider)
			iter.pCollider->Render();
#endif // _DEBUG
		if (iter.bActByParent)
			iter.vColor.w = m_vColor.w;

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
	}
	return hr;
}

const HRESULT CUI_Lobby_Panel_Slasher::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_Lobby_Panel_Slasher::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_pRectTransform->Set_Size(1920.f, 1080.f);
	m_vColor.w = 0.f;

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);

	SImageDesc* pDesc = nullptr;
	pDesc = Add_Image(L"sel_panel", CRectTransform::EEdge::LTOP);
	pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-200.f, -150.f, 0.f));
	pDesc->vColor.w = 0.5f;

	pDesc = Add_Image(L"sel_choice_back", CRectTransform::EEdge::LTOP);
	pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-200.f, 0.f, 0.f));
	pDesc->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(.8f, .8f, .8f));
	pDesc->bActByParent = false;
	pDesc->vColor.w = .8f;
}
