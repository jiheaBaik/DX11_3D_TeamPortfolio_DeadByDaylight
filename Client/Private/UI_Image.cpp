#include "pch.h"
#include "UI_Image.h"

CUI_Image::CUI_Image(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI(pDevice, pContext, pNetwork) {
}

CUI_Image::CUI_Image(const CUI& rhs)
	: CUI(rhs) {
}

void CUI_Image::Delete(void) {
	__super::Delete();
	m_sImage.Release();
	if (!m_vecImages.empty()) {
		for_each(m_vecImages.begin(), m_vecImages.end(), [&](auto& iter) { iter.Release(); });
		m_vecImages.clear();
	}
	if (!m_vecChilds.empty()) {
		/*for_each(m_vecChilds.begin(), m_vecChilds.end(), [&](auto& iter) { 
			if (nullptr != iter) {
				iter->Set_Dead();
				Safe_Release(iter);
			}
			});*/
		m_vecChilds.clear();
	}

	Safe_Release(m_pVBRect);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pRectTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollider);
	Safe_Release(m_pAudio);

	m_bDead = true;
}

const HRESULT CUI_Image::Init_Create(void) {
	HRESULT hr = S_OK;
	hr |= __super::Init(g_iWinSizeX, g_iWinSizeY);
	return hr;
}

const HRESULT CUI_Image::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Buffer_Rect"), TEXT("Buffer_Rect"), reinterpret_cast<CComponent*&>(m_pVBRect));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene ::STATIC), TEXT("Shader_UI"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform"), TEXT("RectTransform"), reinterpret_cast<CComponent*&>(m_pRectTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));

	if (nullptr != pArg) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		memcpy(m_szTexKey, pArg, sizeof(_tchar) * _MAX_PATH);
		hr |= __super::Add_Component(pGame_Instance->Get_LoadingScene(), Get_TextureKey(m_szTexKey).c_str(), TEXT("Texture"), reinterpret_cast<CComponent*&>(m_pTexture));
		this->Set_SizeOfTexture(m_pTexture, m_pRectTransform);
	}
	m_bStart = false;
	m_bOneOff = false;
	return hr;
}

const HRESULT CUI_Image::Init_Font() {
	HRESULT hr = S_OK;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	setlocale(LC_ALL, "Korean");
	// add font
	//m_vecFontKeys.emplace_back(TEXT("Gothic24"));
	//m_vecFontKeys.emplace_back(TEXT("GothicSemilight32"));
	//_tchar szPath[_MAX_PATH] = L"../Bin/Resource/Font/";
	//_tchar szExt[_MAX_EXT] = L".spritefont";
	//for (auto& iter : m_vecFontKeys) {
	//	_tchar szFullPath[_MAX_PATH] = L"";
	//	lstrcpy(szFullPath, szPath);
	//	lstrcat(szFullPath, iter);
	//	lstrcat(szFullPath, szExt);
	//	hr |= pGame_Instance->Add_Font(iter, szFullPath);
	//}
	return hr;
}

const _ubyte CUI_Image::Update(const _double& dTimeDelta) {
	if (nullptr != m_pParent && m_bActiveByParent)
		this->Set_Active(m_pParent->Get_Active());
	if (!m_vecChilds.empty()) {
		for (auto iter = m_vecChilds.begin(); iter != m_vecChilds.end(); ++iter) {
			if (nullptr == &iter) {
				iter = m_vecChilds.erase(iter);
				break;
			}
		}
	}

#ifdef _DEBUG
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	_bool bMouseDown = pGame_Instance->Get_Input_MouseButton(CInput::EMouseButton::L);

	if (m_bCollider && bMouseDown) {
		POINT ptMouse = Get_MousePt(g_hWnd);
		m_pRectTransform->Set_MouseMove(&ptMouse);
	}
	if (!m_vecImages.empty()) {
		for (auto& iter : m_vecImages) {
			if (nullptr == iter.pRectTransform)
				continue;

			if (iter.bCollider && bMouseDown) {
				POINT ptMouse = Get_MousePt(g_hWnd);
				iter.pRectTransform->Set_MouseMove(&ptMouse);
			}
		}
	}
#endif // _DEBUG

	return OBJ_NOEVENT;
}

