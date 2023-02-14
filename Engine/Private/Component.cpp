#include "Component.h"

CComponent::CComponent(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_bClone(false) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent& rhs)
	: CBase(),
	m_pDevice(rhs.m_pDevice),
	m_pContext(rhs.m_pContext),
	m_bClone(true) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CComponent::Delete(void) {
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}