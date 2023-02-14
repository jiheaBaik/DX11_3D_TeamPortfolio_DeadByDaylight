#include "pch.h"
#include "UI_Lobby_Panel_Loadout.h"
#include "Camera_Lobby.h"
#include "UI_Lobby_Panel_Slasher_Choice.h"
#include "UI_Lobby_Panel_Camper_Choice.h"

CUI_Lobby_Panel_Loadout* const CUI_Lobby_Panel_Loadout::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Lobby_Panel_Loadout* pInstnace = new CUI_Lobby_Panel_Loadout(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Lobby_Panel_Loadout::Clone(void* const& pArg) {
	CUI_Lobby_Panel_Loadout* pInstnace = new CUI_Lobby_Panel_Loadout(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Lobby_Panel_Loadout::CUI_Lobby_Panel_Loadout(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Lobby_Panel_Loadout::CUI_Lobby_Panel_Loadout(const CUI_Lobby_Panel_Loadout& rhs)
	: CUI_Image(rhs) {
}

void CUI_Lobby_Panel_Loadout::Delete(void) {
	__super::Delete();
	for (auto& iter : m_sHudFXs)
		Safe_Release(iter.pTexture);	
	for (auto& iter : m_vecHud)
		Safe_Release(iter.pTexture);
	m_vecHud.clear();
}

const HRESULT CUI_Lobby_Panel_Loadout::Init_Create(void) {
	HRESULT hr = S_OK;
	m_iGroup = 2;
	// 설명창 조절
	return hr;
}

const HRESULT CUI_Lobby_Panel_Loadout::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(TEXT("loadout_panel"));
	// fx
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	m_sHudFXs[0].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"loadout_icon_edge").c_str()));
	m_sHudFXs[1].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"loadout_icon_frm").c_str()));
	m_sHudFXs[1].iPass = static_cast<_uint>(EUIPass::PASS_SPREAD);

	return hr;
}

