#include "pch.h"
#include "UI_Lobby_Btn_Exit.h"
#include "Camera_Lobby.h"

CUI_Lobby_Btn_Exit* const CUI_Lobby_Btn_Exit::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Btn_Exit* pInstnace = new CUI_Lobby_Btn_Exit(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Btn_Exit::Clone(void* const& pArg) {
	CUI_Lobby_Btn_Exit* pInstnace = new CUI_Lobby_Btn_Exit(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Btn_Exit::CUI_Lobby_Btn_Exit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Btn_Exit::CUI_Lobby_Btn_Exit(const CUI_Lobby_Btn_Exit& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Btn_Exit::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Btn_Exit::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Btn_Exit::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("exit_btn"));	
	m_iGroup = 1;
	m_iSort = 1;
	return hr;
}

const _ubyte CUI_Lobby_Btn_Exit::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_First"));
		this->Set_Parent(pParent, false);
		m_bActiveByParent = false;
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	if (__super::IsCollisionStay(m_pCollider, m_pRectTransform)) {
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			// ready icon ++
			g_bChangeScene = true;
			return OBJ_ERASE;
		}
	}
	return OBJ_NOEVENT;
}

void CUI_Lobby_Btn_Exit::Update_Edit(void) {
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
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, (_uint)(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d");

	if (ImGui::Button("attach btn")) {
		m_iType = iTemp;
		CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
		pCamera->Add_LobbyBtn(static_cast<CCamera_Lobby::ECAMSTATE>(m_iType), this);
	}
	ImGui::SameLine();
	ImGui::SliderInt("button", (_int*)&m_iType, 0, (_uint)(CUI_Lobby_Btn_Exit::EType::END)-1, "btn = %d");

	__super::Update_Edit_Images();
}

void CUI_Lobby_Btn_Exit::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Btn_Exit::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if ((nullptr != m_pParent && m_pParent->Get_IsRender()) || !m_bActiveSelf)
		return hr;
	if (0 > m_iSort) {
		if (hr == __super::Render(eOrder))
			hr |= m_pVBRect->Render(m_pShader, m_iPass);
	}
	else {
		_uint iIdx = 0;
		for (auto& iter : m_vecImages) {
			if (m_iSort == iIdx) {
				hr |= __super::Set_RawValue(m_pShader);
				hr |= m_pShader->Set_RawValue("g_WorldMatrix", &m_pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
				hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
				hr |= m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
				m_pVBRect->Render(m_pShader, m_iPass);
			}
			++iIdx;

			if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
				continue;
#ifdef _DEBUG
			if (iter.bCollider)
				iter.pCollider->Render();
#endif // _DEBUG
			m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
			iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			m_pVBRect->Render(m_pShader, iter.iPass);
		}
	}
	return hr;
}

const HRESULT CUI_Lobby_Btn_Exit::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	root["Type"] = m_iType;
	return S_OK;
}

