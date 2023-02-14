#include "Enviro.h"

CEnviro::CEnviro(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork){
}

CEnviro::CEnviro(const CEnviro& rhs)
	: CObject(rhs) {
}

void CEnviro::Delete(void) {
	__super::Delete();
}

ID3D11ShaderResourceView* const CEnviro::Get_CubeSRV(void) {
	return m_pCubeSRV;
}