void CUI_Image::Update_Edit(void) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	_float3 vMouse;
	XMStoreFloat3(&vMouse, m_pRectTransform->Get_PreMouse());
	ImGui::InputFloat2("mouse", (_float*)&vMouse);
	ImGui::Separator();

	if (ImGui::Checkbox("active", &m_bActiveSelf))
		this->Set_Active(m_bActiveSelf);
	ImGui::SameLine();
	ImGui::Checkbox("actByPar", &m_bActiveByParent);
	ImGui::SameLine();
	ImGui::Checkbox("collider", &m_bCollider);
	ImGui::SameLine();
	if (ImGui::Button("szOfTex") && nullptr != m_pTexture)
		this->Set_SizeOfTexture(m_pTexture, m_pRectTransform);
	ImGui::InputInt("depth", &m_iDepth);
	m_pRectTransform->Update_Edit(0);
	ImGui::Separator();
	const char* combo_uitype = CUI_Image::EUIPass_ToString(static_cast<EUIPass>(m_iPass));
	if (ImGui::BeginCombo("pass type", combo_uitype)) {
		_uint iSize = static_cast<_uint>(EUIPass::END);
		for (_uint i = 0; i < iSize; ++i) {
			const bool is_selected = (m_iPass == i);
			if (ImGui::Selectable(CUI_Image::EUIPass_ToString(static_cast<EUIPass>(i)), is_selected))
				m_iPass = i;
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat4("color", (_float*)&m_vColor, 0.1f);
}

void CUI_Image::Update_Edit_Images() {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

#pragma region Add

	ImGui::Separator();
	if (ImGui::Button("Add Image") && lstrcmp(m_sImage.szTexture, L"")) {
		m_sImage.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), m_sImage.szTexture));
		if (nullptr == m_sImage.pTexture)
			return;
		m_sImage.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		m_sImage.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		if (m_sImage.bSizeOfTex)
			this->Set_SizeOfTexture(m_sImage.pTexture, m_sImage.pRectTransform);
		m_vecImages.emplace_back(m_sImage);
		m_sImage.Reset();
	}
	ImGui::SameLine();
	if (ImGui::TreeNode("Image Info")) {
		static _char szTexture[_MAX_PATH] = "";
		if (ImGui::InputText("Textue", szTexture, _MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue) && strcmp(szTexture, "")) {
			_tchar szTemp[_MAX_PATH] = L"";
			CONVERT_MULTI_TO_WIDE(szTexture, szTemp);
			CTexture* pProtoTex = dynamic_cast<CTexture*>(pGame_Instance->Get_Component_Prototype(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(szTemp).c_str()));
			if (nullptr != pProtoTex)
				lstrcpy(m_sImage.szTexture, this->Get_TextureKey(szTemp).c_str());
			else
				strcpy_s(szTexture, "");
		}
		//static _char szParent[_MAX_PATH] = "";
		if (ImGui::InputText("Parent", m_szBuffer, _MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue) && strcmp(m_szBuffer, "")) {
			if (!strcmp(m_szBuffer, "This"))
				lstrcpy(m_sImage.szParent, L"This");
			else {
				_tchar szTemp[_MAX_PATH] = L"";
				CONVERT_MULTI_TO_WIDE(m_szBuffer, szTemp);
				CTexture* pProtoTex = dynamic_cast<CTexture*>(pGame_Instance->Get_Component_Prototype(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(szTemp).c_str()));
				if (nullptr != pProtoTex)
					lstrcpy(m_sImage.szTexture, this->Get_TextureKey(szTemp).c_str());
				else
					strcpy_s(m_szBuffer, "");
			}		
		}
		ImGui::Checkbox("IsRender", &m_sImage.bRender);
		ImGui::SameLine();
		ImGui::Checkbox("SizeOfTex", &m_sImage.bSizeOfTex);
		ImGui::SliderInt("Depth", (_int*)&m_sImage.iDepth, 0, (_uint)(m_vecImages.size()), "depth = %d");
		ImGui::SliderInt("Pass", (_int*)&m_sImage.iPass, 0, (_uint)(CUI_Image::EUIPass::END), "pass = %d");
		ImGui::DragFloat4("Color", (_float*)&m_sImage.vColor, 0.1f);
		ImGui::TreePop();
	}
#pragma endregion

