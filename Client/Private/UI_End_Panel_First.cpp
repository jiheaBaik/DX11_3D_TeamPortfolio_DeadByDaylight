#include "pch.h"
#include "UI_End_Panel_First.h"
#include "UI_Play_Gauge_Exit.h"

CUI_End_Panel_First* const CUI_End_Panel_First::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_End_Panel_First* pInstnace = new CUI_End_Panel_First(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_End_Panel_First::Clone(void* const& pArg) {
	CUI_End_Panel_First* pInstnace = new CUI_End_Panel_First(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_End_Panel_First::CUI_End_Panel_First(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_End_Panel_First::CUI_End_Panel_First(const CUI_End_Panel_First& rhs)
	: CUI_Image(rhs) {
}

void CUI_End_Panel_First::Delete(void) {
	__super::Delete();
	Safe_Release(m_pDissolveTexture);
}

const HRESULT CUI_End_Panel_First::Init_Create(void) {
	HRESULT hr = S_OK;
	// 결과텍스트 통으로 만들기 + 정리
	return hr;
}

const HRESULT CUI_End_Panel_First::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(L"end_panel_back_fx");

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"test_dissolve").c_str(), TEXT("Texture_Dissolve"), reinterpret_cast<CComponent*&>(m_pDissolveTexture));
	return hr; 
}

const _ubyte CUI_End_Panel_First::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf) {
		return OBJ_NOEVENT;
	}	
	if (!m_bPreActive) {
#ifdef _DEBUG
		CInfo* pInfo = CInfo::Get_Instance();
		if (pInfo->Get_Player())
			this->Set_State(static_cast<_uint>(CUI_End_Panel_First::ESlasher::PEACEFUL));
		else
			this->Set_State(static_cast<_uint>(CUI_End_Panel_First::ECamper::SACRIFICE));
#endif // _DEBUG

		//dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"))->Set_Active(false);
		dynamic_cast<CUI_Play_Gauge_Exit*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Exit"))->Set_Active(false);
		m_fDissolve = 1.f;
	}
	m_fDissolve -= (_float)dTimeDelta * 0.5f;
	if (0 >= m_fDissolve)
		m_fDissolve = 0.f;

	return OBJ_NOEVENT;
}

void CUI_End_Panel_First::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	if (ImGui::Button("bind") && 0 <= m_iType)
		this->Set_State(m_iType, false);
	ImGui::SameLine();
	ImGui::InputInt("type", &m_iType);
		
	Update_Edit_Images();
}

void CUI_End_Panel_First::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_End_Panel_First::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_vecImages) {
		if (iter.IsContinue())
			continue;
		//if (iter.bActByParent)
		//	iter.vColor.w = m_vColor.w;

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		if (EUIPass::PASS_DISSOLVE == static_cast<EUIPass>(iter.iPass) && nullptr != m_pDissolveTexture) {
			m_pDissolveTexture->Set_Resource(m_pShader, "g_DissolveTexture", 0);
			hr |= m_pShader->Set_RawValue("g_fDissolve", &m_fDissolve, sizeof(_float));
		}
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
#ifdef _DEBUG
		if (iter.bCollider)
			iter.pCollider->Render();
#endif // _DEBUG

	}
	return hr;
}

void CUI_End_Panel_First::Set_State(const _uint& iState, const _bool& bOneOff, const _bool& bActive) {
	if (m_bOneOff || m_vecImages.size() <= iState)
		return;
	_uint i = 0;
	for (auto& iter : m_vecImages) {
		if (0 > iter.iDepth) { // on
			iter.bRender = true;
			continue;
		}			
		(i++ == iState) ? iter.bRender = true : iter.bRender = false;
	}
	if (bActive) {
		this->Set_Active(bActive);
	}	

	m_bOneOff = bOneOff;
}

const HRESULT CUI_End_Panel_First::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_End_Panel_First::Load_Data(Json::Value& root) {
	__super::Load_Data(root);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr;
	// result
	pImage = Add_Image(L"end_txt_result", -1, false, CRectTransform::EEdge::LTOP);
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(47.f, 200.f, 0.f));
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(1.2f, 1.2f, 1.2f));
	pImage->iPass = static_cast<_uint>(EUIPass::PASS_DISSOLVE);

	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player()) {
		pImage = Add_Image(L"end_panel_sla_mur", static_cast<_int>(ESlasher::MURDER), false, CRectTransform::EEdge::LTOP);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(50.f, 230.f, 0.f));
		pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.8f, 0.8f, 0.8f));
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_DISSOLVE);
		pImage->bSave = false;

		pImage = Add_Image(L"end_panel_sla_peace", static_cast<_int>(ESlasher::PEACEFUL), false, CRectTransform::EEdge::LTOP);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(50.f, 230.f, 0.f));
		pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.8f, 0.8f, 0.8f));
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_DISSOLVE);
		pImage->bSave = false;
	}
	else {
		pImage = Add_Image(L"end_txt_cam_esc", static_cast<_int>(ECamper::ESCAPE) , false, CRectTransform::EEdge::LTOP);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(50.f, 230.f, 0.f));
		pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.8f, 0.8f, 0.8f));
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_DISSOLVE);
		pImage->bSave = false;

		pImage = Add_Image(L"end_txt_cam_sac", static_cast<_int>(ECamper::SACRIFICE), false,  CRectTransform::EEdge::LTOP);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(50.f, 230.f, 0.f));
		pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.8f, 0.8f, 0.8f));
		pImage->iPass = static_cast<_uint>(EUIPass::PASS_DISSOLVE);
		pImage->bSave = false;
	}

	this->Sort_Images();
}
