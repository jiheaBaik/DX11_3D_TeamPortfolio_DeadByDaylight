#include "pch.h"
#include "UI_End_Btn_Return.h"
#include "UI_Play_Panel_Manager.h"

CUI_End_Btn_Return* const CUI_End_Btn_Return::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_End_Btn_Return* pInstnace = new CUI_End_Btn_Return(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_End_Btn_Return::Clone(void* const& pArg) {
	CUI_End_Btn_Return* pInstnace = new CUI_End_Btn_Return(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_End_Btn_Return::CUI_End_Btn_Return(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_End_Btn_Return::CUI_End_Btn_Return(const CUI_End_Btn_Return& rhs)
	: CUI_Image(rhs) {
}

void CUI_End_Btn_Return::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_End_Btn_Return::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_End_Btn_Return::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(L"end_btn_back");
	return hr;
}

const _ubyte CUI_End_Btn_Return::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_End_Panel_Second"));
		this->Set_Parent(pParent, false);
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	if (__super::IsCollisionStay(m_pCollider, m_pRectTransform)) {
		if (5 > m_iFXIdx && this->Invoke(dTimeDelta, 0.005f)) {
			SImageDesc* pImage = &m_vecImages[static_cast<_uint>(ETexType::FX) + m_iFXIdx++];
			if (ETexType::FX == static_cast<ETexType>(pImage->iDepth))
				pImage->bRender = true;
		}
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			// continue end
			if (0 > m_iType) {
				dynamic_cast<CUI_Play_Panel_Manager*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Panel_Manager"))->Set_Panel(CUI_Play_Panel_Manager::EPanel::FADE, true, CUI_Play_Panel_Manager::EPanel::END);
				return OBJ_NOEVENT;
			}
			// watch
			else {

			}
		}
	}
	else {
		if (0 < m_iFXIdx && this->Invoke(dTimeDelta, 0.02f)) {
			SImageDesc* pImage = &m_vecImages[static_cast<_uint>(ETexType::FX) + --m_iFXIdx];
			if (ETexType::FX == static_cast<ETexType>(pImage->iDepth))
				pImage->bRender = false;
		}
	}

	return OBJ_NOEVENT;
}

void CUI_End_Btn_Return::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	ImGui::InputInt("type", &m_iType);

	Update_Edit_Images();
}

void CUI_End_Btn_Return::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_End_Btn_Return::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	Render_Images(eOrder);

	return hr;
}

const HRESULT CUI_End_Btn_Return::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	root["Type"] = m_iType;
	Save_AddData(L"UI_End_Btn_Return");
	return S_OK;
}

void CUI_End_Btn_Return::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_iType = root["Type"].asInt();
	Load_AddData(L"UI_End_Btn_Return");

	//SImageDesc* pImage = nullptr;
	//// frm
	//pImage = Add_Image(L"end_btn_frame", static_cast<_uint>(ETexType::FRAME), true, CRectTransform::EEdge::CEN, false);
	//pImage->pRectTransform->Set_Parent(m_pRectTransform);
	//// icon
	//pImage = Add_Image(L"end_btn_icon", static_cast<_uint>(ETexType::ICON), true, CRectTransform::EEdge::CEN, false);
	//pImage->pRectTransform->Set_Parent(m_pRectTransform);
	//pImage->pRectTransform->Set_Size(3.f, 60.f);
	//pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(42.f, 0.f, 0.f));
	//// fx
	//for (_uint i = 0; i < 5; ++i) {
	//	pImage = Add_Image(L"end_btn_return_fx2", static_cast<_uint>(ETexType::FX), false, CRectTransform::EEdge::RCOR);
	//	pImage->pRectTransform->Set_Parent(m_pRectTransform);
	//	pImage->pRectTransform->Set_Size(40.f, 130.f - (i * 10.f));
	//	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-80.f - (15.f * i), 0.f, 0.f));
	//}
	//// text
	//if (0 > m_iType) {
	//	pImage = Add_Image(L"end_btn_txt_res", static_cast<_uint>(ETexType::TEXT), true, CRectTransform::EEdge::RBOT);
	//	pImage->pRectTransform->Set_Anchor(m_pRectTransform->Get_Desc().vAnchor);
	//	pImage->pRectTransform->Set_Pivot(m_pRectTransform->Get_Desc().vPivot);
	//	//pImage->pRectTransform->Set_State(CRectTransform::EState::POS, XMLoadFloat3(&m_pRectTransform->Get_Desc().vPosition) + XMVectorSet(-20.f, -18.f, 0.f, 0.f));
	//	//pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.9f, 0.9f, 1.f));
	//	//pImage->pRectTransform->Set_Size(80.f, 60.f);
	//	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-73.f, 54.f, 0.f));
	//}
	//else {
	//	pImage = Add_Image(L"end_btn_txt_watch", static_cast<_uint>(ETexType::TEXT), true, CRectTransform::EEdge::RBOT);
	//	pImage->pRectTransform->Set_Anchor(m_pRectTransform->Get_Desc().vAnchor);
	//	pImage->pRectTransform->Set_Pivot(m_pRectTransform->Get_Desc().vPivot);
	//	//pImage->pRectTransform->Set_Size(80.f, 60.f);
	//	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-283.f, 55.f, 0.f));
	//}

	//Sort_Images();
}