#pragma region List

	if (m_vecImages.empty())
		return;
	ImGui::Separator();

	static _bool bCollider = false;
	ImGui::Checkbox("all col", &bCollider);
	ImGui::SameLine();
	if (ImGui::Button("sort childs"))
		sort(m_vecImages.begin(), m_vecImages.end(), [&](SImageDesc sSour, SImageDesc sDesc)->_bool { return sSour.iDepth < sDesc.iDepth; });
	ImGui::SameLine();
	ImGui::DragInt("sort in layer", (_int*)&m_iSort, 1.f, -1, (_int)m_vecImages.size(), "sort = %d");

	_uint i = 0;
	for (auto& iter = m_vecImages.begin(); iter != m_vecImages.end(); ++iter) {
		//for (auto& iter : m_vecImages) {
		if (nullptr == iter->pTexture)
			continue;
		_char szLabel[_MAX_PATH] = "";
		_char szBuf[8] = "";
		_itoa_s(i, szBuf, 10);
		CONVERT_WIDE_TO_MULTI(iter->szTexture, szLabel);
		IM_LABELING(szLabel, szLabel, ": ");
		IM_LABELING(szLabel, szLabel, szBuf);

		if (ImGui::TreeNode(szLabel)) {
			IM_LABELING(szLabel, "delete", szBuf);
			if (ImGui::Button(szLabel)) {
				iter->Release();
				m_vecImages.erase(iter);
				ImGui::TreePop();
				break;
			}
			ImGui::SameLine();
			IM_LABELING(szLabel, "parent", szBuf);
			if (ImGui::Button(szLabel) && strcmp(m_szBuffer, "")) {
				_tchar szTemp[_MAX_PATH] = L"";
				CONVERT_MULTI_TO_WIDE(m_szBuffer, szTemp);
				if (!lstrcmp(szTemp, L"This") && nullptr != m_pRectTransform) {
					iter->pRectTransform->Set_Parent(m_pRectTransform, iter->pRectTransform->Get_SclByParent());
				}			
				else {
					for (auto& value : m_vecImages) {
						if (lstrcmp(this->Get_TextureKey(szTemp).c_str(), value.szTexture) || nullptr == value.pRectTransform)
							continue;
						iter->pRectTransform->Set_Parent(value.pRectTransform, iter->pRectTransform->Get_SclByParent());
						break;
					}
				}
			}

			IM_LABELING(szLabel, "render", szBuf);
			ImGui::Checkbox(szLabel, &iter->bRender);
			ImGui::SameLine();
			IM_LABELING(szLabel, "collider", szBuf);
			ImGui::Checkbox(szLabel, &iter->bCollider);
			if (bCollider)
				iter->bCollider = bCollider;
			ImGui::SameLine();
			IM_LABELING(szLabel, "sizeOfTex", szBuf);
			if (ImGui::Checkbox(szLabel, &iter->bSizeOfTex) && iter->bSizeOfTex)
				Set_SizeOfTexture(iter->pTexture, iter->pRectTransform);
			ImGui::SameLine();
			IM_LABELING(szLabel, "actByParent", szBuf);
			ImGui::Checkbox(szLabel, &iter->bActByParent);
			iter->pRectTransform->Update_Edit(0);
			IM_LABELING(szLabel, "depth", szBuf);
			ImGui::SliderInt(szLabel, (_int*)&iter->iDepth, 0, (_uint)(m_vecImages.size()), "depth = %d");
			IM_LABELING(szLabel, "pass", szBuf);
			ImGui::SliderInt(szLabel, (_int*)&iter->iPass, 0, (_uint)(CUI_Image::EUIPass::END), "pass = %d");
			IM_LABELING(szLabel, "color", szBuf);
			ImGui::DragFloat4(szLabel, (_float*)&iter->vColor, 0.1f);
			ImGui::TreePop();
		}
		ImGui::Separator();
		++i;
	}
#pragma endregion
}

void CUI_Image::Update_Edit_Font(SFontDesc* const& pText) {
	ImGui::Separator();
	if (ImGui::TreeNode("Font Info")) {
		_char szBuf[_MAX_PATH] = "";
		CONVERT_WIDE_TO_MULTI(pText->szFontKey, szBuf);
		if (ImGui::InputText("text", szBuf, ImGuiInputTextFlags_EnterReturnsTrue) && strcmp(szBuf, ""))
			CONVERT_MULTI_TO_WIDE(szBuf, pText->szFontKey);
		pText->pRectTransform->Update_Edit(0);
		ImGui::Checkbox("render", &pText->bRender);
		ImGui::SameLine();
		ImGui::Checkbox("lefttop", &pText->bLeftTop);
		ImGui::InputInt("depth", (_int*)&pText->iDepth);
		ImGui::DragFloat("scale", (_float*)&pText->fScale, 1.0f, 0.f);
		ImGui::DragFloat4("color", (_float*)&pText->vColor, 0.1f);
		ImGui::TreePop();
	}

//#pragma region Input Kor
//	CONV_UTF_8TO16 convert;
//	wsprintf(pText->szText, L"%hs", m_szBuffer); // 화면에 보이는 문자를 임시버퍼에 (8-16) 
//	strcpy_s(/*text.Text_Input*/m_szBuffer, convert.to_bytes(pText->szText).c_str()); // 임시 버퍼의 문자를 변환. 그 다음에 구이에 쓸 버퍼에 옮김
//	ImGui::InputTextMultiline("Text", m_szBuffer, 100); // 원래 있던 거
//	lstrcpy(pText->szText, convert.from_bytes(m_szBuffer).c_str()); // 구이에서 입력한 문자를 변환해서 임시 버퍼에 담움(16-8)
//	sprintf_s(m_szBuffer, "%ws", pText->szText); // 임시버퍼에서 원래 쓰던 버퍼로 바꿈
//#pragma endregion

	//if (ImGui::Button("create dds")) {
	//	// get back buffer
	//	if (FAILED(DirectX::SaveWICTextureToFile(m_pContext, pDestTex2D, TEXT("../Bin/Resource/Data/Test2.dds"))))
	//		return;
	//}
}

