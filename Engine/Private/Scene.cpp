#include "Scene.h"
#include "Network.h"

CScene::CScene(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CBase(),
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pNetwork(pNetwork) {
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pNetwork);
}

void CScene::Delete(void) {
	Safe_Release(m_pNetwork);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}