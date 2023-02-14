#include "Effect.h"
#include "Shader.h"
#include "Transform.h"

CEffect::CEffect(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CObject(pDevice, pContext, pNetwork) {
}

CEffect::CEffect(const CEffect& rhs)
	: CObject(rhs) {
}

void CEffect::Delete(void) {
	__super::Delete();
}

CTransform* CEffect::Get_EffectTransform(){
	if (m_pEffect_Transform == nullptr)
		m_pEffect_Transform = dynamic_cast<CTransform*>(Get_Component(TEXT("Transform")));
	return m_pEffect_Transform;
}