void CUI_Lobby_Btn_Exit::Load_Data(Json::Value& root){
	__super::Load_Data(root);	// load group
	m_iType = root["Type"].asInt();

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);
	// 수락 & 거절
	pCamera->Add_LobbyBtn(static_cast<CCamera_Lobby::ECAMSTATE>(m_iType), this);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	switch (static_cast<EType>(m_iType))
	{
	case Client::CUI_Lobby_Btn_Exit::EType::NO: {
		m_pRectTransform->Set_State(CRectTransform::EState::ROT, XMVectorSet(0.f, 0.f, 180.f, 0.f));
		SImageDesc sDesc;
		// frame
		sDesc.Reset();
		sDesc.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		sDesc.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		sDesc.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), __super::Get_TextureKey(L"exit_btn_frm").c_str()));
		lstrcpy(sDesc.szTexture, this->Get_TextureKey(L"exit_btn_frm").c_str());
		//CONVERT_WIDE_TO_MULTI(__super::Get_TextureKey(L"exit_btn_frm").c_str(), sDesc.szTexKey);
		sDesc.pRectTransform->Set_Parent(m_pRectTransform);
		sDesc.pRectTransform->Set_Size(60.f, 80.f);
		sDesc.pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(16.f, 0.f, 0.f, 0.f));
		sDesc.iDepth = 1;
		m_vecImages.emplace_back(sDesc);
		// icon
		sDesc.Reset();
		sDesc.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		sDesc.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		sDesc.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), __super::Get_TextureKey(L"sel_btn_ready_icon").c_str()));
		lstrcpy(sDesc.szTexture, this->Get_TextureKey(L"sel_btn_ready_icon").c_str());
		//CONVERT_WIDE_TO_MULTI(__super::Get_TextureKey(L"sel_btn_ready_icon").c_str(), sDesc.szTexKey);
		sDesc.pRectTransform->Set_Parent(m_pRectTransform);
		sDesc.pRectTransform->Set_Size(3.f, 60.f);
		sDesc.pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(42.f, 0.f, 0.f, 0.f));
		sDesc.iDepth = 2;
		m_vecImages.emplace_back(sDesc);
		// fx
		sDesc.Reset();
		sDesc.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		sDesc.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		sDesc.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), __super::Get_TextureKey(L"sel_btn_ready_fx").c_str()));
		lstrcpy(sDesc.szTexture, this->Get_TextureKey(L"sel_btn_ready_fx").c_str());
		//CONVERT_WIDE_TO_MULTI(__super::Get_TextureKey(L"sel_btn_ready_fx").c_str(), sDesc.szTexKey);
		sDesc.pRectTransform->Set_Parent(m_pRectTransform);
		sDesc.pRectTransform->Set_Size(20.f, 60.f);
		sDesc.pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(42.f, 10.f, 0.f, 0.f));
		sDesc.iDepth = 3;
		m_vecImages.emplace_back(sDesc);
	}
		break;
	case Client::CUI_Lobby_Btn_Exit::EType::YES: {
		SImageDesc sDesc;
		// frame
		sDesc.Reset();
		sDesc.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		sDesc.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		sDesc.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), __super::Get_TextureKey(L"exit_btn_frm").c_str()));
		lstrcpy(sDesc.szTexture, this->Get_TextureKey(L"exit_btn_frm").c_str());
		//CONVERT_WIDE_TO_MULTI(__super::Get_TextureKey(L"exit_btn_frm").c_str(), sDesc.szTexKey);
		sDesc.pRectTransform->Set_Parent(m_pRectTransform);
		sDesc.pRectTransform->Set_Size(60.f, 80.f);
		sDesc.pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(16.f, 0.f, 0.f, 0.f));
		sDesc.iDepth = 1;
		m_vecImages.emplace_back(sDesc);
		// icon
		sDesc.Reset();
		sDesc.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		sDesc.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		sDesc.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), __super::Get_TextureKey(L"sel_btn_ready_icon").c_str()));
		lstrcpy(sDesc.szTexture, this->Get_TextureKey(L"sel_btn_ready_icon").c_str());
		//CONVERT_WIDE_TO_MULTI(__super::Get_TextureKey(L"sel_btn_ready_icon").c_str(), sDesc.szTexKey);
		sDesc.pRectTransform->Set_Parent(m_pRectTransform);
		sDesc.pRectTransform->Set_Size(3.f, 60.f);
		sDesc.pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(42.f, 0.f, 0.f, 0.f));
		sDesc.iDepth = 2;
		m_vecImages.emplace_back(sDesc);
		// fx
		sDesc.Reset();
		sDesc.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		sDesc.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		sDesc.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), __super::Get_TextureKey(L"sel_btn_ready_fx").c_str()));
		lstrcpy(sDesc.szTexture, this->Get_TextureKey(L"sel_btn_ready_fx").c_str());
		//CONVERT_WIDE_TO_MULTI(__super::Get_TextureKey(L"sel_btn_ready_fx").c_str(), sDesc.szTexKey);
		sDesc.pRectTransform->Set_Parent(m_pRectTransform);
		sDesc.pRectTransform->Set_Size(20.f, 60.f);
		sDesc.pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(42.f, 10.f, 0.f, 0.f));
		sDesc.iDepth = 3;
		m_vecImages.emplace_back(sDesc);
	}
		break;
	}

}
