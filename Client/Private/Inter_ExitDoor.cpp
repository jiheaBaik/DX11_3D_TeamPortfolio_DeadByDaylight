#include "pch.h"
#include "Inter_ExitDoor.h"
#include "Slasher_Huntress.h"
#include "Camper_GamePlay.h"
#include "Shadow.h"
#include "Inter_Generator.h"
#include "UI_Play_Panel_Manager.h"
#include "Setting_BGM.h"

CInter_ExitDoor* const CInter_ExitDoor::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CInter_ExitDoor* pInstnace = new CInter_ExitDoor(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CInter_ExitDoor::Clone(void* const& pArg) {
	CInter_ExitDoor* pInstnace = new CInter_ExitDoor(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CInter_ExitDoor::CInter_ExitDoor(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CInter_ExitDoor::CInter_ExitDoor(const CInter_ExitDoor& rhs)
	: CObject(rhs) {
}

void CInter_ExitDoor::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);

	Safe_Release(m_pColTransform);
	Safe_Release(m_pCollider2);

	Safe_Release(m_pCollider_Door);

	Safe_Release(m_pAudio);
}

const HRESULT CInter_ExitDoor::Init_Create(void) {
	return S_OK;
}

const HRESULT CInter_ExitDoor::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_ModelAnim"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Model_Inter_ExitDoor"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("ColTransform"), reinterpret_cast<CComponent*&>(m_pColTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider2"), reinterpret_cast<CComponent*&>(m_pCollider2));

	m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(228.656f, 0.f, 173.503f, 1.f));
	m_pCollider->Set_Center(_float3{ -4.f, 0.f, 3.5f });

	m_pColTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(-4.3f, 0.f, 3.5f, 1.f));

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider_Door"), reinterpret_cast<CComponent*&>(m_pCollider_Door));
	m_pCollider_Door->Set_Center(_float3{ -3.61f, 1.94f, -0.29f });
	m_pCollider_Door->Set_Extents(_float3{ 0.26f, 1.94f, 2.435f });

	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Audio"), TEXT("Audio"), reinterpret_cast<CComponent*&>(m_pAudio));
	m_pTarget_Depth = CTarget_Mgr::Get_Instance()->Get_Target(TEXT("Depth"));

	m_eState = ESTATE::LOCK;
	Set_State(ESTATE::LOCK, false);
	return hr;
}

const _ubyte CInter_ExitDoor::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	Update_Network();

	if (m_eState == ESTATE::GEAR) {
		if (m_fGauge >= 100.f)
			Set_State(ESTATE::OPENING, true);
	}

	switch (m_eState) {
	case ESTATE::LOCK: {
		_uint iGenOn = 0;
		list<CObject*>* listGen = pGame_Instance->Get_Object_ListClone(static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Inter_Generator"));
		for (auto& iter : *listGen) {
			CInter_Generator* pGen = dynamic_cast<CInter_Generator*>(iter);
			if (CInter_Generator::EState::ON == pGen->Get_State())
				++iGenOn;
		}
		// set ui
		dynamic_cast<CUI_Play_Panel_Second*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Panel_Second"))->Set_FontCnt(((_uint)listGen->size() - 3) - iGenOn, 0, false);
		if (iGenOn == 5) {
			dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::EXIT, false, false);
			Set_State(ESTATE::CLOSED, true);
		}
		break;
	}
	case ESTATE::OPENING: {
		break;
	}
	case ESTATE::KILLER_OPEN: {
		break;
	}
	}

	m_pModel->Update_Anim(dTimeDelta, m_bLoop);
	m_pCollider->Update(m_pTransform->Get_Matrix());
	m_pCollider_Door->Update(m_pTransform->Get_Matrix());

	//collsion
	CInfo* pInfo = CInfo::Get_Instance();
	if (false == pInfo->Get_Player()) {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CCamper_GamePlay* pCamper = dynamic_cast<CCamper_GamePlay*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::GAMEPLAY), TEXT("Camper_GamePlay")));
		CCollider* pCamperCollider = dynamic_cast<CCollider*>(pCamper->Get_Component(TEXT("Collider")));


		if (pCamper->Get_Control()) {
			if (m_pCollider->Is_Collision(pCamperCollider)) {
				pCamper->Set_ColObject(this);
				pCamper->Set_CollisionState(CCamper_GamePlay::ECollisionState::EXITDOOR);

				CUI_Play_Icon_Hud* pHud = dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"));
				CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
				//������
				if (m_eState != ESTATE::LOCK && m_eState != ESTATE::OPENING) {
					//�ⱸ ���� UI  
					pHud->Set_HudOn(static_cast<_uint>(CUI_Play_Icon_Hud::ECamper::ESCAPE), CUI_Play_Icon_Hud::EKey::MOUSE_L, CUI_Play_Icon_Hud::EDiv::CENTER);
					pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::EXIT, true, true);
				}
				else {
					//�ⱸ ���� UI ����
					pHud->Set_HudOff(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::ESCAPE), true);
					pGauge->Set_Render(CUI_Play_Gauge_Inter::EInter::EXIT, false, false);
				}
				pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::EXIT, m_fGauge);

			}
			else {
				if (pCamper->Get_ColObject() == this) {
					pCamper->Set_ColObject(nullptr);
					pCamper->Set_CollisionState(CCamper_GamePlay::ECollisionState::NONE);
					if (pCamper->m_pFM->Get_State() == pCamper->m_pFM->EXITDOOR_IN ||
						pCamper->m_pFM->Get_State() == pCamper->m_pFM->EXITDOOR_LOOP) {
						CUI_Play_Gauge_Inter* pGauge = dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"));
						pGauge->Set_Gauge(CUI_Play_Gauge_Inter::EInter::EXIT, m_fGauge);

					}
					else {
						dynamic_cast<CUI_Play_Icon_Hud*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Icon_Hud"))->Set_HudOff(static_cast<_int>(CUI_Play_Icon_Hud::ECamper::ESCAPE), false);
						dynamic_cast<CUI_Play_Gauge_Inter*>(pGame_Instance->Get_Object_Clone(static_cast<_ubyte>(EScene::GAMEPLAY), L"UI_Play_Gauge_Inter"))->Set_Render(CUI_Play_Gauge_Inter::EInter::EXIT, false, false);
					}
				}
			}
		}
	}

	return OBJ_NOEVENT;
}

