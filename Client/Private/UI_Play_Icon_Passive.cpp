#include "pch.h"
#include "UI_Play_Icon_Passive.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "UI_Play_Gauge_Inter.h"

CUI_Play_Icon_Passive* const CUI_Play_Icon_Passive::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Icon_Passive* pInstnace = new CUI_Play_Icon_Passive(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Icon_Passive::Clone(void* const& pArg) {
	CUI_Play_Icon_Passive* pInstnace = new CUI_Play_Icon_Passive(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Icon_Passive::CUI_Play_Icon_Passive(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Icon_Passive::CUI_Play_Icon_Passive(const CUI_Play_Icon_Passive& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Icon_Passive::Delete(void) {
	__super::Delete();
#ifdef _DEBUG
	this->Delete_AddData(L"UI_Play_Icon_Passive");
#endif // _DEBUG

	m_sFont.Release();

	for_each(m_vecFXTexs.begin(), m_vecFXTexs.end(), [&](auto& iter) { Safe_Release(iter); });
	m_vecFXTexs.clear();

	m_listIcons.clear();
	for (auto& iter : m_sSlots)
		Safe_Release(iter.pTexture);
}

const HRESULT CUI_Play_Icon_Passive::Init_Create(void) {
	HRESULT hr = S_OK;
	// µé±â¶û ½º¿ÒÇÏ±â
	// ½ºÇÇ¸´ ³×¸ð ÄðÅ¸ÀÓ
	return hr;
}

const HRESULT CUI_Play_Icon_Passive::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(/*TEXT("play_icon_skill_back")*/);
	m_iType = 0;
	return hr;
}

const _ubyte CUI_Play_Icon_Passive::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		// set parent
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
		this->Set_Parent(pParent, false);
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

	// set slot
	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player()) { // lift
		CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"Slasher_Huntress"));
		if ((pSlasher->Get_State() == CFM_Slasher::EState::PICKUP || pSlasher->Get_State() == CFM_Slasher::EState::LOCKER_GRAB ||
			pSlasher->Get_State() == CFM_Slasher::EState::GRAB_GENE_IN || pSlasher->Get_State() == CFM_Slasher::EState::GRAB_PARKOUR_BK_IN ||
			pSlasher->Get_State() == CFM_Slasher::EState::GRAB_PARKOUR_FT_IN) && !m_bOneOff) {
			Push_Slot(1);
			m_bOneOff = true;
		}
		if ((pSlasher->Get_State() == CFM_Slasher::EState::DROP || pSlasher->Get_State() == CFM_Slasher::EState::STUN_DROP_IN || 
			pSlasher->Get_State() == CFM_Slasher::EState::STUN_FLASH_IN || pSlasher->Get_State() == CFM_Slasher::EState::HOOK) && m_bOneOff) {
			Pop_Slot();
			m_bOneOff = false;
		}
	}

	// push slot
	for (auto& iter : m_listIcons) {
		switch (iter.iPush)
		{
		case 0: continue;
		case 1: { // right
			if (0 > (iter.iIdx - 1))
				continue;
			_vector vSour = (XMVector3Equal(iter.vDist, XMVectorZero())) ?
				XMLoadFloat3((_float3*)&m_sSlots[iter.iIdx - 1].pImage->pRectTransform->Get_Row(CTransform::ERow::POS)) : iter.vDist;
			_vector vDest = XMLoadFloat3((_float3*)&m_sSlots[iter.iIdx].pImage->pRectTransform->Get_Row(CTransform::ERow::POS));

			iter.vDist = XMVectorLerp(vSour, vDest, static_cast<_float>(dTimeDelta) * 10.f);
			if (1.f > XMVectorGetX(XMVector3Length(vDest - vSour))) {
				iter.Reset(0.8f);
			}
		}
			  break;
		case 2: { // left
			if (static_cast<_uint>(m_listIcons.size()) > (iter.iIdx + 1))
				continue;
			_vector vSour = (XMVector3Equal(iter.vDist, XMVectorZero())) ?
				XMLoadFloat3((_float3*)&m_sSlots[iter.iIdx + 1].pImage->pRectTransform->Get_Row(CTransform::ERow::POS)) : iter.vDist;
			_vector vDest = XMLoadFloat3((_float3*)&m_sSlots[iter.iIdx].pImage->pRectTransform->Get_Row(CTransform::ERow::POS));

			iter.vDist = XMVectorLerp(vSour, vDest, static_cast<_float>(dTimeDelta) * 10.f);
			if (1.f > XMVectorGetX(XMVector3Length(vDest - vSour))) {
				iter.Reset(1.f);
			}
		}
			  break;
		}
	}

	return OBJ_NOEVENT;
}

