#include "pch.h"
#include "UI_Lobby_Panel_Camper_Choice.h"
#include "Camera_Lobby.h"
#include "Camper_Lobby.h"

CUI_Lobby_Panel_Camper_Choice* const CUI_Lobby_Panel_Camper_Choice::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Panel_Camper_Choice* pInstnace = new CUI_Lobby_Panel_Camper_Choice(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Panel_Camper_Choice::Clone(void* const& pArg) {
	CUI_Lobby_Panel_Camper_Choice* pInstnace = new CUI_Lobby_Panel_Camper_Choice(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Panel_Camper_Choice::CUI_Lobby_Panel_Camper_Choice(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Panel_Camper_Choice::CUI_Lobby_Panel_Camper_Choice(const CUI_Lobby_Panel_Camper_Choice& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Panel_Camper_Choice::Delete(void) {
	__super::Delete();
	for (auto& iter : m_pFXTexture)
		Safe_Release(iter);
	m_vecHuds.clear();
}

const HRESULT CUI_Lobby_Panel_Camper_Choice::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Lobby_Panel_Camper_Choice::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(L"choice_panel_camper");
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_pFXTexture[0] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"choice_2fx").c_str()));
	m_pFXTexture[1] = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"choice_3edge").c_str()));
	return hr;
}

const _ubyte CUI_Lobby_Panel_Camper_Choice::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Lobby_Panel_Camper"));
		this->Set_Parent(pParent, false);
		m_bActiveByParent = false;
		// set first
		//CInfo* pInfo = CInfo::Get_Instance();
		//CCamper_Lobby* pCamper = dynamic_cast<CCamper_Lobby*>(pGame_Instance->Get_Object_CloneControl(static_cast<_ubyte>(EScene::LOBBY), TEXT("Camper_Lobby")));
		//pCamper->Set_Camper(CCamper_Lobby::ECamper::DWIGHT);
		//pInfo->Set_Camper(CInfo::ECamper::DWIGHT);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	_uint iIdx = 0;

	for (auto& iter : m_vecHuds) {

		
		if (__super::IsCollisionEnter(iter.pImage->pCollider, iter.pImage->pRectTransform)) {
			m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_highlight"), 4, -1, 0.08f, FMOD_2D);
		}
		if (__super::IsCollisionStay(iter.pImage->pCollider, iter.pImage->pRectTransform)) {
			iter.vSpread.y += (_float)dTimeDelta;
			if (1.f <= iter.vSpread.y)
				iter.vSpread.y = 1.f;

			// choice
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				iter.bDown = true;

			}
			else if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L) && (!iter.bClick)) {
				CInfo* pInfo = CInfo::Get_Instance();
				CCamper_Lobby* pCamper = dynamic_cast<CCamper_Lobby*>(pGame_Instance->Get_Object_CloneControl(static_cast<_ubyte>(EScene::LOBBY), TEXT("Camper_Lobby")));
				switch (iter.iIndex) {
				case 0: {
					pCamper->Set_Camper(CCamper_Lobby::ECamper::DWIGHT);
					pInfo->Set_Camper(CInfo::ECamper::DWIGHT);
					m_iChoice = iter.iIndex;
					break;
				}
				case 1: {
					pCamper->Set_Camper(CCamper_Lobby::ECamper::MEG);
					pInfo->Set_Camper(CInfo::ECamper::MEG);
					m_iChoice = iter.iIndex;
					break;
				}
				case 2: {
					pCamper->Set_Camper(CCamper_Lobby::ECamper::BILL);
					pInfo->Set_Camper(CInfo::ECamper::BILL);
					m_iChoice = iter.iIndex;
					break;
				}
				case 3: {
					pCamper->Set_Camper(CCamper_Lobby::ECamper::CHERYL);
					pInfo->Set_Camper(CInfo::ECamper::CHERYL);
					m_iChoice = iter.iIndex;
					break;
				}
				}

				m_pAudio->Play_Sound(TEXT("sfx_ui_equip_01"), -1, 0.2f, FMOD_2D);
				iter.bDown = false;
				iter.bClick = true;
				for (auto& value : m_vecHuds) {
					if (&iter == &value)
						continue;
					value.bClick = false;
				}
			}
		}
		else {
			iter.bDown = false;
			iter.vSpread.y -= (_float)dTimeDelta * 5.f;
			if (0.f >= iter.vSpread.y)
				iter.vSpread.y = 0.f;
		}
		iIdx++;
	}


	_uint j = 0;
	for (auto& value : m_vecNameTags)
		(j++ == m_iChoice) ? value->bRender = true : value->bRender = false;

	return OBJ_NOEVENT;
}

