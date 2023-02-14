#include "pch.h"
#include "UI_Play_Icon_Hud.h"
#include "Slasher_Huntress.h"
#include "UI_End_Panel_First.h"

CUI_Play_Icon_Hud* const CUI_Play_Icon_Hud::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Icon_Hud* pInstnace = new CUI_Play_Icon_Hud(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Icon_Hud::Clone(void* const& pArg) {
	CUI_Play_Icon_Hud* pInstnace = new CUI_Play_Icon_Hud(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Icon_Hud::CUI_Play_Icon_Hud(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Icon_Hud::CUI_Play_Icon_Hud(const CUI_Play_Icon_Hud& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Icon_Hud::Delete(void) {
	__super::Delete();
	for (auto& iter : m_sKeys)
		iter.Release();
	//for_each(m_vecFonts.begin(), m_vecFonts.end(), [&](auto& iter) { iter.Release(); });
}

const HRESULT CUI_Play_Icon_Hud::Init_Create(void) {
	HRESULT hr = S_OK;

	// update 순서 및 따닥 문제 해결
	// 스킬 hud 퍽쪽에 추가

	return hr;
}

const HRESULT CUI_Play_Icon_Hud::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone();
	m_iType = -1;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	m_sPoses[0].Reset(_float2(0.f, 0.5f), _float3(-230.f, -137.f, 0.f)); // left
	m_sPoses[1].Reset(_float2(0.5, 0.5f), _float3(10.f, -137.f, 0.f)); // cen
	m_sPoses[2].Reset(_float2(1.f, 0.5f), _float3(230.f, -137.f, 0.f)); // right

	m_sKeys[0].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_hud_mouseL").c_str()));
	m_sKeys[1].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_hud_space").c_str()));
	m_sKeys[2].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_hud_mouseR").c_str()));
	m_sKeys[3].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_hud_KeyE").c_str()));
	m_sKeys[4].pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(L"play_hud_KeyR").c_str()));
	for (_uint i = 0; i < static_cast<_uint>(EKey::END); ++i) {
		m_sKeys[i].pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		this->Set_SizeOfTexture(m_sKeys[i].pTexture, m_sKeys[i].pRectTransform);
		m_sKeys[i].pRectTransform->Set_Anchor(_float2(0.5f, 1.f));
		m_sKeys[i].iDepth = 3;
		(i == 1 || i == 3 || i == 4) ? m_sKeys[i].pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.75f, 0.75f, 0.75f)) :
			m_sKeys[i].pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.35f, 0.35f, 0.35f));
	}
	return hr;
}

const _ubyte CUI_Play_Icon_Hud::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
		this->Set_Parent(pParent, false);
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;

#ifdef _DEBUG

	//if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_E)) {
//	dynamic_cast<CUI_End_Panel_First*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_End_Panel_First"))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ECamper::SACRIFICE), true);
//}
//if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_R)) {
//	dynamic_cast<CUI_End_Panel_First*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_End_Panel_First"))->Set_State(static_cast<_uint>(CUI_End_Panel_First::ECamper::ESCAPE), true);
//}

	//if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_E)) {
	//	CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"Slasher_Huntress"));
	//	if (0 < pSlasher->Get_WeaponCount()) {
	//		this->Set_AllHudsOn(CUI_Play_Icon_Hud::ESlasher::PASSIVE1, CUI_Play_Icon_Hud::EKey::MOUSE_R, CUI_Play_Icon_Hud::ESlasher::LIFT, CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::ESlasher::KILL, CUI_Play_Icon_Hud::EKey::MOUSE_L);
	//	}
	//	else {
	//		this->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::LIFT), CUI_Play_Icon_Hud::EKey::SPACE, CUI_Play_Icon_Hud::EDiv::LEFT);
	//		this->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ESlasher::KILL), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::RIGHT);
	//	}
	//}
	//if (pGame_Instance->Get_Input_KeyboardKeyDown(DIK_R)) {
	//	CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"Slasher_Huntress"));
	//	this->Set_AllHuds(false, -1);
	//}