void CUI_Play_Icon_Passive::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();


	ImGui::InputInt("num", &m_iType);
	if (ImGui::Button("push"))
		this->Push_Slot(m_iType);
	ImGui::SameLine();
	if (ImGui::Button("pop"))
		this->Pop_Slot();

	ImGui::Separator();
	for (auto& iter : m_listIcons) {
		ImGui::InputInt("idx", (_int*)&iter.iIdx);

		_float3 vDist;
		XMStoreFloat3(&vDist, iter.vDist);
		ImGui::InputFloat3("dist", (_float*)&vDist);
	}

	Update_Edit_Font(&m_sFont);
	Update_Edit_Images();
}

void CUI_Play_Icon_Passive::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
	if (nullptr != m_sFont.pRectTransform)
		m_sFont.pRectTransform->Update_Desc();
}

const HRESULT CUI_Play_Icon_Passive::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	for (auto& iter : m_listIcons) {
		if (nullptr == iter.pTexture)
			continue;
		SImageDesc* pBackImg = m_sSlots[iter.iIdx].pImage;
		_bool bPassive = (iter.pTexture == m_sSlots[0].pTexture);

		_float4x4 f4World = pBackImg->pRectTransform->Get_Float4x4();
		_matrix matWorldTP = XMMatrixIdentity();
		if (0 < iter.iPush) {
			XMStoreFloat3((_float3*)&f4World.m[3][0], iter.vDist);
		}
		matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
		matWorldTP *= XMMatrixTranspose(XMMatrixScaling(iter.fScale, iter.fScale, iter.fScale));

		_float4 vColor = pBackImg->vColor;
		vColor.w = (iter.iIdx == 0) ? 1.f : 0.5f;		

		// fx (spirit)
		CInfo* pInfo = CInfo::Get_Instance();
		if (bPassive && pInfo->Get_Player() && CInfo::ESlasher::SPIRIT == pInfo->Get_Slasher()) {
			CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"Slasher_Huntress"));

			// ratio = fA * (1.0f / fB);
			_float fCoolTime = static_cast<_float>(pSlasher->Get_SpiritTime());
			_float fAngle = (fCoolTime * (1.0f / 100.f)) * XM_2PI;
			hr |= m_pShader->Set_RawValue("g_fAngle", &fAngle, sizeof(_float));

			_matrix matrix = matWorldTP * XMMatrixTranspose(XMMatrixScaling(1.1f, 1.1f, 1.1f));
			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matrix, sizeof(_float4x4));
			hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
			hr |= m_vecFXTexs[1]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
			hr |= m_pVBRect->Render(m_pShader, static_cast<_uint>(EUIPass::PASS_COOLDOWN));
		}

		// back
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
		hr |= pBackImg->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pBackImg->iPass);
#ifdef _DEBUG
		if (pBackImg->bCollider)
			hr |= pBackImg->pCollider->Render();
