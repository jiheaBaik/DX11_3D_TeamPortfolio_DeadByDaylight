#include "pch.h"
#include "UI_Lobby_Btn_Select_Role.h"
#include "Camera_Lobby.h"
#include "Camper_Lobby.h"
#include "Camper_Lobby_Sit.h"
#include "UI_Lobby_Panel_Loadout.h"

CUI_Lobby_Btn_Select_Role* const CUI_Lobby_Btn_Select_Role::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Btn_Select_Role* pInstnace = new CUI_Lobby_Btn_Select_Role(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Btn_Select_Role::Clone(void* const& pArg) {
	CUI_Lobby_Btn_Select_Role* pInstnace = new CUI_Lobby_Btn_Select_Role(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Btn_Select_Role::CUI_Lobby_Btn_Select_Role(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Btn_Select_Role::CUI_Lobby_Btn_Select_Role(const CUI_Lobby_Btn_Select_Role& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Btn_Select_Role::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Btn_Select_Role::Init_Create(void) {
	HRESULT hr = S_OK;
	//hr |= __super::Init_Font();
	return hr;
}

const HRESULT CUI_Lobby_Btn_Select_Role::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("btn_back"));
	m_iGroup = 1;
	m_vAlphaLim = { 0.5f, 0.8f };
	return hr;
}

const _ubyte CUI_Lobby_Btn_Select_Role::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_First"));
		this->Set_Parent(pParent, false);
		m_bActiveByParent = false;
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	// active & fade
	if (!m_bActiveSelf && m_pParent->Get_IsRender())
		return OBJ_NOEVENT;
	if (!m_bPreActive) {
		m_bFade = true;
		m_vSpread.y = 0.f;	 
	}
	if (m_bFade) 
		this->Set_Fade(true, 0.f, 0.5f, (_float)dTimeDelta, !m_bPreActive);
	else {
		if (__super::IsCollisionEnter(m_pCollider, m_pRectTransform)) {
			m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_highlight"), 4, -1, 0.08f, FMOD_2D);
		}
		if (__super::IsCollisionStay(m_pCollider, m_pRectTransform)) {
			m_vColor.w += (_float)dTimeDelta * 3.f;
			if (m_vColor.w >= m_vAlphaLim.y)
				m_vColor.w = m_vAlphaLim.y;
			m_vSpread.y += (_float)dTimeDelta;
			if (1.f <= m_vSpread.y)
				m_vSpread.y = 1.f;

			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_select_press_bones"), 3, -1, 0.25f, FMOD_2D);
			}
			if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
				m_pAudio->Play_Sound(TEXT("sfx_ui_select_release_01"), -1, 0.2f, FMOD_2D);
				m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_select_release_bones"), 3, -1, 0.2f, FMOD_2D);
				CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));

				//Network
				CInfo* pInfo = CInfo::Get_Instance();
				switch (m_iType) {
				case 2: { //Slasher
					_uint iSlasherSlot = pInfo->Get_SlasherSlot();
					if (iSlasherSlot < 1) {
						pCamera->Set_State(static_cast<CCamera_Lobby::ECAMSTATE>(m_iType));
						pCamera->Set_Group(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), static_cast<CCamera_Lobby::ECAMSTATE>(m_iType));
						m_vColor.w = m_vAlphaLim.x; // alpha reset;

						//pInfo->Add_SlasherSlot();
						pInfo->Set_Player(true);
						//pInfo->Set_Camper(CInfo::ECamper::END);
						//pInfo->Set_Slasher(CInfo::ESlasher::HUNTRESS);
					}
					break;
				}
				case 3:  //Camper
					_uint iCamperSlot = pInfo->Get_CamperSlot();
					if (iCamperSlot < 4) {
						pCamera->Set_State(static_cast<CCamera_Lobby::ECAMSTATE>(m_iType));
						pCamera->Set_Group(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), static_cast<CCamera_Lobby::ECAMSTATE>(m_iType));
						m_vColor.w = m_vAlphaLim.x; // alpha reset;

						pInfo->Add_CamperSlot(1);
						pInfo->Set_MyCamperSlot(iCamperSlot);
						pInfo->Set_Player(false);
						//pInfo->Set_Slasher(CInfo::ESlasher::END);
						//pInfo->Set_Camper(CInfo::ECamper::DWIGHT);
					}
					break;
				}
			}
		}
		// after click
		else {
			m_vColor.w -= (_float)dTimeDelta * 3.f;
			if (m_vColor.w <= m_vAlphaLim.x)
				m_vColor.w = m_vAlphaLim.x;
			m_vSpread.y -= (_float)dTimeDelta * 2.f;
			if (0.f >= m_vSpread.y)
				m_vSpread.y = 0.f;
		}

	}

	return OBJ_NOEVENT;
}