void CInter_ExitDoor::Update_Edit(void) {
	ImGui::InputText("Model", m_szModel, IM_ARRAYSIZE(m_szModel));
	m_pTransform->Update_Edit(0);
	m_pCollider->Update_Edit(1);
	ImGui::InputInt("ID", &m_iId);
	m_pCollider_Door->Update_Edit(2);
}

void CInter_ExitDoor::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CInfo* pInfo = CInfo::Get_Instance();
	CInven* pInven = CInven::Get_Instance();
	m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::SHADOW, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
	if (ESTATE::CLOSED == m_eState) {
		if (m_fSuccesTime > 0.f) {
			m_fSuccesTime -= static_cast<_float>(dTimeDelta);
			m_pRenderer->Add_Object(CRenderer::EOrder::PENETRATE, this);
		}
	}
}

const HRESULT CInter_ExitDoor::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			if (FAILED(m_pModel->Set_Resource(i, aiTextureType_NORMALS, m_pShader, "g_NormalTexture")))
				m_pModel->Render(i, m_pShader, 0);
			else
				m_pModel->Render(i, m_pShader, 7);
		}
		break;
	}
	case CRenderer::EOrder::PENETRATE: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);
		CInfo* pInfo = CInfo::Get_Instance();
		ID3D11ShaderResourceView* pDepthSRV = m_pTarget_Depth->Get_ShaderResourceView();
		m_pShader->Set_Resource("g_DepthTexture", pDepthSRV);
		_float2 fWinSize = _float2{ static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY) };
		m_pShader->Set_RawValue("g_fWinSize", &fWinSize, sizeof(_float2));
		_float4 vPenetrateColor;
		if (pInfo->Get_Player() == true)
			vPenetrateColor = _float4{ 1.f, 1.f, 1.f, 1.f };
		else
			vPenetrateColor = _float4{ 1.f, 1.f, 1.f, 1.f };
		m_pShader->Set_RawValue("g_vPenetrateColor", &vPenetrateColor, sizeof(_float4));
		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
		m_pModel->Set_Resource(0, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
		m_pModel->Render(0, m_pShader, 3);
		break;
	}
	case CRenderer::EOrder::SHADOW: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CShadow* pShadow = dynamic_cast<CShadow*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), TEXT("Shadow")));
		pShadow->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			m_pModel->Render(i, m_pShader, 1);
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		//m_pCollider->Render(pCamera);
		//m_pCollider2->Render(pCamera);
		//m_pCollider_Door->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

