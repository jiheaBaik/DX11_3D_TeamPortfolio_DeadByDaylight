#include "pch.h"
#include "Inter_HookHuntress.h"
#include "Slasher_Huntress.h"

CInter_HookHuntress* const CInter_HookHuntress::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CInter_HookHuntress* pInstnace = new CInter_HookHuntress(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CInter_HookHuntress::Clone(void* const& pArg) {
	CInter_HookHuntress* pInstnace = new CInter_HookHuntress(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CInter_HookHuntress::CInter_HookHuntress(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CInter_HookHuntress::CInter_HookHuntress(const CInter_HookHuntress& rhs)
	: CObject(rhs) {
}

void CInter_HookHuntress::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	Safe_Release(m_pCollider);
}

const HRESULT CInter_HookHuntress::Init_Create(void) {
	return S_OK;
}

const HRESULT CInter_HookHuntress::Init_Clone(void* const& pArg) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_VtxAnimModel"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::SAMPLE), TEXT("Model_Inter_HookHuntress"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_AABB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	m_pTransform->Set_Row(CTransform::ERow::POS, XMVectorSet(-2.f, 0.f, 5.f, 1.f));
	return hr;
}

const _ubyte CInter_HookHuntress::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();

	CSlasher_Huntress* pSlasher = dynamic_cast<CSlasher_Huntress*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::SAMPLE), TEXT("Slasher_Huntress")));
	if (nullptr == pSlasher)
		return OBJ_NOEVENT;
	CCollider* pSlaherCollider = dynamic_cast<CCollider*>(pSlasher->Get_Component(TEXT("Collider")));
	if (nullptr == pSlaherCollider)
		return OBJ_NOEVENT;

	Update_Network();

	//state
	switch (m_eState){
	case ESTATE::IDLE:
		m_dTime = 0.0;
		m_bLoop = true;
		m_pModel->Set_Anim("SK_HookHuntress.ao|SK_DeerHanger_Struggle");
		break;
	case ESTATE::HANGING:
		m_bLoop = false;
		m_pModel->Set_Anim("SK_HookHuntress.ao|SK_DeerHanger_GetHookedIn_K27");
		break;
	case ESTATE::KILL:
		m_bLoop = false;
		m_pModel->Set_Anim("SK_HookHuntress.ao|SK_DeerHanger_GetHookedIn_K27");
		break;
	case ESTATE::BREAK:
		m_bLoop = false;
		m_pModel->Set_Anim("SK_HookHuntress.ao|SK_DeerHangers_BreakEnd");
		break;
	case ESTATE::BREAK_IDLE:
		m_bLoop = true;
		m_pModel->Set_Anim("SK_HookHuntress.ao|SK_DeerHangers_Broken_Idle");
		break;
	}
	m_iAnimFinish = m_pModel->Update_Anim(dTimeDelta, m_bLoop);
	m_pCollider->Update(m_pTransform->Get_Matrix());
	//collsion
	if (true == m_pCollider->Is_Collision(pSlaherCollider))
		m_eState = ESTATE::HANGING;

	return OBJ_NOEVENT;
}

void CInter_HookHuntress::Update_Edit(void) {
	ImGui::InputText("Model", m_szModel, IM_ARRAYSIZE(m_szModel));
	m_pTransform->Update_Edit(0);
	m_pCollider->Update_Edit(2);
}

void CInter_HookHuntress::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (true == pGame_Instance->IsIn_Frustum_World(m_pTransform->Get_Row(CTransform::ERow::POS), 15.f))
		m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CInter_HookHuntress::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Matrix_Transpose(), sizeof(_float4x4));
		for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
			m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
			m_pModel->Render(i, m_pShader, 0);
		}
		break;
	}
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		m_pCollider->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CInter_HookHuntress::Save_Data(Json::Value& root) {
	//string str = m_szModel;
	//root["Model"] = str;

	//m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CInter_HookHuntress::Load_Data(Json::Value& root) {
	/*string str = root["Model"].asString();
	wstring wstr{ str.begin(), str.end() };
	strcpy_s(m_szModel, str.c_str());
	__super::Add_Component(static_cast<_ubyte>(EScene::SAMPLE), wstr.c_str(), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));

	m_pTransform->Load_Data(root["Transform"]);*/
}

void CInter_HookHuntress::Update_Network(const string& pKey) {
	Json::Value root;
	if ("Set_State" == pKey) {
		root["State"] = static_cast<_int>(m_eState);
		m_pNetwork->Set_Send(m_iId, pKey, root);
	}
}

void CInter_HookHuntress::Update_Network(void) {
	list<Json::Value> listRecv = m_pNetwork->Get_Recv(m_iId);
	for (auto& iter : listRecv) {
		string key = iter["Key"].asString();
		if ("Set_State" == key) {
			m_eState = static_cast<ESTATE>(iter["State"].asInt());
		}
	}
	listRecv.clear();
}
