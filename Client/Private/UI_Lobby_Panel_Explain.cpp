#include "pch.h"
#include "UI_Lobby_Panel_Explain.h"
#include "Camera_Lobby.h"

CUI_Lobby_Panel_Explain* const CUI_Lobby_Panel_Explain::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Panel_Explain* pInstnace = new CUI_Lobby_Panel_Explain(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Panel_Explain::Clone(void* const& pArg) {
	CUI_Lobby_Panel_Explain* pInstnace = new CUI_Lobby_Panel_Explain(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Panel_Explain::CUI_Lobby_Panel_Explain(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Panel_Explain::CUI_Lobby_Panel_Explain(const CUI_Lobby_Panel_Explain& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Panel_Explain::Delete(void) {
	__super::Delete();
	for (auto& iter : m_pFlowTextures) {
		Safe_Release(iter);
	}
}

const HRESULT CUI_Lobby_Panel_Explain::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Panel_Explain::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	//hr |= __super::Init_Clone(/*TEXT("sel_btn")*/);	
	hr |= __super::Init_Clone(L"loadout_exp_back2");
	m_iGroup = 2;
	m_iSort = 1;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pFlowTextures[0] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"fog11").c_str()));

	//SImageDesc* pImage = nullptr;
	//// back
	//pImage = Add_Image(L"00exp", CRectTransform::EEdge::CEN);
	//CRectTransform* pParent = pImage->pRectTransform;
	////pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(1.5f, 2.f, 0.f));
	//pImage->iDepth = static_cast<_uint>(ETexType::BACK);
	//pImage = nullptr;
	//// tag
	//pImage = Add_Image(L"exit_bar", CRectTransform::EEdge::CEN);
	////pImage->pRectTransform->Set_Parent(pParent, false);
	//pParent = pImage->pRectTransform;
	//pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(0.f, 300.f, 0.f));
	//pImage->iDepth = static_cast<_uint>(ETexType::TAG);
	//pImage->vColor = { 0.f, 0.f, 0.f, 1.f };
	//pImage = nullptr;
	//// fx
	//pImage = Add_Image(L"fx_flow3", CRectTransform::EEdge::CEN);
	//pImage->pRectTransform->Set_Desc(pParent->Get_Desc());
	//pImage->iDepth = static_cast<_uint>(ETexType::FLOW1);
	//pImage->iPass = 3;
	//pImage = nullptr;

	//this->Sort_Images();

	return hr;
}

const _ubyte CUI_Lobby_Panel_Explain::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//if (!m_bStart) {
	//	CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_Slasher"));
	//	this->Set_Parent(pParent, false);
	//	m_bStart = true;
	//}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf || (nullptr != m_pParent && m_pParent->Get_IsFade())) {
		//this->Set_OnClicked(-1);
		return OBJ_NOEVENT;
	}
	//if (__super::IsCollisionStay(m_pCollider, m_pRectTransform)) {
	//	// click : select char
	//	if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
	//	}
	//}

	for (auto& iter : m_vecImages) {
		if (__super::IsCollisionStay(iter.pCollider, iter.pRectTransform)) {
			if (iter.iDepth == static_cast<_uint>(ETexType::FLOW1)) {
				//iter.pRectTransform->Set_State(CRectTransform::EState::ROT, _float3(0.f, 0.f, 10.f), true);

				// hover : fx + view explain
				m_vTrans.x += (_float)dTimeDelta * 0.02f;
				m_vTrans.y += (_float)dTimeDelta * 0.05f;
			}
		}
	}

	return OBJ_NOEVENT;
}

void CUI_Lobby_Panel_Explain::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

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
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, (_uint)(CCamera_Lobby::ECAMSTATE::START_SLASHER_TO_FIRST)-1, CCamera_Lobby::ECamState_ToString(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup)));
	ImGui::SliderInt("button", (_int*)&m_iType, 0, (_uint)(EMenuType::END)-1, "btn = %d");

	__super::Update_Edit_Images();
}

void CUI_Lobby_Panel_Explain::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Panel_Explain::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf/* || nullptr == m_pParent*/)
		return hr;
	//(m_pParent->Get_IsFade()) ? (m_vColor.w = m_pParent->Get_Color().w) : (m_vColor.w = 1.f);

	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	//_uint iIdx = 0;
	//for (auto& iter : m_vecImages) {
	//	if (m_iSort == iIdx) {
	//		if (hr == __super::Render(eOrder))
	//			hr |= m_pVBRect->Render(m_pShader, m_iPass);
	//	}
	//	++iIdx;

	//	if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
	//		continue;
	//	if (iter.bCollider)
	//		iter.pCollider->Render();
	//	if (iter.bActByParent)
	//		iter.vColor.w = m_vColor.w;

	//	//_float4x4 f4World = iter.pRectTransform->Get_Float4x4_Transpose();
	//	//if (iter.iDepth == static_cast<_uint>(ETexType::FLOW1)) {
	//	//	f4World = m_vecImages[static_cast<_uint>(ETexType::TAG)].pRectTransform->Get_Float4x4_Transpose();
	//	//}
	//	hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
	//	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
	//	hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
	//	if (EUIPass::PASS_FLOW == static_cast<EUIPass>(iter.iPass)) {
	//		hr |= m_pFlowTextures[0]->Set_Resource(m_pShader, "g_FlowTexture", 0);
	//		hr |= m_pShader->Set_RawValue("g_vTrans", &m_vTrans, sizeof(_float2));
	//		//_float fDelta = .5f;
	//		//hr |= m_pShader->Set_RawValue("g_fDelta", &fDelta, sizeof(_float));
	//	}
	//	hr |= m_pVBRect->Render(m_pShader, iter.iPass);
	//}

	return hr;
}


const HRESULT CUI_Lobby_Panel_Explain::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	//root["Type"] = m_iType;
	return S_OK;
}

void CUI_Lobby_Panel_Explain::Load_Data(Json::Value& root){
	__super::Load_Data(root);
	//m_iType = root["Type"].asInt();

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);
	//pCamera->Add_LobbyBtn(static_cast<CCamera_Lobby::ECAMSTATE>(m_iBtnIdx), this);

	//CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	//SImageDesc* pImage = nullptr;
	//// back
	//pImage = Add_Image(L"exp_nametag_back2", CRectTransform::EEdge::CEN);
	//CRectTransform* pParent = pImage->pRectTransform;
	//pImage->iDepth = static_cast<_uint>(ETexType::BACK);
	//pImage = nullptr;
	//// tag
	//pImage = Add_Image(L"exp_nametag_back", CRectTransform::EEdge::CEN);
	//pImage->pRectTransform->Set_Parent(pParent, false);
	////pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3());
	//pImage->iDepth = static_cast<_uint>(ETexType::TAG);
	//pImage->iPass = static_cast<_uint>(EUIPass::PASS_FLOW);
	//pImage = nullptr;
}