void CUI_Image::Late_Update(const _double& dTimeDelta) {
	m_pRectTransform->Update_Desc();
	m_pCollider->Update(m_pRectTransform->Get_Matrix());

	POINT ptMouse = Get_MousePt(g_hWnd);
	m_pRectTransform->Set_ScreenToWorld(&ptMouse);

	if (!m_vecImages.empty()) {
		for (auto& iter : m_vecImages) {
			if (nullptr == iter.pRectTransform)
				continue;
			iter.pRectTransform->Update_Desc();
			if (nullptr != iter.pCollider)
				iter.pCollider->Update(iter.pRectTransform->Get_Matrix());

			POINT ptMouse = Get_MousePt(g_hWnd);
			iter.pRectTransform->Set_ScreenToWorld(&ptMouse);
		}
	}
	m_bPreActive = m_bActiveSelf;
	m_pRenderer->Add_Object(CRenderer::EOrder::UI, this);
}

const HRESULT CUI_Image::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (nullptr == m_pTexture)
		return E_FAIL;
	hr |= __super::Set_RawValue(m_pShader);
	if (!m_bRender)
		return hr;
	hr |= m_pShader->Set_RawValue("g_WorldMatrix", &m_pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
	hr |= m_pShader->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
	hr |= m_pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
#ifdef _DEBUG
	if (m_bCollider)
		m_pCollider->Render();
#endif // _DEBUG
	return hr;
}

// 참고만
const HRESULT CUI_Image::Render_Images(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	for (auto& iter : m_vecImages) {
		if (!iter.bRender || nullptr == iter.pTexture || nullptr == iter.pRectTransform)
			continue;
		if (iter.bCollider)
			iter.pCollider->Render();
		if (iter.bActByParent)
			iter.vColor = m_vColor;

		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
	}

	return hr;
}

const HRESULT CUI_Image::Render_Font(SFontDesc* const& pFont, CRectTransform* const& pRectTransform) {
	HRESULT hr = S_OK;
	if (!pFont->bRender || nullptr == pFont->pRectTransform)
		return hr;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	_float2	vScreenPos = (nullptr == pRectTransform) ? (pFont->pRectTransform->Get_WorldToScreen(pFont->bLeftTop)) : (pRectTransform->Get_WorldToScreen());
	pGame_Instance->Render_Font(pFont->szFontKey, pFont->szText, vScreenPos, XMLoadFloat4(&pFont->vColor), pFont->fScale);
	return hr;
}

void CUI_Image::Set_Parent(CUI_Image* const& pParent, const _bool& bScaleBy) {
	if (nullptr != pParent) {
		if (nullptr == m_pParent) {
			m_pRectTransform->Set_Parent(pParent->m_pRectTransform, bScaleBy);
			pParent->Push_Child(this);
			m_pParent = pParent;
		}
		else {
			m_pParent->Pop_Child(this);
			m_pRectTransform->Set_Parent(pParent->m_pRectTransform, bScaleBy);
			pParent->Push_Child(this);
			m_pParent = pParent;
		}
	}
	else {
		if (nullptr != m_pParent) {
			m_pParent->Pop_Child(this);
			m_pParent = nullptr;
			m_pRectTransform->Set_Parent(nullptr);
		}
	}
}

void CUI_Image::Push_Child(CUI_Image* const& pChild) {
	if (m_vecChilds.empty()) {
		m_vecChilds.emplace_back(pChild);
		//Safe_AddRef(pChild);
		return;
	}
	for (auto& iter : m_vecChilds) {
		if (iter != pChild) {
			m_vecChilds.emplace_back(pChild);
			//Safe_AddRef(pChild);
			break;
		}
	}
}

void CUI_Image::Pop_Child(CUI_Image* const& pChild) {
	for (auto iter = m_vecChilds.begin(); iter != m_vecChilds.end();) {
		if ((*iter) == pChild) {
			//Safe_Release(*iter);
			iter = m_vecChilds.erase(iter);
			break;
		}
		else
			++iter;
	}
}

void CUI_Image::Set_FontCnt(SFontDesc* const& pFont, const _int& iCnt, const _bool& bAcc) {
	if (nullptr == pFont->pRectTransform)
		return;
	_tchar szBuf[8] = L"";
	_itow_s(iCnt, szBuf, 10);
	(true == bAcc) ? lstrcat(pFont->szText, szBuf) : lstrcpy(pFont->szText, szBuf);
}

