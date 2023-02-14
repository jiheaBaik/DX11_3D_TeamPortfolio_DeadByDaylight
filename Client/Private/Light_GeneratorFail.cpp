#include "pch.h"
#include "Light_GeneratorFail.h"

CLight_GeneratorFail* const CLight_GeneratorFail::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CLight_GeneratorFail* pInstnace = new CLight_GeneratorFail(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CLight_GeneratorFail::Clone(void* const& pArg) {
	CLight_GeneratorFail* pInstnace = new CLight_GeneratorFail(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CLight_GeneratorFail::CLight_GeneratorFail(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CLight(pDevice, pContext, pNetwork) {
}

CLight_GeneratorFail::CLight_GeneratorFail(const CLight_GeneratorFail& rhs)
	: CLight(rhs) {
}

void CLight_GeneratorFail::Delete(void) {
	__super::Delete();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pCollider);
}

const HRESULT CLight_GeneratorFail::Init_Create(void) {
	return S_OK;
}

const HRESULT CLight_GeneratorFail::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Renderer"), TEXT("Renderer"), reinterpret_cast<CComponent*&>(m_pRenderer));
	hr |= __super::Add_Component(static_cast<_ubyte>(EScene::STATIC), TEXT("Collider_OBB"), TEXT("Collider"), reinterpret_cast<CComponent*&>(m_pCollider));
	m_bOnOff = true;
	return hr;
}

const _ubyte CLight_GeneratorFail::Update(const _double& dTimeDelta) {
	m_pCollider->Update(m_pTransform->Get_Matrix());
	if (m_bOnOff) {
		m_dOffTime += dTimeDelta;
		m_sDesc.vDiffuse.x -= static_cast<_float>(dTimeDelta) * 20.f;
		m_sDesc.vDiffuse.y -= static_cast<_float>(dTimeDelta) * 20.f;
		m_sDesc.vDiffuse.z -= static_cast<_float>(dTimeDelta) * 20.f;		
		if (m_sDesc.vDiffuse.z < 0.f)
			m_sDesc.vDiffuse.z = 0.f;
		
		m_sDesc.vAmbient.x -= static_cast<_float>(dTimeDelta) * 20.f;
		m_sDesc.vAmbient.y -= static_cast<_float>(dTimeDelta) * 20.f;
		m_sDesc.vAmbient.z -= static_cast<_float>(dTimeDelta) * 20.f;
		if (m_sDesc.vAmbient.z < 0.f) {
			m_sDesc.vAmbient.z = 0.f;
			return OBJ_ERASE;
		}
	}		
	return OBJ_NOEVENT;
}

void CLight_GeneratorFail::Late_Update(const _double& dTimeDelta) {	
	m_pRenderer->Add_Object(CRenderer::EOrder::NONLIGHT, this);
}

const HRESULT CLight_GeneratorFail::Render(const CRenderer::EOrder& eOrder) {
	CCamera* pCamera = CCamera::Get_Camera(static_cast<_uint>(0));
	//m_pCollider->Render(pCamera);
	return S_OK;
}
const HRESULT CLight_GeneratorFail::Save_Data(Json::Value& root) {
	//root["Key"] = string{ m_szKey };
	//root["Type"] = static_cast<_int>( m_sDesc.eType);
	//root["Range"] = m_sDesc.fRange;
	//root["Diffuse"]["x"] = m_sDesc.vDiffuse.x;
	//root["Diffuse"]["y"] = m_sDesc.vDiffuse.y;
	//root["Diffuse"]["z"] = m_sDesc.vDiffuse.z;
	//root["Ambient"]["x"] = m_sDesc.vAmbient.x;
	//root["Ambient"]["y"] = m_sDesc.vAmbient.y;
	//root["Ambient"]["z"] = m_sDesc.vAmbient.z;
	//root["Specular"]["x"] = m_sDesc.vSpecular.x;
	//root["Specular"]["y"] = m_sDesc.vSpecular.y;
	//root["Specular"]["z"] = m_sDesc.vSpecular.z;
	//root["Angle"] = m_sDesc.fAngle;
	//m_pTransform->Save_Data(root["Transform"]);
	return E_FAIL;
}

void CLight_GeneratorFail::Load_Data(Json::Value& root) {
	//strcpy_s(m_szKey, root["Key"].asString().c_str());
	//
	//m_sDesc.eType = static_cast<EType>(root["Type"].asInt());
	//m_sDesc.fRange = root["Range"].asFloat();
	//m_sDesc.vDiffuse.x = root["Diffuse"]["x"].asFloat();
	//m_sDesc.vDiffuse.y = root["Diffuse"]["y"].asFloat();
	//m_sDesc.vDiffuse.z = root["Diffuse"]["z"].asFloat();
	//m_sDesc.vAmbient.x = root["Ambient"]["x"].asFloat();
	//m_sDesc.vAmbient.y = root["Ambient"]["y"].asFloat();
	//m_sDesc.vAmbient.z = root["Ambient"]["z"].asFloat();
	//m_sDesc.vSpecular.x = root["Specular"]["x"].asFloat();
	//m_sDesc.vSpecular.y = root["Specular"]["y"].asFloat();
	//m_sDesc.vSpecular.z = root["Specular"]["z"].asFloat();
	//m_sDesc.fAngle = root["Angle"].asFloat();
	//m_sDesc.fAngle = root["Angle"].asBool();
	//m_pTransform->Load_Data(root["Transform"]);

	//s_vecLight.emplace_back(this);
	//string str{ m_szKey };
	//wstring wstr{ str.begin(),str.end() };
	//s_mapLight.emplace(wstr, this);
}

void CLight_GeneratorFail::Set_LightOnPos(_fvector pos, CLight::SDesc sDesc){
	m_pTransform->Set_Row(CTransform::ERow::POS, pos);
	m_bOnOff = true;
	m_dOffTime = 0.;
	m_sDesc.fRange = sDesc.fRange;
	m_sDesc.vDiffuse = sDesc.vDiffuse;
	m_sDesc.vAmbient = sDesc.vAmbient;
	m_sDesc.vSpecular = sDesc.vSpecular;
}
