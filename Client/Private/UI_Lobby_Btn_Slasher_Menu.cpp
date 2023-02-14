#include "pch.h"
#include "UI_Lobby_Btn_Slasher_Menu.h"
#include "Camera_Lobby.h"

list<class CObject*>* CUI_Lobby_Btn_Slasher_Menu::s_pMenuItems = nullptr;
_int CUI_Lobby_Btn_Slasher_Menu::s_iOnClicked = -1;

CUI_Lobby_Btn_Slasher_Menu* const CUI_Lobby_Btn_Slasher_Menu::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Btn_Slasher_Menu* pInstnace = new CUI_Lobby_Btn_Slasher_Menu(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Btn_Slasher_Menu::Clone(void* const& pArg) {
	CUI_Lobby_Btn_Slasher_Menu* pInstnace = new CUI_Lobby_Btn_Slasher_Menu(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Btn_Slasher_Menu::CUI_Lobby_Btn_Slasher_Menu(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Btn_Slasher_Menu::CUI_Lobby_Btn_Slasher_Menu(const CUI_Lobby_Btn_Slasher_Menu& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Btn_Slasher_Menu::Delete(void) {
	__super::Delete();
#ifdef _DEBUG
	this->Delete_AddData(L"UI_Lobby_Btn_Slasher_Menu");
#endif // _DEBUG
}

const HRESULT CUI_Lobby_Btn_Slasher_Menu::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Btn_Slasher_Menu::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("sel_btn"));	
	m_iGroup = 2;
	m_iSort = 1;
	//strcpy_s(m_szParentKey, "UI_Lobby_Panel_First");
	return hr;
}

const _ubyte CUI_Lobby_Btn_Slasher_Menu::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_Slasher"));
		this->Set_Parent(pParent, false);
		__super::Update(dTimeDelta);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf || (nullptr != m_pParent && m_pParent->Get_IsFade())) {
		this->Set_OnClicked(-1);
		return OBJ_NOEVENT;
	}
	if (!m_bPreActive)
		m_vSpread.y = 0.f;

	if (__super::IsCollisionEnter(m_pCollider, m_pRectTransform)) {
		m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_highlight"), 4, -1, 0.08f, FMOD_2D);
	}
	if (__super::IsCollisionStay(m_pCollider, m_pRectTransform)) {
		// hover : fx + view explain
		m_vSpread.y += (_float)dTimeDelta;
		if (1.f <= m_vSpread.y)
			m_vSpread.y = 1.f;
		for (auto& iter : m_vecImages) {
			if (!iter.bRender &&
				ETexType::BRIDGE == static_cast<ETexType>(iter.iDepth) || ETexType::TEXT == static_cast<ETexType>(iter.iDepth))
				iter.bRender = true;
		}	
		// click : select char
		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_select_press_bones"), 3, -1, 0.25f, FMOD_2D);
		}
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			m_pAudio->Play_Sound(TEXT("sfx_ui_select_release_01"), -1, 0.2f, FMOD_2D);
			m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_select_release_bones"), 3, -1, 0.2f, FMOD_2D);
			CInfo* pInfo = CInfo::Get_Instance();
			switch (static_cast<EMenuType>(m_iType))
			{
			case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::ONE: {
				this->Set_OnClicked(m_iType);
				this->Set_MenuItmes(m_szMenuItems);
				m_vecImages[static_cast<_uint>(ETexType::BRIDGE2)].bRender = true;
			}
				break;
			case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::TWO: {
				this->Set_OnClicked(m_iType);
				//_uint i = (m_iGroup == static_cast<_uint>(CCamera_Lobby::ECAMSTATE::SELECT_SLASHER)) ? 0 : 1;
				this->Set_MenuItmes(m_szMenuItems, m_iGroup);
				m_vecImages[static_cast<_uint>(ETexType::BRIDGE2)].bRender = true;
			}
				break;
			case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::THREE: {
				this->Set_OnClicked(m_iType);
				this->Set_MenuItmes(m_szMenuItems);
				m_vecImages[static_cast<_uint>(ETexType::BRIDGE2)].bRender = true;
			}
				break;
			case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::FOUR: {
				//_uint iCamperSlot = pInfo->Get_CamperSlot();
				//if (iCamperSlot < 4) {
				//	this->Set_MenuItmes();
				//	m_vecImages[static_cast<_uint>(ETexType::BRIDGE2)].bRender = true;

				//	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
				//	pCamera->Set_State(CCamera_Lobby::ECAMSTATE::START_CAMPER);
				//	pCamera->Set_Group(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), CCamera_Lobby::ECAMSTATE::START_CAMPER);

				//	pInfo->Add_CamperSlot(1);
				//	pInfo->Set_Player(false);
				//	pInfo->Set_Camper(CInfo::ECamper::CHERYL);
				//}
			}
				break;
			}
		}
	}
	// after click
	else {
		m_vSpread.y -= (_float)dTimeDelta * 2.f;
		if (0.f >= m_vSpread.y)
			m_vSpread.y = 0.f;

		for (auto& iter : m_vecImages) {
			if ((ETexType::BRIDGE == static_cast<ETexType>(iter.iDepth)) ||	(ETexType::BRIDGE2 == static_cast<ETexType>(iter.iDepth))) {
				(m_iType == s_iOnClicked) ? (iter.bRender = true) : (iter.bRender = false);
			}
			else if (ETexType::TEXT == static_cast<ETexType>(iter.iDepth)) {
				if (iter.bRender)
					iter.bRender = false;
			}
		}
	}

	return OBJ_NOEVENT;
}