#endif // _DEBUG

		// fx(back)
		matWorldTP *= XMMatrixTranspose(XMMatrixScaling(0.8f, 0.8f, 0.8f));
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
		_float4 vColor2 = { 1.f, 0.9f, 0.7f, vColor.w };
		hr |= m_pShader->Set_RawValue("g_vColor", &vColor2, sizeof(_float4));
		hr |= m_vecFXTexs[0]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pBackImg->iPass);

		// icon
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pBackImg->iPass);

		// font (huntress)
		if (bPassive && nullptr != m_sFont.pRectTransform) {
			// world to screen
			XMStoreFloat4x4(&f4World, XMMatrixTranspose(matWorldTP));
			_float2 vScreenPos;
			XMStoreFloat2(&vScreenPos, XMLoadFloat2((_float2*)&f4World.m[3][0]));
			vScreenPos.x += (m_sFont.pRectTransform->Get_Desc().fWidth * .6f) + static_cast<_float>(g_iWinSizeX) * .5f;
			vScreenPos.y = -vScreenPos.y - (m_sFont.pRectTransform->Get_Desc().fHeight * 1.5f) + (static_cast<_float>(g_iWinSizeY) * .5f);
			 
			CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
			pGame_Instance->Render_Font(m_sFont.szFontKey, m_sFont.szText, vScreenPos, XMLoadFloat4(&m_sFont.vColor), m_sFont.fScale);
			//Render_Font(&m_sFont, false);
		}			
	}

//	_int iIdx = -1;
//	for (auto& iter : m_sSlots) {
//		SHudFX* pIcon = &m_vecIcons[++iIdx];
//		if (iter.pImage->IsContinue() || 0 > pIcon->iIdx)
//			continue;
//
//		(0 == pIcon->iIdx) ? iter.pImage->vColor.w = 1.f : iter.pImage->vColor.w = 0.5f;
//		_float4x4 f4World = iter.pImage->pRectTransform->Get_Float4x4();
//		_matrix matWorldTP = XMMatrixIdentity();
//		if (pIcon->bPush) {
//			XMStoreFloat3((_float3*)&f4World.m[3][0], pIcon->vDist);
//		}
//		else {
//			// set pos : after lerp
//		}
//		matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
//		// back
//		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
//		hr |= m_pShader->Set_RawValue("g_vColor", &iter.pImage->vColor, sizeof(_float4));
//		hr |= iter.pImage->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
//		hr |= m_pVBRect->Render(m_pShader, iter.pImage->iPass);
//#ifdef _DEBUG
//		if (iter.pImage->bCollider)
//			hr |= iter.pImage->pCollider->Render();
//#endif // _DEBUG
//
//		// fx
//		//_float4x4 f4World = iter.pImage->pRectTransform->Get_Float4x4();
//		//_matrix matWorldTP = XMMatrixTranspose(XMLoadFloat4x4(&f4World));
//		matWorldTP *= XMMatrixTranspose(XMMatrixScaling(0.8f, 0.8f, 0.8f));
//		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
//		_float4 vColor = { 1.f, 0.9f, 0.7f, iter.pImage->vColor.w };
//		hr |= m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
//		hr |= m_pFXTextures[0]->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
//		hr |= m_pVBRect->Render(m_pShader, iter.pImage->iPass);
//
//		// icon
//		if (nullptr != pIcon->pTexture) {
//			hr |= m_pShader->Set_RawValue("g_WorldMatrix", &matWorldTP, sizeof(_float4x4));
//			hr |= pIcon->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
//			hr |= m_pVBRect->Render(m_pShader, iter.pImage->iPass);
//		}
//
//		// font
//		if (nullptr != m_sFont.pRectTransform)
//			Render_Font(&m_sFont, false);
//	}

	return hr;
}

void CUI_Play_Icon_Passive::Push_Slot(const _int& iSlot) {
	if (static_cast<_uint>(m_listIcons.size()) > m_iSlotNum)
		return;
	// 1.push
	if (0 < static_cast<_uint>(m_listIcons.size())) {
		for (auto& iter : m_listIcons) {
			++iter.iIdx;
			iter.iPush = 1;
		}
	}
	// 2.input
	SIcon sIcon;
	sIcon.pTexture = m_sSlots[iSlot].pTexture;
	sIcon.iPush = 0;
	m_listIcons.emplace_back(sIcon);
}