void CUI_Lobby_Panel_Camper_Choice::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

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
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, static_cast<_int>(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d");

	__super::Update_Edit_Images();
}

void CUI_Lobby_Panel_Camper_Choice::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Panel_Camper_Choice::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (nullptr != m_pParent)
		m_vColor.w = m_pParent->Get_Color().w;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_vecNameTags) {
		if (iter->IsContinue())
			continue;
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
		hr |= iter->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, iter->iPass);
	}

	for (auto& iter : m_vecHuds) {
		SImageDesc* pImage = iter.pImage;
		if (pImage->IsContinue())
			continue;

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		hr |= pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pImage->iPass);
#ifdef _DEBUG
		if (pImage->bCollider)
			pImage->pCollider->Render();
#endif // _DEBUG

		for (_uint i = 0; i < 2; ++i) {
			if (nullptr == m_pFXTexture[i])
				continue;
			_uint iPass = pImage->iPass;
			if (i == 0) {
				iPass = static_cast<_uint>(EUIPass::PASS_SPREAD);
				m_pShader->Set_RawValue("g_vSpread", &iter.vSpread, sizeof(_float2));
			}
			else {
				if (!iter.bClick)
					continue;
			}
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
			hr |= m_pFXTexture[i]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, iPass);
		}
	}

	return hr;
}

const HRESULT CUI_Lobby_Panel_Camper_Choice::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	root["Type"] = m_iType;
	Save_AddData(L"UI_Lobby_Panel_Camper_Choice");
	return S_OK;
}
 
void CUI_Lobby_Panel_Camper_Choice::Load_Data(Json::Value& root){
	__super::Load_Data(root);
	m_iType = root["Type"].asInt();
	Set_SizeOfTexture(m_pTexture, m_pRectTransform);

	_uint iCharNum = static_cast<_uint>(CInfo::ECamper::END);
	Load_AddData(L"UI_Lobby_Panel_Camper_Choice", iCharNum);

	//SImageDesc* pImage = nullptr;
	//pImage = Add_Image(L"choice_icon_cam_dwi",1, true);
	//pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
	//pImage = Add_Image(L"choice_icon_cam_meg", 1, true);
	//pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
	//pImage = Add_Image(L"choice_icon_cam_bill", 1, true);
	//pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
	//pImage = Add_Image(L"choice_icon_cam_cheryl", 1, true);
	//pImage->pRectTransform->Set_Parent(m_pRectTransform, false);

	m_vecHuds.reserve(m_vecImages.size());
	_uint iIdx = 0;
	for (auto& iter : m_vecImages) {
		SHudFX sHud;	
		sHud.pImage = &iter;
		(iIdx == 0) ? (sHud.bClick = true) : (sHud.bClick = false);
		sHud.iIndex = iIdx++;
		m_vecHuds.emplace_back(sHud);
	}

	m_vecNameTags.reserve(iCharNum);
	for (_uint i = 0; i < iCharNum; ++i) {
		_tchar szKey[100] = L"loadout_name_cam";
		_tchar szBuf[8] = L"";
		_itow_s(i + 1, szBuf, 10);
		lstrcat(szKey, szBuf);
		SImageDesc* pImage = Add_Image(szKey, 0, true, CRectTransform::EEdge::LTOP);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(310.f, 60.f, 0.f));
		pImage->bSave = false;
		m_vecNameTags.emplace_back(pImage);
	}
}
