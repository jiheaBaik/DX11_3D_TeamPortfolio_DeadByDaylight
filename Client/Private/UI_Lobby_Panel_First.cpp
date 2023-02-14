#include "pch.h"
#include "UI_Lobby_Panel_First.h"
#include "Camera_Lobby.h"

CUI_Lobby_Panel_First* const CUI_Lobby_Panel_First::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Panel_First* pInstnace = new CUI_Lobby_Panel_First(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Panel_First::Clone(void* const& pArg) {
	CUI_Lobby_Panel_First* pInstnace = new CUI_Lobby_Panel_First(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Panel_First::CUI_Lobby_Panel_First(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Panel_First::CUI_Lobby_Panel_First(const CUI_Lobby_Panel_First& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Panel_First::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Lobby_Panel_First::Init_Create(void) {
	HRESULT hr = S_OK;
	m_iGroup = 1;
	return hr;
}

const HRESULT CUI_Lobby_Panel_First::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("black"));
	m_iGroup = 1;
	return hr;
}

const _ubyte CUI_Lobby_Panel_First::Update(const _double& dTimeDelta) {
	if (!m_bStart) {
		Set_Active(true);
		m_bFade = true;
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;
	// fade out
	if (m_bRender) {
		this->Set_Fade(false, 0.f, 1.f, (_float)dTimeDelta * 0.3f);
		if (!m_bFade)
			m_bRender = false;
	}
	// fade in
	else {
		// first input
		CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
		if (!m_vecImages[0].bActByParent && pCamera->Get_IsMoveEnd()) {
			if (m_vecImages[0].vColor.w <= 0.5f)
				m_vecImages[0].vColor.w += (_float)dTimeDelta;
			else {
				m_vecImages[0].vColor.w = 0.5f;
				m_vecImages[0].bActByParent = true;
			}			
		}

		this->Set_Fade(true, 0.f, 0.5f, (_float)dTimeDelta, !m_bPreActive);
	}

	return OBJ_NOEVENT;
}

void CUI_Lobby_Panel_First::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	static _int iTemp = m_iGroup;
	const char* combo_uitype = CCamera_Lobby::ECamState_ToString(static_cast<CCamera_Lobby::ECAMSTATE>(iTemp));
	if (ImGui::BeginCombo("group type", combo_uitype)) {
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
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, (_uint)(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d");

	Update_Edit_Images();
}

void CUI_Lobby_Panel_First::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Panel_First::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	__super::Set_RawValue(m_pShader);

	if (m_bRender) {
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &m_pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
		hr |= m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, m_iPass);
	}
	else {
		for (auto& iter : m_vecImages) {
			if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
				continue;
#ifdef _DEBUG
			if (iter.bCollider)
				iter.pCollider->Render();
#endif // _DEBUG
			if (iter.bActByParent && m_bFade)
				iter.vColor.w = m_vColor.w;

			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
			hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, iter.iPass);
		}
	}

	return hr;
}

const HRESULT CUI_Lobby_Panel_First::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_Lobby_Panel_First::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	//m_pRectTransform->Set_Size(_float(g_iWinSizeX), _float(g_iWinSizeY));
	m_pRectTransform->Set_Size(1920.f, 1080.f);
	m_vColor.w = 1.f;

	CCamera_Lobby* pCamera = dynamic_cast<CCamera_Lobby*>(CCamera::Get_Camera(static_cast<_uint>(0)));
	pCamera->Add_UIGroup(static_cast<CCamera_Lobby::ECAMSTATE>(m_iGroup), this, false);
	
	SImageDesc* pImage = nullptr;
	pImage = Add_Image(L"panel_first", 0, true, CRectTransform::EEdge::LTOP);
	pImage->bActByParent = false;
	pImage->pRectTransform->Set_Size(1000.f, 2000.f);
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, XMVectorSet(-500.f, -500.f, 0.f, 0.f));
	pImage->vColor = { .5f, .5f, .5f, 0.f };
}