#endif // _DEBUG


	return OBJ_NOEVENT;
}

void CUI_Play_Icon_Hud::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	Update_Edit_Images();

	for (_uint i = 0; i < 3; ++i) {
		if (nullptr == m_sKeys[i].pRectTransform)
			continue;
		_char szBuf[8] = "";
		_itoa_s(i, szBuf, 10);
		if (ImGui::TreeNode(szBuf)) {
			m_sKeys[i].pRectTransform->Update_Edit(0);
			ImGui::TreePop();
		}
	}

}

void CUI_Play_Icon_Hud::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
	for (auto& iter : m_sKeys) {
		if (nullptr == iter.pRectTransform || nullptr == iter.pTexture)
			continue;
		iter.pRectTransform->Update_Desc();
	}
}

const HRESULT CUI_Play_Icon_Hud::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (m_bDelay) {
		m_bDelay = false;
		return hr;
	}
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_sHuds) {
		SImageDesc* pImg = iter.pHudImg;
		if (pImg == nullptr || !pImg->bRender || !m_sKeys[iter.iKey].bRender) 
			continue;
		// 공용 키를 특정 위치에 그린다.
		auto& value = m_sKeys[iter.iKey];
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &value.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= value.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pImg->iPass);

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &pImg->pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &pImg->vColor, sizeof(_float4));
		hr |= pImg->pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, pImg->iPass);
#ifdef _DEBUG
		if (pImg->bCollider)
			pImg->pCollider->Render();
#endif // _DEBUG				

	}

	// reset
	/*for (auto& iter : m_sHuds) {
		iter.Reset();
	}*/
		
	return hr;
}

CUI_Image::SImageDesc* const CUI_Play_Icon_Hud::Get_HudImage(const _int& iType) {
	if (iType >= (_int)(m_vecImages.size()))
		return nullptr;
	SImageDesc* pImage = nullptr;
	for (auto& iter : m_vecImages) {
		if (iter.iDepth != iType)
			continue;
		pImage = &iter;
		break;
	}
	return pImage;
}

CUI_Image::SImageDesc* const CUI_Play_Icon_Hud::Get_HudImage(const EDiv& eDiv) {
	if (EDiv::NONE >= eDiv || EDiv::END <= eDiv)
		return nullptr;
	return m_sHuds[static_cast<_uint>(eDiv)].pHudImg;
}

const _bool CUI_Play_Icon_Hud::Get_IsEmptyDiv(const EDiv& eDiv, const _int& iType) {
	if (EDiv::NONE >= eDiv || EDiv::END <= eDiv)
		return false;
	SImageDesc* pImage = m_sHuds[static_cast<_uint>(eDiv)].pHudImg;
	if (0 < iType) {
		_uint iPreType = -1;
		for (auto& iter : m_vecImages) {
			if (++iPreType == iType)
				break;
		}
		if (nullptr != pImage && pImage->iDepth == iPreType)
			return true;
	}
	return (nullptr == pImage);
}

