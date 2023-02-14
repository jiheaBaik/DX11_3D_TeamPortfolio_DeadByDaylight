#include "pch.h"
#include "Weapon_Spirit_Hand_Lobby.h"
#include "Slasher_Spirit_Lobby.h"

CWeapon_Spirit_Hand_Lobby* const CWeapon_Spirit_Hand_Lobby::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CWeapon_Spirit_Hand_Lobby* pInstance = new CWeapon_Spirit_Hand_Lobby(pDevice, pContext, pNetwork);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CObject* const CWeapon_Spirit_Hand_Lobby::Clone(void* const& pArg) {
	CWeapon_Spirit_Hand_Lobby* pInstance = new CWeapon_Spirit_Hand_Lobby(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CWeapon_Spirit_Hand_Lobby::CWeapon_Spirit_Hand_Lobby(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CWeapon_Spirit_Hand_Lobby::CWeapon_Spirit_Hand_Lobby(const CWeapon_Spirit_Hand_Lobby& rhs)
	: CObject(rhs) {
}

void CWeapon_Spirit_Hand_Lobby::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pModel);
	//Safe_Release(m_pCollider);
}

const HRESULT CWeapon_Spirit_Hand_Lobby::Init_Create(void) {
	return S_OK;
}

const HRESULT CWeapon_Spirit_Hand_Lobby::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Transform"), TEXT("Transform"), reinterpret_cast<CComponent*&>(m_pTransform));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Shader_Model"), TEXT("Shader"), reinterpret_cast<CComponent*&>(m_pShader));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::LOBBY), TEXT("Model_Weapon_Spirit_Hand_Lobby"), TEXT("Model"), reinterpret_cast<CComponent*&>(m_pModel));

	return hr;
}

const _ubyte CWeapon_Spirit_Hand_Lobby::Update(const _double& dTimeDelta) {

	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();


	return OBJ_NOEVENT;
}

void CWeapon_Spirit_Hand_Lobby::Update_Edit(void) {
	m_pTransform->Update_Edit(0);
}

void CWeapon_Spirit_Hand_Lobby::Late_Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	CSlasher_Spirit_Lobby* pSlasher = dynamic_cast<CSlasher_Spirit_Lobby*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::LOBBY), TEXT("Slasher_Spirit_Lobby")));
	if (nullptr == pSlasher)
		return;
	CTransform* pSlasherTransform = dynamic_cast<CTransform*>(pSlasher->Get_Component(TEXT("Transform")));
	if (nullptr == pSlasherTransform)
		return;
	CModel* pModel = dynamic_cast<CModel*>(pSlasher->Get_Component(TEXT("Model")));
	_matrix BoneMatrix = pModel->Get_BoneMatrix("joint_FingerART_01");

	_matrix SocketMatrix;

	SocketMatrix = BoneMatrix * pSlasherTransform->Get_Matrix();

	m_pTransform->Set_Row(CTransform::ERow::RIGHT, -SocketMatrix.r[0]);
	m_pTransform->Set_Row(CTransform::ERow::UP, SocketMatrix.r[1]);
	m_pTransform->Set_Row(CTransform::ERow::LOOK, -SocketMatrix.r[2]);
	m_pTransform->Set_Row(CTransform::ERow::POS, SocketMatrix.r[3]);

	if (pSlasher->Get_Dissolve() > 2.0f) {
		m_pRenderer->Add_Object(CRenderer::EOrder::NONALPHABLEND, this);
		m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
	}
}

const HRESULT CWeapon_Spirit_Hand_Lobby::Render(const CRenderer::EOrder& eOrder) {
	switch (eOrder) {
	case CRenderer::EOrder::NONALPHABLEND: {
		CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
		CSlasher_Spirit_Lobby* pSlasher = dynamic_cast<CSlasher_Spirit_Lobby*>(pGame_Instance->Get_Object_Clone(static_cast<_int>(EScene::LOBBY), TEXT("Slasher_Spirit_Lobby")));

		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		pCamera->Set_RawValue(m_pShader);

		//if (CSlasher_Spirit_Lobby::ESTATE::IDLE_THROW == pSlasher->Get_State() && pSlasher->Get_RenderWeapon() == true) {
			m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_Float4x4_Transpose(), sizeof(_float4x4));
			for (_uint i = 0; i < m_pModel->Get_NumMesh(); ++i) {
				m_pModel->Set_Resource(i, aiTextureType_DIFFUSE, m_pShader, "g_DiffuseTexture");
				m_pModel->Render(i, m_pShader, 0);
			}
		//}
		break;
	}
										 break;
	case CRenderer::EOrder::NONLIGHT: {
		CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
		////m_pCollider->Render(pCamera);
		//m_pCollider_FPV->Render(pCamera);
		break;
	}
	}
	return S_OK;
}

const HRESULT CWeapon_Spirit_Hand_Lobby::Save_Data(Json::Value& root) {

	return S_OK;
}

void CWeapon_Spirit_Hand_Lobby::Load_Data(Json::Value& root) {

}
