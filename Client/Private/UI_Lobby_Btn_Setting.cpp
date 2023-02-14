#include "pch.h"
#include "UI_Lobby_Btn_Setting.h"
#include "Camera_Lobby.h"

CUI_Lobby_Btn_Setting* const CUI_Lobby_Btn_Setting::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Btn_Setting* pInstnace = new CUI_Lobby_Btn_Setting(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Btn_Setting::Clone(void* const& pArg) {
	CUI_Lobby_Btn_Setting* pInstnace = new CUI_Lobby_Btn_Setting(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Btn_Setting::CUI_Lobby_Btn_Setting(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Btn_Setting::CUI_Lobby_Btn_Setting(const CUI_Lobby_Btn_Setting& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Btn_Setting::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Btn_Setting::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Btn_Setting::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("btn_setting_back"));
	m_iGroup = 1;
	m_iSort = 1;
	m_vAlphaLim = { 0.5f, 0.8f };
	__super::Update(0.02);
	return hr;
}

const _ubyte CUI_Lobby_Btn_Setting::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_First"));
		this->Set_Parent(pParent, false);
		m_bActiveByParent = false;
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf /*&& m_pParent->Get_IsRender()*/)
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

			for (auto& iter : m_vecImages) {
				if (!iter.bRender && (2 == iter.iDepth))
					iter.bRender = true;
			}
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_select_press_bones"), 3, -1, 0.25f, FMOD_2D);
			}
			if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
				m_pAudio->Play_Sound(TEXT("sfx_ui_select_release_01"), -1, 0.2f, FMOD_2D);
				m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_select_release_bones"), 3, -1, 0.2f, FMOD_2D);
				switch (static_cast<EType>(m_iType))
				{
				case Client::CUI_Lobby_Btn_Setting::EType::SETTING: {
					// 환경창 열기
				}
																  break;
				case Client::CUI_Lobby_Btn_Setting::EType::EXIT:
					DestroyWindow(g_hWnd);
					break;
				}
				// alpha reset;
				m_vColor.w = m_vAlphaLim.x;
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

			for (auto& iter : m_vecImages) {
				if (ETexType::TEXT == static_cast<ETexType>(iter.iDepth)) {
					if (iter.bRender)
						iter.bRender = false;
				}
			}
		}
	}
	
	return OBJ_NOEVENT;
}

void CUI_Lobby_Btn_Setting::Update_Edit(void) {
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
	ImGui::SliderInt("group", &m_iGroup, 0, static_cast<_int>(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d");
	ImGui::SliderInt("button", &m_iType, 0, static_cast<_int>(EType::END)-1, "btn = %d");

	__super::Update_Edit_Images();
}

void CUI_Lobby_Btn_Setting::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Btn_Setting::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if ((nullptr != m_pParent && m_pParent->Get_IsRender()) || !m_bActiveSelf)
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
		// iter -> true
		if (iter.bActByParent /*&& ETexType::FRAME != static_cast<ETexType>(iter.iDepth)*/)
			iter.vColor.w = m_vColor.w;

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		if (EUIPass::PASS_SPREAD == static_cast<EUIPass>(iter.iPass))
			m_pShader->Set_RawValue("g_vSpread", &m_vSpread, sizeof(_float2));
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
	}

	return hr;
}

const HRESULT CUI_Lobby_Btn_Setting::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	root["Type"] = m_iType;
	return S_OK;
}

void CUI_Lobby_Btn_Setting::Load_Data(Json::Value& root){
	__super::Load_Data(root);
	m_iType = root["Type"].asInt();
	m_vColor.w = 0.f;

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	switch (static_cast<EType>(m_iType))
	{
	case Client::CUI_Lobby_Btn_Setting::EType::SETTING: {
		m_vecImages.reserve(static_cast<_uint>(ETexType::END));
		SImageDesc* pImage = nullptr;
		// icon
		pImage = Add_Image(L"btn_set_setting", static_cast<_uint>(ETexType::ICON));
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->pRectTransform->Set_Size(80.f, 80.f);
		pImage = nullptr;
		// frm
		pImage = Add_Image(L"sel_btn_frm", static_cast<_uint>(ETexType::FRAME), true, CRectTransform::EEdge::CEN, false);
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_SPREAD);
		pImage->vColor.w = 0.3f;
		pImage = nullptr;
		// text
		pImage = Add_Image(L"text_setting", static_cast<_uint>(ETexType::TEXT), false);
		pImage->pRectTransform->Set_Anchor(m_pRectTransform->Get_Desc().vAnchor);
		pImage->pRectTransform->Set_Pivot(m_pRectTransform->Get_Desc().vPivot);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, (XMLoadFloat3(&m_pRectTransform->Get_Desc().vPosition) + XMVectorSet(30.f, 0.f, 0.f, 0.f)));
		pImage->iPass = 0;
		pImage = nullptr;

		Sort_Images();
	}
		break;
	case Client::CUI_Lobby_Btn_Setting::EType::EXIT: {
		m_vecImages.reserve(static_cast<_uint>(ETexType::END));
		SImageDesc* pImage = nullptr;
		// icon
		pImage = Add_Image(L"btn_set_exit", static_cast<_uint>(ETexType::ICON));
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->pRectTransform->Set_Size(80.f, 80.f);
		pImage = nullptr;
		// frm
		pImage = Add_Image(L"sel_btn_frm", static_cast<_uint>(ETexType::FRAME), true, CRectTransform::EEdge::CEN, false);
		pImage->pRectTransform->Set_Parent(m_pRectTransform);
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_SPREAD);
		pImage->vColor.w = 0.3f;
		pImage = nullptr;
		// text
		pImage = Add_Image(L"text_setting_exit", static_cast<_uint>(ETexType::TEXT), false);
		pImage->pRectTransform->Set_Anchor(m_pRectTransform->Get_Desc().vAnchor);
		pImage->pRectTransform->Set_Pivot(m_pRectTransform->Get_Desc().vPivot);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, (XMLoadFloat3(&m_pRectTransform->Get_Desc().vPosition) + XMVectorSet(30.f, 0.f, 0.f, 0.f)));
		pImage->iPass = 0;
		pImage = nullptr;

		Sort_Images();
	}
		break;
	}

}
