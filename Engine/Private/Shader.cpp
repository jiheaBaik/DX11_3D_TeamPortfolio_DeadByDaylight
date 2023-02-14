#include "Shader.h"
#include "Texture.h"

CShader* const CShader::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath) {
	CShader* pInstance = new CShader(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(pFilePath)))
		Safe_Release(pInstance);
	return pInstance;
}

CShader* const CShader::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, const _tchar* const& pFilePath, D3D11_INPUT_ELEMENT_DESC* const& pInput_Element_Desc, const _uint& iNumElement) {
	CShader* pInstance = new CShader(pDevice, pContext);
	if (FAILED(pInstance->Init_Create(pFilePath, pInput_Element_Desc, iNumElement)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* const CShader::Clone(void* const& pArg) {
	CShader* pInstance = new CShader(*this);
	if (FAILED(pInstance->Init_Clone(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

CShader::CShader(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CComponent(pDevice, pContext) {
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs),
	m_pEffect(rhs.m_pEffect),
	m_vecsDesc(rhs.m_vecsDesc) {
	Safe_AddRef(m_pEffect);
	for (auto& iter : m_vecsDesc) {
		Safe_AddRef(iter.pPass);
		Safe_AddRef(iter.pInputLayout);
	}
}

void CShader::Delete(void) {
	__super::Delete();
	for (auto& iter : m_vecsDesc) {
		Safe_Release(iter.pInputLayout);
		Safe_Release(iter.pPass);
	}
	m_vecsDesc.clear();
	Safe_Release(m_pEffect);
}

const HRESULT CShader::Init_Create(const _tchar* const& pFileName) {
	_uint iHLSLFlags = 0;
#ifdef  _DEBUG
	iHLSLFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHLSLFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHLSLFlags, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC Technique_Desc;
	ZeroMemory(&Technique_Desc, sizeof(D3DX11_TECHNIQUE_DESC));
	pTechnique->GetDesc(&Technique_Desc);

	for (_uint i = 0; i < Technique_Desc.Passes; ++i) {
		SDesc sDesc;
		ZeroMemory(&sDesc, sizeof(SDesc));
		sDesc.pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC Pass_Desc;
		ZeroMemory(&Pass_Desc, sizeof(D3DX11_PASS_DESC));
		sDesc.pPass->GetDesc(&Pass_Desc);
		sDesc.pInputLayout = nullptr;

		m_vecsDesc.emplace_back(sDesc);
	}
	return S_OK;
}

const HRESULT CShader::Init_Create(const _tchar* const& pFileName, D3D11_INPUT_ELEMENT_DESC* const& pInput_Element_Desc, const _uint& iNumElement) {
	_uint iHLSLFlags = 0;
#ifdef  _DEBUG
	iHLSLFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHLSLFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHLSLFlags, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC Technique_Desc;
	ZeroMemory(&Technique_Desc, sizeof(D3DX11_TECHNIQUE_DESC));
	pTechnique->GetDesc(&Technique_Desc);

	for (_uint i = 0; i < Technique_Desc.Passes; ++i) {
		SDesc sDesc;
		ZeroMemory(&sDesc, sizeof(SDesc));
		sDesc.pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC Pass_Desc;
		ZeroMemory(&Pass_Desc, sizeof(D3DX11_PASS_DESC));
		sDesc.pPass->GetDesc(&Pass_Desc);
		if (FAILED(m_pDevice->CreateInputLayout(pInput_Element_Desc, iNumElement, Pass_Desc.pIAInputSignature, Pass_Desc.IAInputSignatureSize, &sDesc.pInputLayout)))
			return E_FAIL;

		m_vecsDesc.emplace_back(sDesc);
	}
	return S_OK;
}

const HRESULT CShader::Init_Clone(void* const& pArg) {
	return S_OK;
}

const HRESULT CShader::Set_RawValue(const char* const& pName, const void* const& pData, const _uint& iByteCount) {
	if (nullptr == m_pEffect)
		return E_FAIL;
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pName);
	if (nullptr == pVariable)
		return E_FAIL;
	return pVariable->SetRawValue(pData, 0, iByteCount);
}

const HRESULT CShader::Set_Resource(const char* const& pName, ID3D11ShaderResourceView* const& pShaderResourceView) {
	if (nullptr == m_pEffect)
		return E_FAIL;
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pName);
	if (nullptr == pVariable)
		return E_FAIL;
	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable = pVariable->AsShaderResource();
	if (nullptr == pShaderResourceVariable)
		return E_FAIL;
	return pShaderResourceVariable->SetResource(pShaderResourceView);
}

const HRESULT CShader::Set_Resource(const char* const& pName, ID3D11UnorderedAccessView* const& pUnorderedAccessView) {
	if (nullptr == m_pEffect)
		return E_FAIL;
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pName);
	if (nullptr == pVariable)
		return E_FAIL;
	ID3DX11EffectUnorderedAccessViewVariable* pUnorderedAccessViewVariable = pVariable->AsUnorderedAccessView();
	if (nullptr == pUnorderedAccessViewVariable)
		return E_FAIL;
	return pUnorderedAccessViewVariable->SetUnorderedAccessView(pUnorderedAccessView);
}

const HRESULT CShader::Apply(const _uint& iDesc) {
	if (iDesc >= m_vecsDesc.size())
		return E_FAIL;
	if (nullptr == m_vecsDesc[iDesc].pPass)
		return E_FAIL;
	if (nullptr != m_vecsDesc[iDesc].pInputLayout)
		m_pContext->IASetInputLayout(m_vecsDesc[iDesc].pInputLayout);
	m_vecsDesc[iDesc].pPass->Apply(0, m_pContext);
	return S_OK;
}