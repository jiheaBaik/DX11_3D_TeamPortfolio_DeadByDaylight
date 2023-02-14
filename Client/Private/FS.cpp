#include "pch.h"
#include "FS.h"

CFS::CFS(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, class CNetwork* const& pNetwork)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pNetwork(pNetwork) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pNetwork);
}

void CFS::Delete(void) {
	Safe_Release(m_pNetwork);
	Safe_Release(m_pContext);
	Safe_Release (m_pDevice);
}

const HRESULT CFS::Init(_uint iAnimIndex) {
	return S_OK;
}

const HRESULT CFS::Init(_char* pModelName, _double dLerpTime){
	return S_OK;
}