void CUI_Play_Icon_Passive::Pop_Slot() {
	if (m_listIcons.empty())
		return;
	// 1.output
	m_listIcons.pop_back();
	// 2.push
	if (0 < static_cast<_uint>(m_listIcons.size())) {
		for (auto& iter : m_listIcons) {
			--iter.iIdx;
			iter.iPush = 2;
		}
	}
}

void CUI_Play_Icon_Passive::Set_FontCnt(const _int& iCnt, const _bool& bAcc) {
	__super::Set_FontCnt(&m_sFont, iCnt, bAcc);
}

void CUI_Play_Icon_Passive::Set_FontStr(const _tchar* pText, const _bool& bAcc) {
	__super::Set_FontStr(&m_sFont, pText, bAcc);
}

const HRESULT CUI_Play_Icon_Passive::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	Save_AddData(L"UI_Play_Icon_Passive");
	return S_OK;
}

void CUI_Play_Icon_Passive::Load_Data(Json::Value& root) {
	__super::Load_Data(root);
	Load_AddData(L"UI_Play_Icon_Passive");
	for (_uint i = 0; i < m_iSlotNum; ++i) {
		m_sSlots[i].pImage = &m_vecImages[i];
	}

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	/// n ¹øÂ° Ä­ ´ë±â
	//m_vecImages.reserve(10);
	//SImageDesc* pImage = nullptr;
	//m_sSlots[0].pImage = Add_Image(L"play_icon_skill_back", 0, true, CRectTransform::EEdge::LBOT);
	//m_sSlots[0].pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(100.f, -80.f, 0.f ));
	//m_sSlots[0].pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.3f, 0.3f, 0.3f));
	//m_sSlots[1].pImage = Add_Image(L"play_icon_skill_back", 1, true, CRectTransform::EEdge::LBOT);
	//m_sSlots[1].pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(100.f, -80.f, 0.f));
	//m_sSlots[1].pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.3f, 0.3f, 0.3f));

	// icon
	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player()) { // slasher
		switch (pInfo->Get_Slasher())
		{
		case CInfo::ESlasher::HUNTRESS: {
			// slot
			m_sSlots[0].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_skill_huntless").c_str()));
			// fx
			m_vecFXTexs.reserve(1);
			CTexture* pFXTextrue = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), this->Get_TextureKey(L"play_icon_skill_in").c_str()));
			m_vecFXTexs.emplace_back(pFXTextrue);
			// font
			m_sFont = Add_Font(L"Gothic20", true, _float2(36.f, 36.f), CRectTransform::EEdge::RTOP);
			m_sFont.pRectTransform->Set_Parent(m_pRectTransform);
			m_sFont.pRectTransform->Set_State(CRectTransform::EState::POS, _float3(30.f, -30.f, 0.f));
			m_sFont.pRectTransform->Update_Desc();
		}
			break;
		case CInfo::ESlasher::SPIRIT: {
			m_sSlots[0].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_skill_spirit").c_str()));
		
			m_vecFXTexs.reserve(2);
			m_vecFXTexs.emplace_back(dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), this->Get_TextureKey(L"play_icon_skill_in").c_str())));
			m_vecFXTexs.emplace_back(dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), this->Get_TextureKey(L"play_icon_skill_fx").c_str())));
		}
									  break;
		default: return;
		}
		m_sSlots[1].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_skill_carry").c_str()));
		this->Push_Slot(0);
	}
	else { // camper	
		m_sSlots[0].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_skill_medkit").c_str()));
		m_sSlots[1].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_skill_flashlight").c_str()));

		m_vecFXTexs.reserve(1);
		m_vecFXTexs.emplace_back(dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), this->Get_TextureKey(L"play_icon_skill_in").c_str())));
	}

}
