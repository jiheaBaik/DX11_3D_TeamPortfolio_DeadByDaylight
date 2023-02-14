#include "pch.h"
#include "UI_Lobby_Icon_Ready_Bone.h"
#include "Camera_Lobby.h"

CUI_Lobby_Icon_Ready_Bone* const CUI_Lobby_Icon_Ready_Bone::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Icon_Ready_Bone* pInstnace = new CUI_Lobby_Icon_Ready_Bone(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Icon_Ready_Bone::Clone(void* const& pArg) {
	CUI_Lobby_Icon_Ready_Bone* pInstnace = new CUI_Lobby_Icon_Ready_Bone(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Icon_Ready_Bone::CUI_Lobby_Icon_Ready_Bone(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Icon_Ready_Bone::CUI_Lobby_Icon_Ready_Bone(const CUI_Lobby_Icon_Ready_Bone& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Icon_Ready_Bone::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Icon_Ready_Bone::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Icon_Ready_Bone::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone();
	m_iType = 0;
	m_iPass = 5;
	return hr;
}

const _ubyte CUI_Lobby_Icon_Ready_Bone::Update(const _double& dTimeDelta) {
	if (m_bDead)
		return OBJ_ERASE;
	__super::Update(dTimeDelta);

	CInfo* pInfo = CInfo::Get_Instance();
	_int iReady = (_int)pInfo->Get_Ready();

	if (iReady > m_iType) 
		m_bReady = true;
	else {
		m_fTime = 0.f;
		m_bReady = false;
	}			
	//if (!m_bActiveSelf)
	//	return OBJ_NOEVENT;
	if (m_bActiveSelf) 
		m_fTime += (_float)dTimeDelta * 5.f;
	else 
		m_fTime -= (_float)dTimeDelta * 5.f;
	(m_fTime >= 1.0f) ? (m_fTime = 1.0) : (m_fTime <= 0.f ? m_fTime = 0.f : m_fTime);
	
	return OBJ_NOEVENT;
}

void CUI_Lobby_Icon_Ready_Bone::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();
	ImGui::InputInt("Index", (int*)&m_iIndex);

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

	if (ImGui::Button("type add") && nullptr == m_pTexture) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Icon_Ready_Back"));
		switch (m_iType)
		{
		case 0: {
			__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front1").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
			this->Set_Parent(pParent);
			//m_pRectTransform->Set_RectState(CRectTransform::EState::POS, );
		}
			  break;
		case 1: {
			__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front2").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
			this->Set_Parent(pParent);
		}
			  break;
		case 2: {
			__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front3").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
			this->Set_Parent(pParent);
		}
			  break;
		case 3: {
			__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front4").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
			this->Set_Parent(pParent);
		}
			  break;
		case 4: {
			__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front5").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
			this->Set_Parent(pParent);
		}
			  break;
		}
	}
	ImGui::SameLine();
	ImGui::SliderInt("type", (_int*)&m_iType, 0, 4, "type = %d");
}

void CUI_Lobby_Icon_Ready_Bone::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Icon_Ready_Bone::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bReady || !m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder)) {
		hr |= m_pShader->Set_RawValue("g_fLoading", &m_fTime, sizeof(_float));

		_float fAlpha = 0.1f;
		hr |= m_pShader->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float));
		hr |= m_pVBRect->Render(m_pShader, m_iPass);
	}
	// color 레디 X 면, 회색 O 면 빨강 ? 

	return hr;
}

const HRESULT CUI_Lobby_Icon_Ready_Bone::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	root["Type"] = m_iType;
	return S_OK;
}

void CUI_Lobby_Icon_Ready_Bone::Load_Data(Json::Value& root){
	__super::Load_Data(root);

	m_iType = root["Type"].asInt();
	m_iDepth = m_iType + 1;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Icon_Ready_Back"));
	switch (m_iType)
	{
	case 0: {
		__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front1").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
		this->Set_Parent(pParent);
	}
		  break;
	case 1: {
		__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front2").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
		this->Set_Parent(pParent);
	}
		  break;
	case 2: {
		__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front3").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
		this->Set_Parent(pParent);
	}
		  break;
	case 3: {
		__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front4").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
		this->Set_Parent(pParent);
	}
		  break;
	case 4: {
		__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"sel_icon_ready_front5").c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
		this->Set_Parent(pParent);
	}
		  break;

	}
	
	//m_bActiveByParent = false;
	this->Set_Active(false);

}