void CUI_Image::Set_FontStr(SFontDesc* const& pFont, const _tchar* pText, const _bool& bAcc) {
	if (nullptr == pFont->pRectTransform)
		return;
	(true == bAcc) ? lstrcat(pFont->szText, pText) : lstrcpy(pFont->szText, pText);
}
const _bool CUI_Image::Invoke(const _double& dDeltaTime, const _double& dLimTime) {
	if (m_dAccTime >= dLimTime) {
		m_dAccTime = 0.0;
		return true;
	}
	m_dAccTime += dDeltaTime;
	return false;
}

void CUI_Image::Set_Fade(const _bool& bIsIn, const _float& fAlphaMin, const _float& fAlphaMax, const _float& fPerSec, const _bool& bReset) {
	if (bIsIn) {
		if (bReset) {
			m_vColor.w = fAlphaMin;
			m_bFade = true;
		}
		if (m_bFade) {
			if (m_vColor.w <= fAlphaMax)
				m_vColor.w += fPerSec;
			else {
				m_vColor.w = fAlphaMax;
				m_bFade = false;
			}
		}
	}
	else {
		if (bReset) {
			m_vColor.w = fAlphaMax;
			m_bFade = true;
		}
		if (m_bFade) {
			if (m_vColor.w >= fAlphaMin)
				m_vColor.w -= fPerSec;
			else {
				m_vColor.w = fAlphaMin;
				m_bFade = false;
			}
		}
	}
}

wstring CUI_Image::Get_TextureKey(wstring wstrKey) {
	wstring wstr = L"Texture_";
	(wstrKey.find(wstr) == wstring::npos) ? (wstr += wstrKey) : (wstr = wstrKey);
	return wstr;
}

void CUI_Image::Set_Active(const _bool& bActive) {
	m_bActiveSelf = bActive;
	if (0 < this->Get_ChildNums()) {
		for (auto& iter : m_vecChilds) {
			if (!iter->m_bActiveByParent)
				continue;		
			iter->Set_Active(bActive);
		}			
	}
}
const _bool CUI_Image::IsCollisionEnter(CCollider* const& pCollider, CRectTransform* const& pRectTransform) {
	if (!m_bActiveSelf)
		return false;
	_vector vRayOri = pRectTransform->Get_PreMouse();
	_vector vRayDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	return pCollider->Is_CollisionEnter(vRayOri, vRayDir);
}

const _bool CUI_Image::IsCollisionStay(CCollider* const& pCollider, CRectTransform* const& pRectTransform) {
	if (!m_bActiveSelf)
		return false;
	//POINT ptMouse = Get_MousePt(g_hWnd);
	//m_pRectTransform->Set_ScreenToWorld(&ptMouse);	
	//_vector vRayOri = XMVectorSet((_float)ptMouse.x, (_float)ptMouse.y, 0.f, 0.f);
	_vector vRayOri = pRectTransform->Get_PreMouse();
	_vector vRayDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	return pCollider->Is_Collision(vRayOri, vRayDir);
}

void CUI_Image::Set_SizeOfTexture(CTexture* const& pTexture, CRectTransform* const& pRectTransform) {
	vector<ID3D11ShaderResourceView*> SRVs = pTexture->Get_ResourceView();
	ID3D11Resource* pResource = nullptr;
	SRVs[0]->GetResource(&pResource);
	D3D11_TEXTURE2D_DESC sTexDesc;
	ID3D11Texture2D* pTexture2D = static_cast<ID3D11Texture2D*>(pResource);
	pTexture2D->GetDesc(&sTexDesc);
	pRectTransform->Set_Size((_float)sTexDesc.Width, (_float)sTexDesc.Height);
	Safe_Release(pResource);
}

CUI_Image::SImageDesc* const CUI_Image::Add_Image(const _tchar* pTexKey, const CRectTransform::EEdge& eEdge, const _bool& bSizeOfTex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc sImage;
	sImage.Reset();
	sImage.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
	//sImage.pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(m_fAtt, m_fAtt, m_fAtt));
	sImage.pRectTransform->Set_Edge(eEdge);
	sImage.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
	sImage.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(pTexKey).c_str()));
	lstrcpy(sImage.szTexture, this->Get_TextureKey(pTexKey).c_str());
	//CONVERT_WIDE_TO_MULTI(this->Get_TextureKey(pTexKey).c_str(), sImage.szTexKey);
	if (true == (sImage.bSizeOfTex = bSizeOfTex))
		this->Set_SizeOfTexture(sImage.pTexture, sImage.pRectTransform);
	sImage.bSizeOfTex = bSizeOfTex;
	m_vecImages.emplace_back(sImage);
	return &m_vecImages.back();
}

