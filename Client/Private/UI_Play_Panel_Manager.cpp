#include "pch.h"
#include "UI_Play_Panel_Manager.h"

CUI_Play_Panel_Manager* const CUI_Play_Panel_Manager::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Panel_Manager* pInstnace = new CUI_Play_Panel_Manager(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Panel_Manager::Clone(void* const& pArg) {
	CUI_Play_Panel_Manager* pInstnace = new CUI_Play_Panel_Manager(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Panel_Manager::CUI_Play_Panel_Manager(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Panel_Manager::CUI_Play_Panel_Manager(const CUI_Play_Panel_Manager& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Panel_Manager::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Play_Panel_Manager::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Play_Panel_Manager::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone();
	return hr;
}

const _ubyte CUI_Play_Panel_Manager::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		_ubyte byScene = static_cast<_ubyte>(EScene::GAMEPLAY);
		m_pPanels[0] = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(byScene, L"UI_Play_Panel_First"));
		m_pPanels[1] = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(byScene, L"UI_Play_Panel_Second"));
		m_pPanels[2] = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(byScene, L"UI_End_Panel_First"));
		m_pPanels[3] = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(byScene, L"UI_End_Panel_Second"));
		m_pPanels[4] = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(byScene, L"UI_End_Panel_FX"));
		m_bStart = true;
	}
	//__super::Update(dTimeDelta);

	return OBJ_NOEVENT;
}

void CUI_Play_Panel_Manager::Update_Edit(void) {
	//__super::Update_Edit();
	//ImGui::Separator();
}

void CUI_Play_Panel_Manager::Late_Update(const _double& dTimeDelta) {
}

const HRESULT CUI_Play_Panel_Manager::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	return hr;
}

CUI_Image* const CUI_Play_Panel_Manager::Get_Panel(const EPanel& ePanel) {
	if (ePanel <= EPanel::NONE || ePanel >= EPanel::END)
		return nullptr;
	return m_pPanels[static_cast<_uint>(ePanel)];
}

void CUI_Play_Panel_Manager::Set_Panel(const EPanel& ePanel, const _bool& bActive, const EPanel& eOther) {
	if (ePanel > EPanel::END)
		return;
	_bool bAll = false;
	(eOther == EPanel::NONE) ? (bAll = false) : ((eOther == EPanel::END) ? (bAll = true) : (m_pPanels[static_cast<_uint>(eOther)]->Set_Active(!bActive)));
	_uint iIdx = 0;
	for (auto& iter : m_pPanels) {
		if (iIdx++ == static_cast<_uint>(ePanel))
			iter->Set_Active(bActive);
		else {
			if (bAll)
				iter->Set_Active(!bActive);
		}
	}
}

const HRESULT CUI_Play_Panel_Manager::Save_Data(Json::Value& root) {
	return __super::Save_Data(root);
}

void CUI_Play_Panel_Manager::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
}