const _ubyte CUI_Lobby_Panel_Loadout::Update(const _double& dTimeDelta) {
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
	//__super::Update(dTimeDelta);
	m_pRectTransform->Update_Desc();
	m_pCollider->Update(m_pRectTransform->Get_Matrix());
	if (!m_vecImages.empty()) {
		for (auto& iter : m_vecImages) {
			if (nullptr == iter.pRectTransform || nullptr == iter.pCollider)
				continue;
			iter.pRectTransform->Update_Desc();

			_float4x4 f4World = iter.pRectTransform->Get_Float4x4();
			_vector vPos = XMLoadFloat3((_float3*)&f4World.m[3][0]) + XMVectorSet(-XMVectorGetX(iter.pRectTransform->Get_DestSize()) * .2f, XMVectorGetY(iter.pRectTransform->Get_DestSize()) * .1f, 0.f, 0.f);
			XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
			_matrix matrix = XMLoadFloat4x4(&f4World) * XMMatrixScaling(0.95f, 0.95f, 0.95f);
			iter.pCollider->Update(matrix);
		}
	}

	if (!m_bActiveSelf /*|| (nullptr != m_pParent && m_pParent->Get_IsFade())*/) {
		return OBJ_NOEVENT;
	}
	// inven
	_uint iInven = 0;
	for (auto& iter : m_sInvens) {
		if (__super::IsCollisionEnter(iter.pImage->pCollider, iter.pImage->pRectTransform)) {
			m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_highlight"), 4, -1, 0.08f, FMOD_2D);
		}
		if (IsCollisionStay(iter.pImage->pCollider, iter.pImage->pRectTransform)) {
			// set idx
			if (iInven >= m_iPerkNum)
				continue;
			// fx
			iter.bHover = true;
			m_iHover_Inven = iInven + 1;

			iter.vSpread.y += (_float)dTimeDelta;
			if (1.f <= iter.vSpread.y)
				iter.vSpread.y = 1.f;

			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				iter.bDown = true;
			}
			else if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L) && (!iter.bClick)) {
				m_pAudio->Play_Sound(TEXT("sfx_ui_equip_01"), -1, 0.2f, FMOD_2D);

				iter.bDown = false;
				for (_uint i = 0; i < m_iCol; ++i) {
					if (m_sSlots[i].bClick)
						continue;
					m_sSlots[i].Set_Hud(iter.iIndex, true, { 1.f, 1.f, 1.f, 1.f });
					iter.bClick = true;
					// set inven
					CInven* pInven = CInven::Get_Instance();
					pInven->Set_PerkSlot(i, iter.iIndex);
					break;
				}
			}
		}
		else {
			iter.bDown = false;
			iter.bHover = false;
			iter.vSpread.y -= (_float)dTimeDelta * 5.f;
			if (0.f >= iter.vSpread.y)
				iter.vSpread.y = 0.f;
		}
		++iInven;
	}
	// skill
	for (_uint i = 0; i < m_iCol; ++i) {
		if (__super::IsCollisionEnter(m_sSlots[i].pImage->pCollider, m_sSlots[i].pImage->pRectTransform)) {
			m_pAudio->Play_Sound_Rand(TEXT("sfx_ui_highlight"), 4, -1, 0.08f, FMOD_2D);
		}
		if (IsCollisionStay(m_sSlots[i].pImage->pCollider, m_sSlots[i].pImage->pRectTransform)) {
			m_sSlots[i].vSpread.y += (_float)dTimeDelta;
			if (1.f <= m_sSlots[i].vSpread.y)
				m_sSlots[i].vSpread.y = 1.f;
			// set idx
			if (!m_sSlots[i].bClick)
				continue;
			m_sSlots[i].bHover = true;
			if (pGame_Instance->Get_Input_MouseButtonDown(CInput::EMouseButton::L)) {
				m_sSlots[i].bDown = true;
			}
			else if (pGame_Instance->Get_Input_MouseButtonUp(CInput::EMouseButton::L)) {
				m_pAudio->Play_Sound(TEXT("sfx_ui_unequip_01"), -1, 0.2f, FMOD_2D);

				m_sSlots[i].bDown = false;
				m_sInvens[m_sSlots[i].iIndex].bClick = false;
				m_sSlots[i].Set_Hud(-1, false, { 1.f, 1.f, 1.f, 1.f });
				// set inven
				// info
				CInven* pInven = CInven::Get_Instance();
				pInven->Set_PerkSlot(i, 9999);
			}
		}
		else {
			m_sSlots[i].bDown = false;
			m_sSlots[i].bHover = false;
			m_sSlots[i].vSpread.y -= (_float)dTimeDelta * 2.f;
			if (0.f >= m_sSlots[i].vSpread.y)
				m_sSlots[i].vSpread.y = 0.f;
		}
	}

	return OBJ_NOEVENT;
}

void CUI_Lobby_Panel_Loadout::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	static _int iTemp = m_iGroup;
	const char* combo_uitype = CCamera_Lobby::ECamState_ToString(static_cast<CCamera_Lobby::ECAMSTATE>(iTemp));
	if (ImGui::BeginCombo("Group Type", combo_uitype)) {
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
	ImGui::SliderInt("group", (_int*)&m_iGroup, 0, (_uint)(CCamera_Lobby::ECAMSTATE::END)-1, "group = %d", ImGuiInputTextFlags_ReadOnly);

	ImGui::Separator();
	for (_uint i = 0; i < 4; ++i) {
		_char szLabel[_MAX_PATH] = "";
		_char szBuf[8] = "";
		_itoa_s(i, szBuf, 10);
		IM_LABELING(szLabel, "texture : ", szBuf);

		if (ImGui::TreeNode(szLabel)) {
			IM_LABELING(szLabel, "render", szBuf);
			ImGui::Checkbox(szLabel, &m_sHudFXs[i].bRedner);
			IM_LABELING(szLabel, "pass", szBuf);
			ImGui::InputInt(szLabel, (_int*)&m_sHudFXs[i].iPass);
			IM_LABELING(szLabel, "color", szBuf);
			ImGui::DragFloat4(szLabel, (_float*)&m_sHudFXs[i].vColor, 0.1f);
			ImGui::TreePop();
		}
	}

	ImGui::Separator();
	CInven* pInven = CInven::Get_Instance();
	_int4 iSlots = { (_int)pInven->Get_PerkSlot(0), (_int)pInven->Get_PerkSlot(1), (_int)pInven->Get_PerkSlot(2) , (_int)pInven->Get_PerkSlot(3) };
	//_int4 iSlots = { m_sSlots[0].iIndex, m_sSlots[1].iIndex, m_sSlots[2].iIndex , m_sSlots[3].iIndex };
	ImGui::InputInt4("test", (_int*)&iSlots);
	ImGui::Separator();

	ImGui::InputInt("inven", (_int*)&m_iHover_Inven);

	Update_Edit_Images();
}