CUI_Image::SImageDesc* const CUI_Image::Add_Image(const _tchar* pTexKey, const _int& iDepth, const _bool& bRender, const CRectTransform::EEdge& eEdge, const _bool& bSizeOfTex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc sImage;
	sImage.Reset();
	sImage.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
	sImage.pRectTransform->Set_Edge(eEdge);
	sImage.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
	sImage.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(pTexKey).c_str()));
	lstrcpy(sImage.szTexture, this->Get_TextureKey(pTexKey).c_str());
	//CONVERT_WIDE_TO_MULTI(this->Get_TextureKey(pTexKey).c_str(), sImage.szTexKey);
	if (true == (sImage.bSizeOfTex = bSizeOfTex))
		this->Set_SizeOfTexture(sImage.pTexture, sImage.pRectTransform);
	sImage.bSizeOfTex = bSizeOfTex;
	sImage.iDepth = iDepth;
	sImage.bRender = bRender;
	m_vecImages.emplace_back(sImage);
	return &m_vecImages.back();
}

CUI_Image::SImageDesc* const CUI_Image::Add_Image(const _tchar* pTexKey, const _tchar* pParnetKey, CRectTransform* const& pParnetTf, const _bool& bSclByParent, const CRectTransform::EEdge& eEdge, const _bool& bSizeOfTex) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc sImage;
	sImage.Reset();
	sImage.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
	sImage.pRectTransform->Set_Edge(eEdge);
	sImage.pRectTransform->Set_Parent(pParnetTf, bSclByParent);
	if (nullptr != pParnetKey)
		lstrcpy(sImage.szParent, this->Get_TextureKey(pParnetKey).c_str());
	sImage.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
	sImage.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), this->Get_TextureKey(pTexKey).c_str()));
	lstrcpy(sImage.szTexture, this->Get_TextureKey(pTexKey).c_str());
	if (true == (sImage.bSizeOfTex = bSizeOfTex))
		this->Set_SizeOfTexture(sImage.pTexture, sImage.pRectTransform);
	m_vecImages.emplace_back(sImage);
	return &m_vecImages.back();
}

const CUI_Image::SFontDesc CUI_Image::Add_Font(const _tchar* pFontKey, const _bool& bLeftTop, const _float2& fSize, const CRectTransform::EEdge& eEdge, CRectTransform* const& pRectTransform) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SFontDesc sFont;
	sFont.Reset();
	if (nullptr != pFontKey)
		lstrcpy(sFont.szFontKey, pFontKey);
	(nullptr == pRectTransform) ? (sFont.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform"))))  : 
		(sFont.pRectTransform == pRectTransform);
	sFont.pRectTransform->Set_Edge(eEdge);
	sFont.pRectTransform->Set_Size(fSize.x, fSize.y);
	sFont.bLeftTop = bLeftTop;
	return sFont;
}

void CUI_Image::Delete_List(const _uint& iIndex) {
	if (m_vecImages.empty())
		return;
	_uint iIdx = 0;
	for (auto iter = m_vecImages.begin(); iter != m_vecImages.end(); ++iter) {
		if (iIdx++ == iIndex) {
			iter->Release();
			m_vecImages.erase(iter);
			break;
		}
	}
}

const HRESULT CUI_Image::Save_Data(Json::Value& root) {
	root["IsActive"] = m_bActiveSelf;
	root["Depth"] = m_iDepth;
	root["GroupIdx"] = m_iGroup;
	root["Pass"] = m_iPass;
	root["Color"]["x"] = m_vColor.x;
	root["Color"]["y"] = m_vColor.y;
	root["Color"]["z"] = m_vColor.z;
	root["Color"]["w"] = m_vColor.w;
	m_pRectTransform->Save_Data(root["RectTransform"]);
	return S_OK;
} 

void CUI_Image::Load_Data(Json::Value& root) {
	m_bActiveSelf = root["IsActive"].asBool();
	m_iDepth = root["Depth"].asInt();
	m_iGroup = root["GroupIdx"].asInt();
	m_iPass = root["Pass"].asInt();
	m_vColor.x = root["Color"]["x"].asFloat();
	m_vColor.y = root["Color"]["y"].asFloat();
	m_vColor.z = root["Color"]["z"].asFloat();
	m_vColor.w = root["Color"]["w"].asFloat();
	m_pRectTransform->Load_Data(root["RectTransform"]);
	// set att
	//m_fAtt = (1.f / 1920.f) * (_float)g_iWinSizeX;
}

