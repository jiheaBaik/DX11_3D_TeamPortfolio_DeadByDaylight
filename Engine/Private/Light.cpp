#include "Light.h"
#include "Transform.h"
#include "Shader.h"

vector<class CLight*> CLight::s_vecLight;
map<wstring, class CLight*> CLight::s_mapLight;

CLight::CLight(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CLight::CLight(const CLight& rhs)
	: CObject(rhs) {
}

void CLight::Delete(void) {
	__super::Delete();
	Safe_Release(m_pTransform);
	for (auto pair = s_mapLight.begin(); pair != s_mapLight.end(); ++pair)
		if (this == pair->second) {
			s_mapLight.erase(pair);
			break;
		}
	for (auto pair = s_vecLight.begin(); pair != s_vecLight.end(); ++pair)
		if (this == *pair) {
			s_vecLight.erase(pair);
			break;
		}
}

const HRESULT CLight::Init_Clone(void* const& pArg) {
	if (nullptr != pArg) {
		memcpy(&m_sDesc, pArg, sizeof(SDesc));
		s_vecLight.emplace_back(this);
		if (nullptr != m_sDesc.pKey)
			s_mapLight.emplace(m_sDesc.pKey, this);
	}

	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	m_mapComponent.emplace(TEXT("Transform"), m_pTransform);
	Safe_AddRef(m_pTransform);

	return S_OK;
}

void CLight::Update_Edit(void) {
	ImGui::InputText("Key", m_szKey, IM_ARRAYSIZE(m_szKey));
	ImGui::Checkbox("OnOff", &m_bOnOff);
	if (ImGui::RadioButton("Dir", m_sDesc.eType == EType::DIR)) {
		m_sDesc.eType = EType::DIR;
	} ImGui::SameLine();
	if (ImGui::RadioButton("Point", m_sDesc.eType == EType::POINT)) {
		m_sDesc.eType = EType::POINT;
	}ImGui::SameLine();
	if (ImGui::RadioButton("Spot", m_sDesc.eType == EType::SPOT)) {
		m_sDesc.eType = EType::SPOT;
	}
	ImGui::DragFloat("Range", &m_sDesc.fRange, 0.001f);
	ImGui::DragFloat3("Diffuse", reinterpret_cast<_float*>(&m_sDesc.vDiffuse), 0.001f);
	ImGui::DragFloat3("Ambient", reinterpret_cast<_float*>(&m_sDesc.vAmbient), 0.001f);
	ImGui::DragFloat3("Specular", reinterpret_cast<_float*>(&m_sDesc.vSpecular), 0.001f);
	ImGui::InputFloat2("Angle", reinterpret_cast<_float*>(&m_sDesc.fAngle));
	m_pTransform->Update_Edit(0);
}

const CLight::EType& CLight::Get_Type(void) const {
	return m_sDesc.eType;
}

_vector CLight::Get_Pos(void) const {
	return m_pTransform->Get_Row(CTransform::ERow::POS);
}

_vector CLight::Get_Look(void) const {
	return m_pTransform->Get_Row(CTransform::ERow::LOOK);
}

const _float& CLight::Get_Range(void) const {
	return m_sDesc.fRange;
}

const _float4& CLight::Get_Diffuse(void) const {
	return m_sDesc.vDiffuse;
}

const _float4& CLight::Get_Ambient(void) const {
	return m_sDesc.vAmbient;
}

const _float4& CLight::Get_Specular(void) const {
	return m_sDesc.vSpecular;
}

const HRESULT CLight::Set_RawValue(CShader* const& pShader) {
	switch (m_sDesc.eType) {
	case CLight::EType::DIR:
		pShader->Set_RawValue("g_vLightDir", &m_pTransform->Get_Row(CTransform::ERow::LOOK), sizeof(_float3));
		break;
	case CLight::EType::POINT:
		pShader->Set_RawValue("g_vLightPos", &m_pTransform->Get_Row(CTransform::ERow::POS), sizeof(_float3));
		pShader->Set_RawValue("g_fLightRange", &m_sDesc.fRange, sizeof(_float));
		break;
	case CLight::EType::SPOT:
		pShader->Set_RawValue("g_vLightDir", &m_pTransform->Get_Row(CTransform::ERow::LOOK), sizeof(_float3));
		pShader->Set_RawValue("g_vLightPos", &m_pTransform->Get_Row(CTransform::ERow::POS), sizeof(_float3));
		pShader->Set_RawValue("g_fLightRange", &m_sDesc.fRange, sizeof(_float));
		pShader->Set_RawValue("g_fLightAngle", &m_sDesc.fAngle, sizeof(_float2));
		break;
	}
	pShader->Set_RawValue("g_vLightDiffuse", &m_sDesc.vDiffuse, sizeof(_float4));
	pShader->Set_RawValue("g_vLightAmbient", &m_sDesc.vAmbient, sizeof(_float4));
	pShader->Set_RawValue("g_vLightSpecular", &m_sDesc.vSpecular, sizeof(_float4));
	return S_OK;
}

const _bool& CLight::Get_OnOff(void) const {
	return m_bOnOff;
}

void CLight::Set_OnOff(const _bool& bOnOff) {
	m_bOnOff = bOnOff;
}

CLight* const CLight::Get_Light(const _uint& iLight) {
	if (iLight >= s_vecLight.size())
		return nullptr;
	return s_vecLight[iLight];
}

CLight* const CLight::Get_Light(const _tchar* const& pLightKey) {
	auto iter = find_if(s_mapLight.begin(), s_mapLight.end(), [&](map<wstring, class CLight*>::value_type pair) {
		return pair.first == pLightKey;
		});
	if (s_mapLight.end() == iter)
		return nullptr;
	return iter->second;
}

vector<class CLight*> const& CLight::Get_VecLight(void) {
	return s_vecLight;
}