void CInter_ExitDoor::Set_State(const ESTATE& eState, const _bool& bNetwork) {
	switch (eState) {
	case ESTATE::LOCK:
		//m_pModel->Set_Anim("SK_Frm_ExitDoors01_REF.ao|ExitDoors01_Closed");
		m_iAnimNum = 0;
		m_pModel->Set_Anim(m_iAnimNum);
		m_eState = ESTATE::LOCK;
		m_bLoop = true;
		break;
	case ESTATE::CLOSED: {
		//m_pModel->Set_Anim("SK_Frm_ExitDoors01_REF.ao|ExitDoors01_Closed");
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_LoadingScene(), L"UI_Play_Gauge_Exit"))->Set_Active(true);
		CSetting_BGM* pBGM = dynamic_cast<CSetting_BGM*>(pGame_Instance->Get_Object_Clone(static_cast<_uint>(EScene::STATIC), TEXT("Setting_BGM")));
		pBGM->Play_BGM(TEXT("mu_killer_exit-2"), 0.05f, 2.f);
		//m_pAudio->Play_Sound(TEXT("mu_killer_exit"), 3, -1, 0.1f, FMOD_2D | FMOD_LOOP_NORMAL); asdasd
		if (m_bFirst) {
			CInfo* pInfo = CInfo::Get_Instance();
			CObject* pSmoke = pGame_Instance->CreateGet_Object_Clone(TEXT("Effect_GeneratorSuccesSmokeUI"), static_cast<_ubyte>(EScene::GAMEPLAY), TEXT("Effect_GeneratorSuccesSmokeUI"));
			CTransform* pSmokeTransform = dynamic_cast<CTransform*>(pSmoke->Get_Component(TEXT("Transform")));
			pSmokeTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(224.695f, 1.358f, 173.360f, 0.f));
			pSmokeTransform->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 0.5f));
			m_fSuccesTime = 5.f;
			m_bFirst = false;
		}

		m_iAnimNum = 0;
		m_pModel->Set_Anim(m_iAnimNum);
		m_eState = ESTATE::CLOSED;
		m_bLoop = true;
		break;
	}
	case ESTATE::GEAR:
		//m_pModel->Set_Anim("SK_Frm_ExitDoors01_REF.ao|ExitDoors01_SwitchActivation");
		m_iAnimNum = 4;
		m_pModel->Set_Anim(m_iAnimNum);
		m_eState = ESTATE::GEAR;
		m_bLoop = false;
		break;
	case ESTATE::OPENING:
		//m_pModel->Set_Anim("SK_Frm_ExitDoors01_REF.ao|ExitDoors01_Opening");
		m_iAnimNum = 3;
		m_pModel->Set_Anim(m_iAnimNum);
		m_eState = ESTATE::OPENING;
		m_bLoop = false;
		break;
	case ESTATE::KILLER_OPEN:
		//m_pModel->Set_Anim("SK_Frm_ExitDoors01_REF.ao|ExitDoors01_Killer_Open");
		m_iAnimNum = 1;
		m_pModel->Set_Anim(m_iAnimNum);
		m_eState = ESTATE::KILLER_OPEN;
		m_bLoop = false;
		break;
	}
	if (true == bNetwork)
		Update_Network("Set_State");
}

const _float& CInter_ExitDoor::Get_Gauge(void) {
	return m_fGauge;
}

const void CInter_ExitDoor::Add_Gauge(const _float& fGauge) {
	m_fAddGauge = fGauge;
	m_fGauge += m_fAddGauge;
	Update_Network("Add_Gauge");
}

void CInter_ExitDoor::Update_Network(const string& pKey) {
	Json::Value root;
	if ("Set_State" == pKey) {
		root["State"] = static_cast<_int>(m_eState);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
	if ("Add_Gauge" == pKey) {
		root["Gauge"] = m_fAddGauge;
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
}

void CInter_ExitDoor::Update_Network(void) {
	list<Json::Value>& root = m_pNetwork->Get_Recv(m_iId);
	for (auto iter = root.begin(); iter != root.end();) {
		string key = (*iter)["Key"].asString();
		if ("Set_State" == key) {
			Set_State(static_cast<ESTATE>((*iter)["State"].asInt()), false);
			iter = root.erase(iter);
		}
		else if ("Add_Gauge") {
			m_fAddGauge = (*iter)["Gauge"].asFloat();
			m_fGauge += m_fAddGauge;
			iter = root.erase(iter);
		}
		else {
			++iter;
		}
	}
}

const HRESULT CInter_ExitDoor::Save_Data(Json::Value& root) {
	m_pTransform->Save_Data(root["Transform"]);
	root["Id"] = m_iId;
	return S_OK;
}

void CInter_ExitDoor::Load_Data(Json::Value& root) {
	m_pTransform->Load_Data(root["Transform"]);
	m_iId = root["Id"].asInt();
}