const HRESULT CUI_Image::Save_AddData(const _tchar* pLayer) {
	if (nullptr == pLayer)
		return E_FAIL;
	if (m_vecImages.empty())
		return S_OK;
	Json::Value root;
	string str;
	for (auto& iter : m_vecImages) {
		if (!iter.bSave)
			continue;
		Json::Value value;
		_char szTemp[_MAX_PATH] = "";
		CONVERT_WIDE_TO_MULTI(iter.szTexture, szTemp);;
		value["Texture"] = (str = szTemp);
		value["IsRender"] = iter.bRender;
		value["IsCollider"] = iter.bCollider;
		value["ScaleOfTex"] = iter.bSizeOfTex;
		value["Depth"] = iter.iDepth;
		value["Pass"] = iter.iPass;
		value["Color"]["x"] = iter.vColor.x;
		value["Color"]["y"] = iter.vColor.y;
		value["Color"]["z"] = iter.vColor.z;
		value["Color"]["w"] = iter.vColor.w;
		if (nullptr != iter.pRectTransform)
			iter.pRectTransform->Save_Data(value["RectTransform"]);
		if (nullptr != iter.pRectTransform->Get_Parent()) {
			if (iter.pRectTransform->Get_Parent() != m_pRectTransform) {
				if (lstrcmp(iter.szParent, L"")) {
					CONVERT_WIDE_TO_MULTI(iter.szParent, szTemp);
					value["Parent"] = (str = szTemp);
				}
				else {
					for (auto& parent : m_vecImages) {
						if (iter.pRectTransform->Get_Parent() != parent.pRectTransform)
							continue;
						CONVERT_WIDE_TO_MULTI(parent.szTexture, szTemp);
						value["Parent"] = (str = szTemp);
					}
				}
			}
			else {
				strcpy_s(szTemp, "This");
				value["Parent"] = (str = szTemp);
			}
			value["ActByParent"] = iter.bActByParent;
		}
		root["Lists"].append(value);
	}
	_char szName[_MAX_PATH] = "";
	CONVERT_WIDE_TO_MULTI(pLayer, szName);
	root["Name"] = (str = szName);
	root["Sort Idx"] = m_iSort;

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	_tchar szScene[MAX_PATH] = TEXT("");
	switch (static_cast<EScene>(pGame_Instance->Get_LoadingScene())) {
	case EScene::LOGO: wcscpy_s(szScene, TEXT("01.Logo")); break;
	case EScene::LOBBY: wcscpy_s(szScene, TEXT("02.Lobby")); break;
	case EScene::GAMEPLAY: wcscpy_s(szScene, TEXT("03.GamePlay")); break;
	}
	_tchar szFilePath[MAX_PATH] = TEXT("");
	wsprintfW(szFilePath, TEXT("../Bin/Resource/%s/UI/"), szScene);
	_tchar szName2[_MAX_PATH] = TEXT("");
	wcscpy_s(szName2, pLayer);
	list<CObject*>* pList = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_LoadingScene(), pLayer);
	_uint iIndex = 0;
	for (auto& iter : *pList) {
		if (iter == this) {
			_tchar szBuf[8] = TEXT("");
			_itow_s(iIndex, szBuf, 10);
			wcscat_s(szName2, _MAX_PATH, TEXT("("));
			wcscat_s(szName2, _MAX_PATH, szBuf);
			wcscat_s(szName2, _MAX_PATH, TEXT(")"));
			break;
		}
		++iIndex;
	}
	wcscat_s(szName2, _MAX_PATH, TEXT(".txt"));
	wcscat_s(szFilePath, _MAX_PATH, szName2);
	ofstream ofs(szFilePath);
	if (ofs.fail())
		return E_FAIL;
	Json::StyledWriter sw;
	string str2 = sw.write(root);
	ofs << str2;
	ofs.close();

	return S_OK;

}