/// <summary>
/// hud 켜기
/// </summary>
/// <param name="iType">어떤 이미지 켤건지, enum type 캐스팅</param>
/// <param name="eKey">어떤 키로 켤건지</param>
/// <param name="eDiv">어느 위치에 켤건지</param>
void CUI_Play_Icon_Hud::Set_HudOn(const _int& iType, const EKey& eKey, const EDiv& eDiv, const _bool& bDelay) {
	if (iType >= (_int)m_vecImages.size() || EDiv::NONE >= eDiv)
		return;
	m_bDelay = bDelay;
	// move pre
	_int iPreDiv = -1;
	for (_uint i = 0; i < static_cast<_uint>(EDiv::END); ++i) {
		if (nullptr == m_sHuds[i].pHudImg)
			continue;
		if (m_sHuds[i].pHudImg->iDepth == iType) {
			iPreDiv = i;
			break;
		}
	}
	EDiv eNextDiv = eDiv;
	//if (0 < iPreDiv) {
	switch (eNextDiv)
	{
	case Client::CUI_Play_Icon_Hud::EDiv::CENTER: 
		//if (nullptr == m_sHuds[static_cast<_uint>(EDiv::CENTER)].pHudImg)
		//	break;
		//if (nullptr != m_sHuds[static_cast<_uint>(EDiv::LEFT)].pHudImg)
		//	eNextDiv = EDiv::RIGHT;
		//else if (nullptr != m_sHuds[static_cast<_uint>(EDiv::RIGHT)].pHudImg) 
		//	eNextDiv = EDiv::LEFT;
		break;
	case Client::CUI_Play_Icon_Hud::EDiv::LEFT:	// double : move prev
		if (nullptr == m_sHuds[static_cast<_uint>(EDiv::CENTER)].pHudImg)
			break;
		this->Bind_Pos(EDiv::CENTER, EDiv::RIGHT); // cen to right
		break;
	case Client::CUI_Play_Icon_Hud::EDiv::RIGHT:
		if (nullptr == m_sHuds[static_cast<_uint>(EDiv::CENTER)].pHudImg)
			break;
		this->Bind_Pos(EDiv::CENTER, EDiv::LEFT); // cen to left
		break;
	}
	this->Bind_Hud(iType, static_cast<_uint>(eKey), static_cast<_uint>(eNextDiv));
	this->Bind_Pos(eNextDiv);
}

void CUI_Play_Icon_Hud::Set_HudOn(const _int& iType, const EKey& eKey, const EDiv& eSour, const EDiv& eDest, const _bool& bDelay) {
	if (iType >= (_int)m_vecImages.size() || EDiv::NONE >= eSour || EDiv::NONE >= eDest)
		return;
	m_bDelay = bDelay;
	// move pre
	_int iPreDiv = -1;
	for (_uint i = 0; i < static_cast<_uint>(EDiv::END); ++i) {
		if (nullptr == m_sHuds[i].pHudImg)
			continue;
		if (m_sHuds[i].pHudImg->iDepth == iType) {
			iPreDiv = i;
			break;
		}
	}
	EDiv eNextDiv = (true == this->Get_IsEmptyDiv(eSour, iType)) ? eSour : eDest;
	//if (0 < iPreDiv) {
	switch (eNextDiv)
	{
	case Client::CUI_Play_Icon_Hud::EDiv::CENTER:
		//if (nullptr == m_sHuds[static_cast<_uint>(EDiv::CENTER)].pHudImg)
		//	break;
		//if (nullptr != m_sHuds[static_cast<_uint>(EDiv::LEFT)].pHudImg)
		//	eNextDiv = EDiv::RIGHT;
		//else if (nullptr != m_sHuds[static_cast<_uint>(EDiv::RIGHT)].pHudImg)
		//	eNextDiv = EDiv::LEFT;
		break;								
	case Client::CUI_Play_Icon_Hud::EDiv::LEFT:	// double : move prev
		if (nullptr == m_sHuds[static_cast<_uint>(EDiv::CENTER)].pHudImg)
			break;
		this->Bind_Pos(EDiv::CENTER, EDiv::RIGHT); // cen to right
		break;
	case Client::CUI_Play_Icon_Hud::EDiv::RIGHT:
		if (nullptr == m_sHuds[static_cast<_uint>(EDiv::CENTER)].pHudImg)
			break;
		this->Bind_Pos(EDiv::CENTER, EDiv::LEFT); // cen to left
		break;
	}
	this->Bind_Hud(iType, static_cast<_uint>(eKey), static_cast<_uint>(eNextDiv));
	this->Bind_Pos(eNextDiv);
}

