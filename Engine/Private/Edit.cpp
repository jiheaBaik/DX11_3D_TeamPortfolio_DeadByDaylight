#include "Edit.h"

CEdit::CEdit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CEdit::Delete(void) {
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

const _bool& CEdit::Get_Open(void) const {
	return m_bOpen;
}

void CEdit::Set_Open(const _bool& bOpen) {
	m_bOpen = bOpen;
}