void CUI_Lobby_Panel_Loadout::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Lobby_Panel_Loadout::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (nullptr != m_pParent)
		m_vColor.w = m_pParent->Get_Color().w;

	if (hr == __super::Render(eOrder)) {
		hr |= m_pVBRect->Render(m_pShader, m_iPass);
		// nametag
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		SImageDesc* pImage = nullptr;
		(m_iGroup == static_cast<_uint>(CCamera_Lobby::ECAMSTATE::SELECT_SLASHER)) ?
			pImage = dynamic_cast<CUI_Lobby_Panel_Slasher_Choice*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::LOBBY), L"UI_Lobby_Panel_Slasher_Choice"))->Get_NameTag() :
			pImage = dynamic_cast<CUI_Lobby_Panel_Camper_Choice*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::LOBBY), L"UI_Lobby_Panel_Camper_Choice"))->Get_NameTag();		
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImage->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, m_iPass);
	}	

//#ifdef _DEBUG
//	for (auto& iter : m_pLayouts) 
//		iter->pCollider->Render();
//#endif // _DEBUG

	_int iIdx = -1;
	_int iHover = -1;
	for (auto& iter : m_sInvens) {
		++iIdx;
		SImageDesc* pImage = iter.pImage;
		if (pImage->IsContinue())
			continue;
		_float4 vColor = iter.pImage->vColor;
		if (m_pParent->Get_IsFade())
			vColor.w = m_vColor.w;
#ifdef _DEBUG
		if (pImage->bCollider)
			pImage->pCollider->Render();
#endif // _DEBUG

		_float4x4 f4World = pImage->pRectTransform->Get_Float4x4();
		_vector vPos = XMLoadFloat3((_float3*)&f4World.m[3][0]) + XMVectorSet(5.f, 5.f, 0.f, 0.f);
		XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
		_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
		// icon
		if ((_uint)iter.iIndex < m_iPerkNum && nullptr != m_vecHud[iter.iIndex].pTexture) {	
			SHudFX* pHud = &m_vecHud[iter.iIndex];
			if (iter.bDown)
				matWorldTP *= XMMatrixTranspose(XMMatrixScaling(1.2f, 1.2f, 1.2f));
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", /*&m_sHuds[i].vColor*/&vColor, sizeof(_float4));
			hr |= pHud->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, pHud->iPass);
		}
		// fx
		for (_uint i = 0; i < 2; ++i) {
			// edge
			if (i == 0) {
				m_sHudFXs[i].bRedner = iter.bClick;
				matWorldTP *= XMMatrixTranspose(XMMatrixScaling(1.1f, 1.1f, 1.1f));
			}
			// edge
			//else if (i == 1)
			//	matWorldTP *= XMMatrixTranspose(XMMatrixScaling(1.1f, 1.1f, 1.1f));
			if (!m_sHudFXs[i].bRedner)
				continue;

			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", /*&m_sHuds[i].vColor*/&vColor, sizeof(_float4));
			if (EUIPass::PASS_SPREAD == static_cast<EUIPass>(m_sHudFXs[i].iPass))
				m_pShader->Set_RawValue("g_vSpread", &iter.vSpread, sizeof(_float2));
			hr |= m_sHudFXs[i].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, m_sHudFXs[i].iPass);
		}
		if (iter.bHover)
			iHover = iIdx /*+ 1*/;
	}
	for (auto& iter : m_sSlots) {
		SImageDesc* pImage = iter.pImage;
		if (pImage->IsContinue())
			continue;
		//if (!iter.pImage->bRender || nullptr == iter.pImage->pTexture || nullptr == iter.pImage->pRectTransform)
		//	continue;
		_float4 vColor = iter.pImage->vColor;
		if (m_pParent->Get_IsFade() /*&& iter.pImage->bActByParent*/)
			vColor.w = m_vColor.w;
#ifdef _DEBUG
		if (pImage->bCollider)
			pImage->pCollider->Render();
#endif // _DEBUG

		// set fx
		_matrix matWorldTP = pImage->pRectTransform->Get_Matrix_Transpose();
		if (iter.bClick && 0 <= iter.iIndex && nullptr != m_vecHud[iter.iIndex].pTexture) {
			SHudFX* pHud = &m_vecHud[iter.iIndex];
			if (iter.bDown)
				matWorldTP *= XMMatrixTranspose(XMMatrixScaling(1.2f, 1.2f, 1.2f));
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", /*&pHud->vColor*/&iter.vColor, sizeof(_float4));
			hr |= pHud->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, pHud->iPass);
		}
		for (_uint i = 0; i < 2; ++i) {
			// frm
			if (i == 0) {
				m_sHudFXs[i].bRedner = iter.bClick;
				matWorldTP *= XMMatrixTranspose(XMMatrixScaling(1.1f, 1.1f, 1.1f));
			}
			// edge
			//else if (i == 1)
			//	matWorldTP *= XMMatrixTranspose(XMMatrixScaling(1.1f, 1.1f, 1.1f));

			if (!m_sHudFXs[i].bRedner)
				continue;
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", /*&m_sHuds[i].vColor*/&vColor, sizeof(_float4));
			if (EUIPass::PASS_SPREAD == static_cast<EUIPass>(m_sHudFXs[i].iPass))
				m_pShader->Set_RawValue("g_vSpread", &iter.vSpread, sizeof(_float2));
			hr |= m_sHudFXs[i].pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, m_sHudFXs[i].iPass);
		}
	}

	if (0 <= iHover) {
		SImageDesc* pExpImg = m_sExplains[iHover].pImage;
		_float4x4 f4World = pExpImg->pRectTransform->Get_Float4x4();
		_vector vPos = XMLoadFloat3((_float3*)&m_sInvens[iHover].pImage->pRectTransform->Get_Float4x4().m[3][0]) + XMVectorSet(XMVectorGetX(pExpImg->pRectTransform->Get_DestSize()) * .5f, 0.f, 0.f, 0.f);
		XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
		_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &pExpImg->vColor, sizeof(_float4));
		hr |= pExpImg->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pExpImg->iPass);


		//SImageDesc* pExpImg = m_sExplains[0].pImage;
		//_float4x4 f4World = pExpImg->pRectTransform->Get_Float4x4();
		//_vector vPos = XMLoadFloat3((_float3*)&m_sInvens[iHover - 1].pImage->pRectTransform->Get_Float4x4().m[3][0]) + XMVectorSet(XMVectorGetX(pExpImg->pRectTransform->Get_DestSize()) * .5f, 0.f, 0.f, 0.f);
		//XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
		//_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));

		//hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
		//hr |= m_pShader->Set_RawValue("g_vColor", &pExpImg->vColor, sizeof(_float4));
		//hr |= pExpImg->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		//hr |= m_pVBRect->Render(m_pShader, pExpImg->iPass);


		//SImageDesc* pExpImg2 = m_sExplains[iHover].pImage;
		//f4World = pExpImg2->pRectTransform->Get_Float4x4();
		//vPos = XMLoadFloat3((_float3*)&m_sInvens[iHover - 1].pImage->pRectTransform->Get_Float4x4().m[3][0]) 
		//	+ XMVectorSet(XMVectorGetX(pExpImg2->pRectTransform->Get_DestSize()) * .6f, XMVectorGetY(pExpImg2->pRectTransform->Get_DestSize()) * .6f, 0.f, 0.f);
		////vPos -= XMVectorSet(10.f, 10.f, 0.f, 0.f);
		//XMStoreFloat3((_float3*)&f4World.m[3][0], vPos);
		//matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
		//hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
		//hr |= m_pShader->Set_RawValue("g_vColor", &pExpImg2->vColor, sizeof(_float4));
		//hr |= pExpImg2->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		//hr |= m_pVBRect->Render(m_pShader, pExpImg2->iPass);
	}

	return hr;
}