/// <summary>
/// hud 끄기
/// </summary>
/// <param name="iType">어떤 이미지 끌건지, enum type 캐스팅</param>
/// <param name="bUnBind">현재 위치에서 지울건지</param>
void CUI_Play_Icon_Hud::Set_HudOff(const _uint& iType, const _bool& bUnBind, const _bool& bDelay) {
	if (iType >= (_uint)(m_vecImages.size()))
		return;
	m_bDelay = bDelay;
	_int iDiv = -1;
	for (_uint i = 0; i < static_cast<_uint>(EDiv::END); ++i) {
		if (nullptr == m_sHuds[i].pHudImg)
			continue;
		if (m_sHuds[i].pHudImg->iDepth == iType/*m_vecImages[iIdx].iDepth*/) {
			iDiv = i;
			break;
		}
	}
	if (0 > iDiv)
		return;
	// off render
	m_sHuds[iDiv].pHudImg->bRender = false;
	m_sKeys[m_sHuds[iDiv].iKey].bRender = false;

	switch (static_cast<EDiv>(iDiv)) // 원상복구
	{
	case Client::CUI_Play_Icon_Hud::EDiv::LEFT:	// double : move prev
		this->Bind_Pos(EDiv::RIGHT, EDiv::CENTER);
		break;
	case Client::CUI_Play_Icon_Hud::EDiv::RIGHT:
		this->Bind_Pos(EDiv::LEFT, EDiv::CENTER);
		break;
	}
	if (bUnBind)
		m_sHuds[iDiv].Reset();
}

void CUI_Play_Icon_Hud::Set_HudOff(const EDiv& eDiv, const _bool& bUnBind, const _bool& bDelay) {
	if (nullptr == m_sHuds[static_cast<_uint>(eDiv)].pHudImg)
		return;
	m_bDelay = bDelay;
	// off render
	m_sHuds[static_cast<_uint>(eDiv)].pHudImg->bRender = false;
	m_sKeys[m_sHuds[static_cast<_uint>(eDiv)].iKey].bRender = false;

	switch (eDiv) // 원상복구
	{
	case Client::CUI_Play_Icon_Hud::EDiv::LEFT:	// double : move prev
		this->Bind_Pos(EDiv::RIGHT, EDiv::CENTER);
		break;
	case Client::CUI_Play_Icon_Hud::EDiv::RIGHT:
		this->Bind_Pos(EDiv::LEFT, EDiv::CENTER);
		break;
	}
	if (bUnBind)
		m_sHuds[static_cast<_uint>(eDiv)].Reset();
}

/// <summary>
/// 모든 hud 제어
/// </summary>
/// <param name="bRender">렌더 여부</param>
/// <param name="iNullType">비울 구역, -1는 모두 비우기</param>
void CUI_Play_Icon_Hud::Set_AllHuds(const _bool& bRender, const _int& iNullType, const _bool& bDelay) {
	m_bDelay = bDelay;
	_int iType = -1;
	for (auto& iter : m_vecImages) {
		if (iter.iDepth == iNullType) {
			iType = iNullType;
			break;
		}
	}
	_bool bErase = (0 > iNullType) ? true : false;
	_int iDiv = -1;
	for (auto& iter : m_sHuds)  {
		++iDiv;
		if (nullptr == iter.pHudImg)
			continue;
		iter.pHudImg->bRender = bRender;
		m_sKeys[iter.iKey].bRender = bRender;
		if (iType == iter.pHudImg->iDepth && iDiv != static_cast<_uint>(EDiv::CENTER)) {
			if (nullptr != m_sHuds[static_cast<_uint>(EDiv::LEFT)].pHudImg) // right
				this->Bind_Pos(EDiv::LEFT, EDiv::CENTER, false);
			else if (nullptr != m_sHuds[static_cast<_uint>(EDiv::RIGHT)].pHudImg) // left
				this->Bind_Pos(EDiv::RIGHT, EDiv::CENTER, false);
		}
		if (bErase)
			iter.Reset();
	}
	if (bErase || 0 > iDiv)
		return;
	//if (0 < iType && bRender) {
	//	this->Bind_Hud(iType, m_sHuds[iDiv].iKey, iDiv);
	//	this->Bind_Pos(static_cast<EDiv>(iDiv));
	//}	
	m_sHuds[iDiv].Reset();
}

