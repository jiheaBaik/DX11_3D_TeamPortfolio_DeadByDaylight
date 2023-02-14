#include "pch.h"
#include "Light_Lobby.h"

CLight_Lobby* const CLight_Lobby::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CLight_Lobby* pInstnace = new CLight_Lobby(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CLight_Lobby::Clone(void* const& pArg) {
	CLight_Lobby* pInstnace = new CLight_Lobby(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CLight_Lobby::CLight_Lobby(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CLight(pDevice, pContext, pNetwork) {
}

CLight_Lobby::CLight_Lobby(const CLight_Lobby& rhs)
	: CLight(rhs) {
}

void CLight_Lobby::Delete(void) {
	__super::Delete();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}

const HRESULT CLight_Lobby::Init_Create(void) {
	return S_OK;
}

const HRESULT CLight_Lobby::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	return hr;
}

const _ubyte CLight_Lobby::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	return OBJ_NOEVENT;
}

void CLight_Lobby::Late_Update(const _double& dTimeDelta) {
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CLight_Lobby::Render(const CRenderer::EOrder& eOrder) {
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	//m_pCollider->Render(pCamera);
	return S_OK;
}
const HRESULT CLight_Lobby::Save_Data(Json::Value& root) {
	root["Key"] = string{ m_szKey };
	root["Type"] = static_cast<_int>( m_sDesc.eType);
	root["Range"] = m_sDesc.fRange;
	root["Diffuse"]["x"] = m_sDesc.vDiffuse.x;
	root["Diffuse"]["y"] = m_sDesc.vDiffuse.y;
	root["Diffuse"]["z"] = m_sDesc.vDiffuse.z;
	root["Ambient"]["x"] = m_sDesc.vAmbient.x;
	root["Ambient"]["y"] = m_sDesc.vAmbient.y;
	root["Ambient"]["z"] = m_sDesc.vAmbient.z;
	root["Specular"]["x"] = m_sDesc.vSpecular.x;
	root["Specular"]["y"] = m_sDesc.vSpecular.y;
	root["Specular"]["z"] = m_sDesc.vSpecular.z;
	root["Angle"]["x"] = m_sDesc.fAngle.x;
	root["Angle"]["y"] = m_sDesc.fAngle.y;
	m_pTransform->Save_Data(root["Transform"]);
	return S_OK;
}

void CLight_Lobby::Load_Data(Json::Value& root) {
	strcpy_s(m_szKey, root["Key"].asString().c_str());

	
	m_sDesc.eType = static_cast<EType>(root["Type"].asInt());
	m_sDesc.fRange = root["Range"].asFloat();
	m_sDesc.vDiffuse.x = root["Diffuse"]["x"].asFloat();
	m_sDesc.vDiffuse.y = root["Diffuse"]["y"].asFloat();
	m_sDesc.vDiffuse.z = root["Diffuse"]["z"].asFloat();
	m_sDesc.vAmbient.x = root["Ambient"]["x"].asFloat();
	m_sDesc.vAmbient.y = root["Ambient"]["y"].asFloat();
	m_sDesc.vAmbient.z = root["Ambient"]["z"].asFloat();
	m_sDesc.vSpecular.x = root["Specular"]["x"].asFloat();
	m_sDesc.vSpecular.y = root["Specular"]["y"].asFloat();
	m_sDesc.vSpecular.z = root["Specular"]["z"].asFloat();
	m_sDesc.fAngle.x = root["Angle"]["x"].asFloat();
	m_sDesc.fAngle.y = root["Angle"]["y"].asFloat();
	m_pTransform->Load_Data(root["Transform"]);

	s_vecLight.emplace_back(this);
	string str{ m_szKey };
	wstring wstr{ str.begin(),str.end() };
	s_mapLight.emplace(wstr, this);
}