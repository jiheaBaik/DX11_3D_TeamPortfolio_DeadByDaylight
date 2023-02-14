#include "pch.h"
#include "UI_Play_Gauge_Exit.h"
#include "Camera_Lobby.h"
#include "Camper_GamePlay.h"
#include "UI_Play_Gauge_Exit.h"

CUI_Play_Gauge_Exit* const CUI_Play_Gauge_Exit::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CUI_Play_Gauge_Exit* pInstnace = new CUI_Play_Gauge_Exit(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CUI_Play_Gauge_Exit::Clone(void* const& pArg) {
	CUI_Play_Gauge_Exit* pInstnace = new CUI_Play_Gauge_Exit(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CUI_Play_Gauge_Exit::CUI_Play_Gauge_Exit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CUI_Image(pDevice, pContext, pNetwork) {
}

CUI_Play_Gauge_Exit::CUI_Play_Gauge_Exit(const CUI_Play_Gauge_Exit& rhs)
	: CUI_Image(rhs) {
}

void CUI_Play_Gauge_Exit::Delete(void) {
	__super::Delete();
}

const HRESULT CUI_Play_Gauge_Exit::Init_Create(void) {
	HRESULT hr = S_OK;
	return hr;
}

const HRESULT CUI_Play_Gauge_Exit::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(L"play_gauge_exit_out");
	return hr;
}

const _ubyte CUI_Play_Gauge_Exit::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (!m_bStart) {
		CUI_Image* pParent = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"));
		this->Set_Parent(pParent, false);
		m_bActiveByParent = false;
		this->Set_Active(false);
		m_bStart = true;
	}
	__super::Update(dTimeDelta);
	if (!m_bActiveSelf)
		return OBJ_NOEVENT;
	if (!m_bPreActive)
		m_bDecrease = true;
	if (m_bDecrease) {
		m_fGauge -= (_float)dTimeDelta * 1.f;
		if (0.f >= m_fGauge) {
			m_fGauge = 0.f;

			// woojin
			if (false == m_bEndGameCreate) {
				m_bEndGameCreate = true;

				list<class CObject*>* listCamper = pGame_Instance->Get_Object_ListClone(pGame_Instance->Get_Scene(), TEXT("Camper_GamePlay"));
				if (nullptr != listCamper) {
					for (auto& iter : *listCamper) {
						CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(iter);
						if (false == pCamper->Get_End() && pCamper->Get_Control()) {
							pCamper->Get_FM_Camper()->Set_State(CFM_Camper::EState::ENDGAME);
							pCamper->Get_FM_Camper()->Update_Network("Set_State");							
						}
					}
				}
			}

			this->Set_Active(false);
		}

		//_float3 vPos = m_vecImages[(_uint)ETexType::MOVE_ANC].pRectTransform->Get_Desc().vPosition;
		//vPos.x = -XMVectorGetX(m_pRectTransform->Get_DestSize()) * ((100.f - m_fGauge) * (1.0f / 100.f));
		//m_vecImages[(_uint)ETexType::MOVE_ANC].pRectTransform->Set_State(CRectTransform::EState::POS, vPos);

		_float2 vAnc = m_vecImages[(_uint)ETexType::MOVE_ANC].pRectTransform->Get_Desc().vAnchor;
		vAnc.x = 1.f -((100.f - m_fGauge) * (1.0f / 100.f));	// 0-1
		m_vecImages[(_uint)ETexType::MOVE_ANC].pRectTransform->Set_Anchor(vAnc);
	}

	return OBJ_NOEVENT;
}

void CUI_Play_Gauge_Exit::Update_Edit(void) {
	__super::Update_Edit();
	ImGui::Separator();

	ImGui::Checkbox("dec", &m_bDecrease);
	ImGui::SameLine();
	ImGui::InputFloat("gauge", &m_fGauge, 0.f);
	Update_Edit_Images();
}	


void CUI_Play_Gauge_Exit::Late_Update(const _double& dTimeDelta) {
	__super::Late_Update(dTimeDelta);
}

const HRESULT CUI_Play_Gauge_Exit::Render(const CRenderer::EOrder& eOrder) {
	HRESULT hr = S_OK;
	if (!m_bActiveSelf)
		return hr;
	if (hr == __super::Render(eOrder))
		hr |= m_pVBRect->Render(m_pShader, m_iPass);

	for (auto& iter : m_vecImages) {
		if (iter.IsContinue())
			continue;
		hr |= iter.pTexture->Set_Resource(m_pShader, "g_DiffuseTexture", 0);
		hr |= m_pShader->Set_RawValue("g_WorldMatrix", &iter.pRectTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		hr |= m_pShader->Set_RawValue("g_vColor", &iter.vColor, sizeof(_float4));
		if (EUIPass::PASS_DECREASE == static_cast<EUIPass>(iter.iPass)) {
			_float fScroll = m_fGauge * (1.0f / 100.f);
			hr |= m_pShader->Set_RawValue("g_fScrollX", &fScroll, sizeof(_float));
		}
		hr |= m_pVBRect->Render(m_pShader, iter.iPass);
#ifdef _DEBUG
		if (iter.bCollider)
			hr |= iter.pCollider->Render();
#endif // _DEBUG

	}
	return hr;
}

const HRESULT CUI_Play_Gauge_Exit::Save_Data(Json::Value& root) {
	__super::Save_Data(root);
	Save_AddData(L"UI_Play_Gauge_Exit");
	return S_OK;
}

void CUI_Play_Gauge_Exit::Load_Data(Json::Value& root) {
	__super::Load_Data(root);

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	SImageDesc* pImage = nullptr;
	// in
	pImage = Add_Image(L"play_gauge_exit_yellow", static_cast<_uint>(ETexType::FILLED), true, CRectTransform::EEdge::CTOP);
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(4.5f, 25.f, 0.f));
	pImage->pRectTransform->Set_Size(m_pRectTransform->Get_Desc().fWidth, m_pRectTransform->Get_Desc().fHeight);
	pImage->iPass = static_cast<_uint>(EUIPass::PASS_DECREASE);
	CRectTransform* pGaugeTf = pImage->pRectTransform;
	// fix anc
	pImage = Add_Image(L"play_gauge_exit_start", static_cast<_uint>(ETexType::FIX_ANC), true, CRectTransform::EEdge::CTOP);
	pImage->pRectTransform->Set_State(CRectTransform::EState::POS, _float3(-500.f, 25.f, 0.f));
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.5f, 0.5f, 0.f));
	// move anc
	pImage = Add_Image(L"play_gauge_exit_end", static_cast<_uint>(ETexType::MOVE_ANC), true, CRectTransform::EEdge::RCOR);
	pImage->pRectTransform->Set_Pivot(_float2(0.5f, 0.5f));
	pImage->pRectTransform->Set_Parent(pGaugeTf, false);
	pImage->pRectTransform->Set_State(CRectTransform::EState::SCL, _float3(0.6f, 0.6f, 0.6f));
}