void CUI_Play_Icon_Hud::Set_AllHuds(const _bool& bRender, const EDiv& eDiv, const _bool& bDelay) {
	m_bDelay = bDelay;
	_bool bErase = (EDiv::END == eDiv) ? true : false;
	for (auto& iter : m_sHuds) {
		if (nullptr == iter.pHudImg)
			continue;
		iter.pHudImg->bRender = bRender;
		m_sKeys[iter.iKey].bRender = bRender;
		if (eDiv != EDiv::CENTER) {
			if (nullptr != m_sHuds[static_cast<_uint>(EDiv::LEFT)].pHudImg) // right
				this->Bind_Pos(EDiv::LEFT, EDiv::CENTER, false);
			else if (nullptr != m_sHuds[static_cast<_uint>(EDiv::RIGHT)].pHudImg) // left
				this->Bind_Pos(EDiv::RIGHT, EDiv::CENTER, false);
		}
		if (bErase)
			iter.Reset();
	}
	if (bErase)
		return;
	if (EDiv::NONE < eDiv && EDiv::END > eDiv)
		m_sHuds[static_cast<_uint>(eDiv)].Reset();
}

void CUI_Play_Icon_Hud::Set_AllHudsOn(const ESlasher& eLeft, const EKey& eLKey, const ESlasher& eCenter, const EKey& eCKey, const ESlasher& eRight, const EKey& eRKey) {
	if (eLeft == ESlasher::END || eCenter == ESlasher::END || eRight == ESlasher::END)
		return;
	if ((eLeft == eCenter || eLeft == eRight) || (eLKey == eCKey || eLKey == eRKey))
		return;
	// all off
	for (auto& iter : m_sHuds)
		iter.Reset();
	// all on
	m_sHuds[static_cast<_uint>(EDiv::LEFT)].Init(&m_vecImages[static_cast<_uint>(eLeft)], static_cast<_int>(eLKey));
	m_sHuds[static_cast<_uint>(EDiv::CENTER)].Init(&m_vecImages[static_cast<_uint>(eCenter)], static_cast<_int>(eCKey));
	m_sHuds[static_cast<_uint>(EDiv::RIGHT)].Init(&m_vecImages[static_cast<_uint>(eRight)], static_cast<_int>(eRKey));
	this->Bind_Pos(EDiv::LEFT);
	this->Bind_Pos(EDiv::CENTER);
	this->Bind_Pos(EDiv::RIGHT);
}

void CUI_Play_Icon_Hud::Set_AllHudsOn(const ECamper& eLeft, const EKey& eLKey, const ECamper& eCenter, const EKey& eCKey, const ECamper& eRight, const EKey& eRKey) {
	if (eLeft == ECamper::END || eCenter == ECamper::END || eRight == ECamper::END)
		return;
	if ((eLeft == eCenter || eLeft == eRight) || (eLKey == eCKey || eLKey == eRKey))
		return;
	// all off
	for (auto& iter : m_sHuds)
		iter.Reset();
	// all on
	m_sHuds[static_cast<_uint>(EDiv::LEFT)].Init(&m_vecImages[static_cast<_uint>(eLeft)], static_cast<_int>(eLKey));
	m_sHuds[static_cast<_uint>(EDiv::CENTER)].Init(&m_vecImages[static_cast<_uint>(eCenter)], static_cast<_int>(eCKey));
	m_sHuds[static_cast<_uint>(EDiv::RIGHT)].Init(&m_vecImages[static_cast<_uint>(eRight)], static_cast<_int>(eRKey));
	this->Bind_Pos(EDiv::LEFT);
	this->Bind_Pos(EDiv::CENTER);
	this->Bind_Pos(EDiv::RIGHT);
}