void CUI_Image::Load_AddData(const _tchar* pLayer, const _uint& iAccSize) {
	if (nullptr == pLayer)
		return;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	_tchar szScene[MAX_PATH] = TEXT("");
	switch (static_cast<EScene>(pGame_Instance->Get_LoadingScene())) {
	case EScene::LOGO: wcscpy_s(szScene, TEXT("01.Logo")); break;
	case EScene::LOBBY: wcscpy_s(szScene, TEXT("02.Lobby")); break;
	case EScene::GAMEPLAY: wcscpy_s(szScene, TEXT("03.GamePlay")); break;
	}
	_tchar szFilePath[MAX_PATH] = TEXT("");
	wsprintfW(szFilePath, TEXT("../Bin/Resource/%s/UI/"), szScene);
	_tchar szName2[_MAX_PATH] = TEXT("");
	wcscpy_s(szName2, pLayer);
	list<CObject*>* pList = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_LoadingScene(), pLayer);
	_uint iIndex = 0;
	for (auto& iter : *pList) {
		if (iter == this) {
			_tchar szBuf[8] = TEXT("");
			_itow_s(iIndex, szBuf, 10);
			wcscat_s(szName2, _MAX_PATH, TEXT("("));
			wcscat_s(szName2, _MAX_PATH, szBuf);
			wcscat_s(szName2, _MAX_PATH, TEXT(")"));
			break;
		}
		++iIndex;
	}
	wcscat_s(szName2, _MAX_PATH, TEXT(".txt"));
	wcscat_s(szFilePath, _MAX_PATH, szName2);
	ifstream ifs(szFilePath);
	if (ifs.fail())
		return;
	if (!m_vecImages.empty())
		for_each(m_vecImages.begin(), m_vecImages.end(), [&](auto& iter) { iter.Release(); });

	Json::Value root;
	ifs >> root;
	// root["Name"]
	m_iSort = root["Sort Idx"].asInt();
	_uint iSize = root["Lists"].size();
	(0 == iAccSize) ? m_vecImages.reserve(iSize) : m_vecImages.reserve(iSize + iAccSize);
	for (_uint i = 0; i < iSize; ++i) {
		_char szBuf[8] = "";
		_itoa_s(i, szBuf, 10);
		Json::Value value = root["Lists"][i];

		SImageDesc sImage;
		string str = value["Texture"].asString();
		wstring wstr;
		wstr.assign(str.begin(), str.end());
		sImage.pTexture = dynamic_cast<CTexture*>(pGame_Instance->CreateGet_Component_Clone(pGame_Instance->Get_LoadingScene(), Get_TextureKey(wstr).c_str()));
		wcscpy_s(sImage.szTexture, Get_TextureKey(wstr).c_str());
		sImage.pRectTransform = dynamic_cast<CRectTransform*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("RectTransform")));
		if (sImage.bSizeOfTex = value["ScaleOfTex"].asBool() && nullptr != sImage.pTexture)
			Set_SizeOfTexture(sImage.pTexture, sImage.pRectTransform);
		sImage.pRectTransform->Load_Data(value["RectTransform"]);
		if (!value["Parent"].empty()) {
			_tchar szTemp[_MAX_PATH] = L"";
			CONVERT_STR_TO_WIDE(value["Parent"].asString(), sImage.szParent);
			sImage.bActByParent = value["ActByParent"].asBool();
		}
		sImage.pCollider = dynamic_cast<CCollider*>(pGame_Instance->CreateGet_Component_Clone(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB")));
		sImage.bRender = value["IsRender"].asBool();
		sImage.bCollider = value["IsCollider"].asBool();
		sImage.iDepth = value["Depth"].asInt();
		sImage.iPass = value["Pass"].asUInt();
		sImage.vColor = { value["Color"]["x"].asFloat(), value["Color"]["y"].asFloat(), value["Color"]["z"].asFloat(), value["Color"]["w"].asFloat() };
		m_vecImages.emplace_back(sImage);
	}
	this->Sort_Images();

	// set parent
	for (auto& iter : m_vecImages) {
		if (!lstrcmp(iter.szParent, L"") || nullptr == iter.pRectTransform)
			continue;
		if (!lstrcmp(iter.szParent, L"This") && nullptr != m_pRectTransform)
			iter.pRectTransform->Set_Parent(m_pRectTransform, iter.pRectTransform->Get_SclByParent());
		else {
			for (auto& value : m_vecImages) {
				if (lstrcmp(iter.szParent, value.szTexture) || nullptr == value.pRectTransform)
					continue;
				iter.pRectTransform->Set_Parent(value.pRectTransform, iter.pRectTransform->Get_SclByParent());
				break;
			}
		}
	}
}

#ifdef _DEBUG
void CUI_Image::Delete_AddData(const _tchar* pLayer) {
	if (m_bDead || !m_vecImages.empty() || nullptr == pLayer)
		return;
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	_tchar szScene[MAX_PATH] = TEXT("");
	switch (static_cast<EScene>(pGame_Instance->Get_LoadingScene())) {
	case EScene::LOGO: wcscpy_s(szScene, TEXT("01.Logo")); break;
	case EScene::LOBBY: wcscpy_s(szScene, TEXT("02.Lobby")); break;
	case EScene::GAMEPLAY: wcscpy_s(szScene, TEXT("03.GamePlay")); break;
	}
	_tchar szFilePath[MAX_PATH] = TEXT("");
	wsprintfW(szFilePath, TEXT("../Bin/Resource/%s/UI/"), szScene);
	_tchar szName2[_MAX_PATH] = TEXT("");
	wcscpy_s(szName2, pLayer);
	list<CObject*>* pList = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_LoadingScene(), pLayer);
	_uint iIndex = 0;
	for (auto& iter : *pList) {
		if (iter == this) {
			_tchar szBuf[8] = TEXT("");
			_itow_s(iIndex, szBuf, 10);
			wcscat_s(szName2, _MAX_PATH, TEXT("("));
			wcscat_s(szName2, _MAX_PATH, szBuf);
			wcscat_s(szName2, _MAX_PATH, TEXT(")"));
			break;
		}
		++iIndex;
	}
	wcscat_s(szName2, _MAX_PATH, TEXT(".txt"));
	wcscat_s(szFilePath, _MAX_PATH, szName2);
	ifstream ifs(szFilePath);
	if (ifs.fail())
		return;
	DeleteFile(szFilePath);
}
#endif // _DEBUG


