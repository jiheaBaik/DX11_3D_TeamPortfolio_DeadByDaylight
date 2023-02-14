#include "pch.h"
#include "UI_Play_Panel_First.h"
#include "Camera_Camper.h"
#include "Camera_Slasher.h"
#include "UI_Play_Panel_Manager.h"

CUI_Play_Panel_First* const CUI_Play_Panel_First::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Panel_First* pInstnace = new CUI_Play_Panel_First(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Panel_First::Clone(void* const& pArg) {
	CUI_Play_Panel_First* pInstnace = new CUI_Play_Panel_First(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Panel_First::CUI_Play_Panel_First(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Panel_First::CUI_Play_Panel_First(const CUI_Play_Panel_First& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Panel_First::Delete(void) {
	__super::Delete();
	Safe_Release(m_pDissolveTexture);
}

const HRESULT CUI_Play_Panel_First::Init_Create(void) {
	HRESULT hr = S_OK;
	m_iGroup = 1;
	return hr;
}

const HRESULT CUI_Play_Panel_First::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("back"));

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	__super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(L"test_dissolve").c_str(), TEXT("Texture_Dissolve"), reinterpret_cast<CComponent*&>(m_pDissolveTexture));
	m_fDissolve = 0.f;
	return hr;
}

const _ubyte CUI_Play_Panel_First::Update(const _double& dTimeDelta) {
	if (!m_bStart) {
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

//#ifdef _DEBUG
	// intro skip
	if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_F6)) {
		CObject* pEditCam = pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"Camera_Edit");
		pGame_Instance->Delete_Object_Clone(pEditCam);
		CCamera* pCamera = CCamera::Get_Camera(TEXT("Camper"));
		pCamera->Set_Camera(0);
	}
//#endif // _DEBUG

	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player()) {
		CCamera_Slasher* pCamera = dynamic_cast<CCamera_Slasher*>(CCamera::Get_Camera(static_cast<_uint>(0)));
		if (nullptr == pCamera)
			return OBJ_NOEVENT;
		if (!m_bActiveSelf && !pCamera->Get_IntroEnd()) {
			Set_Active(true);
			return OBJ_NOEVENT;
		}

		if (pCamera->Get_AlmostFin()) 
			m_fDissolve += (_float)dTimeDelta * 0.3f;
		if (m_bActiveSelf && pCamera->Get_IntroEnd()) {
			m_fDissolve = 1.f;
			dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), L"UI_Play_Panel_Second"))->Set_Active(true);
			//dynamic_cast<CUI_Play_Panel_Manager*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Panel_Manager"))->Set_Panel(CUI_Play_Panel_Manager::EPanel::PLAY2, true, CUI_Play_Panel_Manager::EPanel::PLAY1);
			Set_Active(false);
			return OBJ_NOEVENT;
		}
	}
	else {
		CCamera_Camper* pCamera = dynamic_cast<CCamera_Camper*>(CCamera::Get_Camera(static_cast<_uint>(0)));
		if (nullptr == pCamera)
			return OBJ_NOEVENT;
		if (!m_bActiveSelf && !pCamera->Get_IntroEnd()) {
			Set_Active(true);
			return OBJ_NOEVENT;
		}

		if (pCamera->Get_AlmostFin()) 
			m_fDissolve += (_float)dTimeDelta * 0.3f;
		if (m_bActiveSelf && pCamera->Get_IntroEnd()) {
			m_fDissolve = 1.f;
			Set_Active(false);
			dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), L"UI_Play_Panel_Second"))->Set_Active(true);
			return OBJ_NOEVENT;
		}
	}
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	if (1.f <= m_fDissolve)
		m_fDissolve = 1.f;
	// alpha
	if (!m_bPreActive) {
		m_vColor.w = 1.f;
		m_fDissolve = 0.f;
	}
	//m_vColor.w -= (_float)dTimeDelta * 0.5f;
	//if (0 >= m_vColor.w)
	//	m_vColor.w = 0.f;
	this->Set_Fade(false, 0.f, 1.f, (_float)dTimeDelta * 0.5f, false);


	return OBJ_NOEVENT;
}

void CUI_Play_Panel_First::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();
	Update_Edit_Images();
}

void CUI_Play_Panel_First::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Play_Panel_First::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_vecImages) {
		if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
			continue;
		if (iter.bCollider)
			iter.pCollider->Render();
		//if (ETexType::TEXT == static_cast<ETexType>(iter.iDepth))
		//	iter.vColor.w = (1.0f - m_vColor.w);			

		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		if (EUIPass::PASS_DISSOLVE == static_cast<EUIPass>(iter.iPass) && nullptr != m_pDissolveTexture) {
			iter.vColor.w = (1.0f - m_vColor.w);
			m_pDissolveTexture->Set_Resource(m_pShader, "g_DissolveTexture", 0);
			hr |= m_pShader->Set_RawValue("g_fDissolve", &m_fDissolve, sizeof(_float));
		}
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
	}

	return hr;
}

const HRESULT CUI_Play_Panel_First::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	return S_OK;
}

void CUI_Play_Panel_First::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	//m_pRectTransform->Set_Size((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	m_pRectTransform->Set_Size(1920.f, 1080.f);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pDesc = nullptr;
	// fame
	pDesc = Add_Image(L"play_back_frame", static_cast<_int>(ETexType::FRM), true);
	//pDesc->pRectTransform->Set_Size((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	pDesc->pRectTransform->Set_Size(1920.f, 1080.f);
	// text
	pDesc = nullptr;
	pDesc = Add_Image(L"play_txt_map", static_cast<_int>(ETexType::TEXT), true, CRectTransform::EEdge::LBOT);
	pDesc->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(60.f, -78.f, 0.f));
	pDesc->iPass = static_cast<_uint>(EUIPass::PASS_DISSOLVE);

	this->Sort_Images();
}