void CUI_Play_Icon_Hud::Bind_Hud(const _uint& iType, const _uint& iKey, const _uint& iDiv) {
	_uint iIdx = 0;
	for (auto& iter : m_vecImages) {
		if (iter.iDepth == iType)
			break;
		else
			++iIdx;
	}
	// off pre
	if (nullptr != m_sHuds[iDiv].pHudImg && (m_vecImages[iIdx].iDepth != m_sHuds[iDiv].pHudImg->iDepth)) {
		m_sHuds[iDiv].pHudImg->bRender = false;
		m_sKeys[m_sHuds[iDiv].iKey].bRender = false;
		m_sHuds[iDiv].Reset();
	}
	// on cur
	m_sHuds[iDiv].Init(&m_vecImages[iIdx], iKey);
}

void CUI_Play_Icon_Hud::Bind_Pos(const EDiv& eSour, const EDiv& eDest, const _bool& bRender) {
	if (nullptr == m_sHuds[static_cast<_uint>(eSour)].pHudImg || EDiv::END <= eDest)
		return;
	EDiv eDiv = eSour;
	if (eDest != EDiv::NONE) { // swap
		eDiv = eDest;
		_uint iIdx = m_sHuds[static_cast<_uint>(eSour)].pHudImg->iDepth;
		m_sHuds[static_cast<_uint>(eDiv)].Init(&m_vecImages[iIdx], m_sHuds[static_cast<_uint>(eSour)].iKey);
		m_sHuds[static_cast<_uint>(eSour)].Reset();
	}
	// move dest
	SHud* pHud = &m_sHuds[static_cast<_uint>(eDiv)];
	if (0 > pHud->iKey || nullptr == m_sKeys[pHud->iKey].pRectTransform)
		return;

	pHud->pHudImg->pRectTransform->Set_Pivot(m_sPoses[static_cast<_uint>(eDiv)].vPivot);  // set hud
	m_sKeys[pHud->iKey].pRectTransform->Set_Pivot(m_sPoses[static_cast<_uint>(eDiv)].vPivot); // set key

	_float3 vDist = m_sPoses[static_cast<_uint>(eDiv)].vHudPos;
	switch (eDiv)
	{
	case Client::CUI_Play_Icon_Hud::EDiv::LEFT: {
		pHud->pHudImg->pRectTransform->Set_State(CRectTransform::EState::POS, vDist);

		vDist.x -= XMVectorGetX(m_sKeys[pHud->iKey].pRectTransform->Get_DestSize()) + 10.f;
		m_sKeys[pHud->iKey].pRectTransform->Set_State(CRectTransform::EState::POS, vDist);
	}
		break;
	case Client::CUI_Play_Icon_Hud::EDiv::CENTER: {
		vDist.x -= 10.f;
		m_sKeys[pHud->iKey].pRectTransform->Set_State(CRectTransform::EState::POS, vDist);

		vDist = m_sPoses[static_cast<_uint>(eDiv)].vHudPos;
		vDist.x += XMVectorGetX(m_sKeys[pHud->iKey].pRectTransform->Get_DestSize()) * .65f + XMVectorGetX(pHud->pHudImg->pRectTransform->Get_DestSize() * .5f);
		pHud->pHudImg->pRectTransform->Set_State(CRectTransform::EState::POS, vDist);
	}
		break;
	case Client::CUI_Play_Icon_Hud::EDiv::RIGHT: {
		m_sKeys[pHud->iKey].pRectTransform->Set_State(CRectTransform::EState::POS, vDist);

		vDist.x += XMVectorGetX(pHud->pHudImg->pRectTransform->Get_DestSize()) + 10.f;
		pHud->pHudImg->pRectTransform->Set_State(CRectTransform::EState::POS, vDist);
	}
		break;
	}

	pHud->pHudImg->bRender = bRender;
	m_sKeys[pHud->iKey].bRender = bRender;
}