const HRESULT CUI_Lobby_Panel_Loadout::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	__super::Save_AddData(L"UI_Lobby_Panel_Loadout");

	return S_OK;
}

void CUI_Lobby_Panel_Loadout::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	//_float fRatio = (1.f / 1920.f) * g_iWinSizeX;
	//m_pRectTransform->Set_Size(m_pRectTransform->Get_Desc().fWidth * fRatio, m_pRectTransform->Get_Desc().fHeight * fRatio);
	Set_SizeOfTexture(m_pTexture, m_pRectTransform);
	m_pRectTransform->Set_Scale(1.f);

	m_iDepth = 10;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr;
	m_vecImages.reserve(100); // safe adress

	// icon & exp
	m_vecHud.reserve(m_iPerkNum);
	for (_uint i = 0; i < m_iPerkNum; ++i) {
		_tchar szKey[_MAX_PATH] = L"";
		_tchar szBuf[8] = L"";
		_itow_s(i + 1, szBuf, 10);
		(m_iGroup == static_cast<_uint>(CCamera_Lobby::ECAMSTATE::SELECT_SLASHER)) ? lstrcpy(szKey, L"loadout_perk_sla") : lstrcpy(szKey, L"loadout_perk_cam");
		lstrcat(szKey, szBuf);
		SHudFX sHud;
		sHud.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(szKey).c_str()));
		m_vecHud.emplace_back(sHud);

		// explain
		(m_iGroup == static_cast<_uint>(CCamera_Lobby::ECAMSTATE::SELECT_SLASHER)) ? lstrcpy(szKey, L"loadout_exp_sla") : lstrcpy(szKey, L"loadout_exp_cam");
		lstrcat(szKey, szBuf);
		m_sExplains[i].pImage = Add_Image(szKey, static_cast<_int>(ETexType::TEXT), false);
		//m_sExplains[i + 1].pImage = Add_Image(szKey, L"loadout_exp_back2", m_sExplains[0].pImage->pRectTransform, false, CRectTransform::EEdge::LTOP);
	}

	// layout1 : 1 + 4
	pImage = Add_Image(L"loadout_layout_skill", static_cast<_int>(ETexType::LAYOUT), true, CRectTransform::EEdge::LCOR);
	//m_pLayouts[0] = pImage;
	CRectTransform* pLayout = pImage->pRectTransform;
	pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(50.f, -25.f, 0.f));
	pImage->vColor.w = 0.f;
	// back slot
	for (_uint i = 0; i < m_iCol; ++i) {
		//pImage = Add_Image(L"loadout_icon_skill", L"loadout_layout_skill", pLayout, false, CRectTransform::EEdge::LCOR);
		pImage = Add_Image(L"loadout_icon_skill", static_cast<_uint>(ETexType::BACK), true, CRectTransform::EEdge::LCOR);
		pImage->pRectTransform->Set_Parent(pLayout, false);
		pImage->pRectTransform->Set_Scale(0.833f);
		pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3((5.f + XMVectorGetX(pImage->pRectTransform->Get_DestSize())) * i, 0.f, 0.f));
		// group info
		m_sSlots[i].pImage = pImage;
		m_sSlots[i].iIndex = i;
	}
	// layout2 : 1 + 12
	pImage = Add_Image(L"loadout_layout_inven", static_cast<_int>(ETexType::LAYOUT), true, CRectTransform::EEdge::LBOT);
	//m_pLayouts[1] = pImage;
	pLayout = pImage->pRectTransform;
	pImage->pRectTransform->Set_Parent(m_pRectTransform, false);
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(50.f, 0.f, 0.f));
	pImage->vColor.w = 0.f;
	// back inven
	for (_uint i = 0; i < m_iRow; ++i) {
		for (_uint j = 0; j < m_iCol; ++j) {
			pImage = Add_Image(L"loadout_icon_inven", static_cast<_uint>(ETexType::BACK), true, CRectTransform::EEdge::LTOP);
			pImage->pRectTransform->Set_Parent(pLayout, false);
			_float fX = (i % 2 == 0) ? 0.f : 60.f;
			pImage->pRectTransform->Set_State(CRectTransform::EState::POS, 
				_float3(fX + (/*5.f +*/ XMVectorGetX(pImage->pRectTransform->Get_DestSize())) * j, (XMVectorGetY(pImage->pRectTransform->Get_DestSize()) * 0.55f * i), 0.f));
			// group info
			_uint iIdx = i * 4 + j;
			m_sInvens[iIdx].iIndex = iIdx;
			m_sInvens[iIdx].pImage = pImage;
		}
	}
}
