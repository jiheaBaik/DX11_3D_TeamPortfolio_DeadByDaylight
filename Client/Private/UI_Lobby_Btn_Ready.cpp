#include "pch.h"
#include "UI_Lobby_Btn_Ready.h"
#include "Camera_Lobby.h"

_uint CUI_Lobby_Btn_Ready::s_iReadyCnt = 0;

CUI_Lobby_Btn_Ready* const CUI_Lobby_Btn_Ready::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Btn_Ready* pInstnace = new CUI_Lobby_Btn_Ready(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Btn_Ready::Clone(void* const& pArg) {
	CUI_Lobby_Btn_Ready* pInstnace = new CUI_Lobby_Btn_Ready(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Btn_Ready::CUI_Lobby_Btn_Ready(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Btn_Ready::CUI_Lobby_Btn_Ready(const CUI_Lobby_Btn_Ready& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Btn_Ready::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Btn_Ready::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Btn_Ready::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("sel_btn_ready"));	
	m_iType = 1;
	m_iSort = 1;
	return hr;
}

const _ubyte CUI_Lobby_Btn_Ready::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = nullptr;
		(m_iGroup == static_cast<_uint>(CCamera_Lobby::ECAMSTATE::SELECT_SLASHER)) ? pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_Slasher")) :
			pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_Camper"));
		m_bActiveByParent = false;
		this->Set_Parent(pParent, false);
		this->Set_Active(false);
		m_bStart = true;
	}
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;
	__super::Update(dTimeDelta);

	CInfo* pInfo = CInfo::Get_Instance();
	if (__super::IsCollisionEnter(m_pCollider, m_pRectTransform)) {
		m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_blood_action"), 10, -1, 0.2f, FMOD_2D);
	}
	if (__super::IsCollisionStay(m_pCollider, m_pRectTransform)) {
		if (5 > m_iFXIdx && this->Invoke(dTimeDelta, 0.005f)) {
			SImageDesc* pImage = &m_vecImages[static_cast<_uint>(ETexType::FX) + m_iFXIdx++];
			if (ETexType::FX == static_cast<ETexType>(pImage->iDepth))
				pImage->bRender = true;
		}
		if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
			m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_select_press_bones"), 3, -1, 0.25f, FMOD_2D);
		}
		if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
			m_pAudio->Play_Sound(TEXT("sfx_ui_stadia_crowd_choice_killer_01"), -1, 0.25f, FMOD_2D);

			if (!m_bReady) {
				m_vecImages[(_uint)ETexType::FX + 5].bRender = false;
				m_vecImages[(_uint)ETexType::FX + 6].bRender = true;
				//++s_iReadyCnt;
				pInfo->Add_Ready(1);
				pInfo->Set_MyReady(false);
				m_bReady = true;

				CInven* pInven = CInven::Get_Instance();
				if (pInfo->Get_Player()) {
					pInven->Ready_Perk_Slasher();
				}
				else {
					pInven->Ready_Perk_Camper();
				}
				return OBJ_NOEVENT;
			}
			else {
				m_vecImages[(_uint)ETexType::FX + 5].bRender = true;
				m_vecImages[(_uint)ETexType::FX + 6].bRender = false;
				//--s_iReadyCnt;
				pInfo->Add_Ready(-1);
				pInfo->Set_MyReady(true);
				m_bReady = false;
				return OBJ_NOEVENT;
			}
		}
		//	Ready_Perk_Camper();
		//  Ready_Perk_Slasher();
	}
	else {
		if (1 < m_iFXIdx && this->Invoke(dTimeDelta, 0.02f)) {
			SImageDesc* pImage = &m_vecImages[static_cast<_uint>(ETexType::FX) + --m_iFXIdx];
			if (ETexType::FX == static_cast<ETexType>(pImage->iDepth))
				pImage->bRender = false;
		}
	}

	return OBJ_NOEVENT;
}

void CUI_Lobby_Btn_Ready::Update_Edit(void) {
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

	if (ImGui::Button("type")) {
		m_iType = iTemp;
		CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
		pCamera->Add_LobbyBtn(static_cast<CCamera_Lobby::ECAMSTATE>(m_iType), this);
	}
	ImGui::SameLine();
	ImGui::SliderInt("button", (_int*)&m_iType, 0, (_uint)(CCamera_Lobby::ECAMSTATE::END)-1, "btn = %d");

	ImGui::InputInt("ready cnt", (_int*)&s_iReadyCnt);

	__super::Update_Edit_Images();
}

void CUI_Lobby_Btn_Ready::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Btn_Ready::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
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
			if (iter.bCollider)
				iter.pCollider->Render();
			m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
			iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			m_pVBRect->Render(m_pShader, iter.iPass);
		}
	}
	return hr;
}

const HRESULT CUI_Lobby_Btn_Ready::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_Lobby_Btn_Ready::Load_Data(Json::Value& root){
	__super::Load_Data(root);	// load group

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_vecImages.reserve(static_cast<_uint>(ETexType::END));
	SImageDesc* pDesc = nullptr;
	// frame
	pDesc = Add_Image(L"sel_btn_ready_frm", static_cast<_uint>(ETexType::FRAME));
	pDesc->pRectTransform->Set_Parent(m_pRectTransform);
	pDesc->pRectTransform->Set_Size(60.f, 80.f);
	pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(16.f, 0.f, 0.f));
	pDesc = nullptr;
	// icon
	pDesc = Add_Image(L"sel_btn_ready_icon", static_cast<_uint>(ETexType::ICON));
	pDesc->pRectTransform->Set_Parent(m_pRectTransform);
	pDesc->pRectTransform->Set_Size(3.f, 60.f);
	pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(42.f, 0.f, 0.f));
	pDesc = nullptr;
	// fx
	for (_uint i = 0; i < 5; ++i) {
		pDesc = Add_Image(L"sel_btn_ready_fx", static_cast<_uint>(ETexType::FX), (0 == i) ? true : false, CRectTransform::EEdge::RCOR);
		pDesc->pRectTransform->Set_Parent(m_pRectTransform);
		pDesc->pRectTransform->Set_Size(30, 100.f - (i * 10.f));
		pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-80.f - (15.f * i), 0.f, 0.f));
		pDesc = nullptr;
	}
	// text
	pDesc = Add_Image(L"text_sel_ready", static_cast<_uint>(ETexType::TEXT1));
	pDesc->pRectTransform->Set_Anchor(m_pRectTransform->Get_Desc().vAnchor);
	pDesc->pRectTransform->Set_Pivot(m_pRectTransform->Get_Desc().vPivot);
	pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, XMLoadFloat3(&m_pRectTransform->Get_Desc().vPosition) + XMVectorSet(-20.f, -18.f, 0.f, 0.f));
	pDesc->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.9f, 0.9f, 1.f));
	pDesc = nullptr;
	// text
	pDesc = Add_Image(L"text_sel_ready_cancel", static_cast<_uint>(ETexType::TEXT2), false);
	pDesc->pRectTransform->Set_Anchor(m_pRectTransform->Get_Desc().vAnchor);
	pDesc->pRectTransform->Set_Pivot(m_pRectTransform->Get_Desc().vPivot);
	pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, XMLoadFloat3(&m_pRectTransform->Get_Desc().vPosition) + XMVectorSet(-20.f, -18.f, 0.f, 0.f));
	pDesc->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.9f, 0.9f, 1.f));
	pDesc = nullptr;

	sort(m_vecImages.begin(), m_vecImages.end(), [&](SImageDesc sSour, SImageDesc sDesc)->_bool { return sSour.iDepth < sDesc.iDepth; });
}