void CUI_Lobby_Btn_Slasher_Menu::Update_Edit(void) {
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

void CUI_Lobby_Btn_Slasher_Menu::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Btn_Slasher_Menu::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf || nullptr == m_pParent)
		return hr;
	(m_pParent->Get_IsFade()) ? (m_vColor.w = m_pParent->Get_Color().w) : (m_vColor.w = 1.f);
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
			if (iter.bActByParent && iter.iDepth != static_cast<_uint>(ETexType::BRIDGE))
				iter.vColor.w = m_vColor.w;

			iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
			if (EUIPass::PASS_SPREAD == static_cast<EUIPass>(iter.iPass))
				m_pShader->Set_RawValue("g_vSpread", &m_vSpread, sizeof(_float2));
			m_pVBRect->Render(m_pShader, iter.iPass);
#ifdef _DEBUG
			if (iter.bCollider)
				iter.pCollider->Render();
#endif // _DEBUG
		}
	}
	return hr;
}

void CUI_Lobby_Btn_Slasher_Menu::Set_MenuItmes(const _tchar* pLayer, const _int& iIndex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	// off
	if (nullptr != s_pMenuItems)
		for (auto& iter : *s_pMenuItems) {
			dynamic_cast<CUI_Image*>(iter)->Set_Active(false);
		}
	// on
	if (nullptr == pLayer)
		return;
	_uint iItem = 0;
	list<class CObject*>* pNewItems = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_Scene(), pLayer);
	if (nullptr != pNewItems)
		if (0 <= iIndex) {
			for (auto& iter : *pNewItems) {
				CUI_Image* pImage = dynamic_cast<CUI_Image*>(iter);
				if (pImage->Get_GroupIdx() == iIndex) {
					pImage->Set_Active(true);
					break;
				}
			}
		}
		else {
			for (auto& iter : *pNewItems)
				dynamic_cast<CUI_Image*>(iter)->Set_Active(true);
		}
	s_pMenuItems = pNewItems;
}

const HRESULT CUI_Lobby_Btn_Slasher_Menu::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	root["Type"] = m_iType;
	this->Save_AddData(L"UI_Lobby_Btn_Slasher_Menu");
	return S_OK;
}

void CUI_Lobby_Btn_Slasher_Menu::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_iType = root["Type"].asInt();

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);

	this->Load_AddData(L"UI_Lobby_Btn_Slasher_Menu");
	switch (static_cast<EMenuType>(m_iType))
	{
	case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::ONE:
		lstrcpy(m_szMenuItems, L"UI_Lobby_Panel_Slasher_Choice");
		break;
	case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::TWO:
		lstrcpy(m_szMenuItems, L"UI_Lobby_Panel_Loadout");
		break;
	case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::THREE:
		break;
	case Client::CUI_Lobby_Btn_Slasher_Menu::EMenuType::FOUR:
		break;
	}

}