const HRESULT CUI_Play_Icon_Hud::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	//Save_AddData(L"UI_Play_Icon_Hud");
	return S_OK;
}

void CUI_Play_Icon_Hud::Load_Data(Json::Value& root) {
	__super::Load_Data(root);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr;
	CInfo* pInfo = CInfo::Get_Instance();
	if (pInfo->Get_Player()) {// slasher
		m_vecImages.reserve(static_cast<_uint>(ESlasher::END));
		// common
		pImage = Add_Image(L"play_hud_com_cancle", static_cast<_uint>(ESlasher::CANCLE), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_com_parkour", static_cast<_uint>(ESlasher::PARKOUR), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_find", static_cast<_uint>(ESlasher::FIND), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_pa", static_cast<_uint>(ESlasher::DESTROY), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_son", static_cast<_uint>(ESlasher::SONSANG), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_lift", static_cast<_uint>(ESlasher::LIFT), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_hang", static_cast<_uint>(ESlasher::HANG), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_kill", static_cast<_uint>(ESlasher::KILL), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_close", static_cast<_uint>(ESlasher::CLOSE), false,  CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_sla_perk1", static_cast<_uint>(ESlasher::PERK1), false, CRectTransform::EEdge::CBOT);
		// passive
		switch (pInfo->Get_Slasher())
		{
		case CInfo::ESlasher::HUNTRESS:
			pImage = Add_Image(L"play_hud_huntress", static_cast<_uint>(ESlasher::PASSIVE1), false, CRectTransform::EEdge::CBOT);
			break;
		case CInfo::ESlasher::SPIRIT:
			pImage = Add_Image(L"play_hud_spirit", static_cast<_uint>(ESlasher::PASSIVE1), false, CRectTransform::EEdge::CBOT);
			break;
		default: return;
		}
		this->Sort_Images();
		for (auto& iter : m_vecImages) {
			iter.pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.85f, 0.85f, 0.85f));
		}
	}
	else { // camper
		m_vecImages.reserve(static_cast<_uint>(ECamper::END));
		// common
		pImage = Add_Image(L"play_hud_com_cancle", static_cast<_uint>(ECamper::CANCLE), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_com_parkour", static_cast<_uint>(ECamper::PARKOUR), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_nurmd", static_cast<_uint>(ECamper::NURMD), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_hide", static_cast<_uint>(ECamper::HIDE), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_repair", static_cast<_uint>(ECamper::REPAIR), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_resqu", static_cast<_uint>(ECamper::RESCUE), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_talsi", static_cast<_uint>(ECamper::ESCTRY), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_talha", static_cast<_uint>(ECamper::ESCAPE), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_healsel", static_cast<_uint>(ECamper::HEALSEL), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_healoth", static_cast<_uint>(ECamper::HEALOTH), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_unlock", static_cast<_uint>(ECamper::UNLOCK), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_get", static_cast<_uint>(ECamper::GET), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_totem", static_cast<_uint>(ECamper::TOTEM), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_wiggle", static_cast<_uint>(ECamper::WIGGLE), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_perk1", static_cast<_uint>(ECamper::PERK1), false, CRectTransform::EEdge::CBOT);
		pImage = Add_Image(L"play_hud_cam_perk2", static_cast<_uint>(ECamper::PERK1), false, CRectTransform::EEdge::CBOT);
		// passive
		this->Sort_Images();
		for (auto& iter : m_vecImages) {
			iter.pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.85f, 0.85f, 0.85f));
		}
	}

	// temp init 이놈의미간...........
	this->Set_HudOn(0, EKey::SPACE, EDiv::CENTER);
	this->Late_Update(0.02);
	this->Set_HudOff(0, true);
}