void CUI_Lobby_Btn_Select_Role::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	ImGui::Indent(16.0f);
	ImGui::Unindent(16.0f);

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
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, (_uint)(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d", ImGuiInputTextFlags_ReadOnly);

	if (ImGui::Button("button")) {
		m_iType = iTemp;
		CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
		pCamera->Add_LobbyBtn(static_cast<CCamera_Lobby::ECAMSTATE>(m_iType), this);
	}
	ImGui::SameLine();
	ImGui::SliderInt("button", (_int*)&m_iType, 0, (_uint)(CCamera_Lobby::ECAMSTATE::END)-1, "btn = %d", ImGuiInputTextFlags_ReadOnly);

	__super::Update_Edit_Images();
}

void CUI_Lobby_Btn_Select_Role::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Btn_Select_Role::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if ((nullptr != m_pParent && m_pParent->Get_IsRender()) || !m_bActiveSelf)
		return hr;
	//m_vColor.w = m_pParent->Get_Color().w;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_vecImages) {
		if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
			continue;
#ifdef _DEBUG
		if (iter.bCollider)
			iter.pCollider->Render();
#endif // _DEBUG
		// iter -> true
		if (iter.bActByParent /*&& ETexType::FRAME != static_cast<ETexType>(iter.iDepth)*/)
			iter.vColor.w = m_vColor.w;

		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		if (EUIPass::PASS_SPREAD == static_cast<EUIPass>(iter.iPass))
			m_pShader->Set_RawValue("g_vSpread", &m_vSpread, sizeof(_float2));
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
	}

	return hr;
}

const HRESULT CUI_Lobby_Btn_Select_Role::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	root["Type"] = m_iType;
	return S_OK;
}

void CUI_Lobby_Btn_Select_Role::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	m_iType = root["Type"].asInt();
	//m_pRectTransform->Set_State(CRectTransform::EState::POS, XMLoadFloat3(&m_pRectTransform->Get_Desc().vPosition) * m_fAtt);
	m_vColor.w = 0.f;

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);
	pCamera->Add_LobbyBtn(static_cast<CCamera_Lobby::ECAMSTATE>(m_iType), this);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (CCamera_Lobby::ECAMSTATE::SELECT_SLASHER == static_cast<CCamera_Lobby::ECAMSTATE>(m_iType)) {
		m_vecImages.reserve(static_cast<_uint>(ETexType::END));
		SImageDesc* pImage = nullptr;
		// frm
		pImage = Add_Image(L"btn_back_frm", CRectTransform::EEdge::CEN, false);
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->iDepth = static_cast<_uint>(ETexType::FRAME);
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_SPREAD);
		pImage->vColor.w = 0.3f;
		pImage = nullptr;
		// icon
		pImage = Add_Image(L"sel_role_slasher");
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->pRectTransform->Set_Size(28.f, 90.f);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(-33.f, 0.f, 0.f, 0.f));
		pImage->iDepth = static_cast<_uint>(ETexType::ICON);
		pImage = nullptr;
		// text
		pImage = Add_Image(L"text_first_slasher");
		pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(10.f, 0.f, 0.f, 0.f));
		pImage->iDepth = static_cast<_uint>(ETexType::TEXT);
	}
	else if (CCamera_Lobby::ECAMSTATE::START_CAMPER == static_cast<CCamera_Lobby::ECAMSTATE>(m_iType)) {
		m_vecImages.reserve(static_cast<_uint>(ETexType::END));
		SImageDesc* pImage = nullptr;
		// frm
		pImage = Add_Image(L"btn_back_frm", CRectTransform::EEdge::CEN, false);
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->iDepth = static_cast<_uint>(ETexType::FRAME);
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_SPREAD);
		pImage->vColor.w = 0.3f;
		pImage = nullptr;
		// icon
		pImage = Add_Image(L"sel_role_camper");
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->pRectTransform->Set_Size(30.f, 90.f);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(-33.f, 0.f, 0.f, 0.f));
		pImage->iDepth = static_cast<_uint>(ETexType::ICON);
		pImage = nullptr;
		// text
		pImage = Add_Image(L"text_first_camper");
		pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(10.f, 0.f, 0.f, 0.f));
		pImage->iDepth = static_cast<_uint>(ETexType::TEXT);
	}

	sort(m_vecImages.begin(), m_vecImages.end(), [&](SImageDesc sSour, SImageDesc sDesc)->_bool { return sSour.iDepth < sDesc.iDepth; });
}
