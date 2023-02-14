#include "pch.h"
#include "Slasher_State.h"

CSlasher_State* const CSlasher_State::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, void* pArg) {
	CSlasher_State* pInstance = new CSlasher_State(pDevice, pContext, pNetwork, pArg);
	if (FAILED(pInstance->Init_Create()))
		Safe_Release(pInstance);
	return pInstance;
}

CSlasher_State::CSlasher_State(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork, void* pArg)
	: CFSM(pDevice, pContext, pNetwork, pArg) {

}

void CSlasher_State::Delete(void) {

}

const HRESULT CSlasher_State::Init_Create(void) {

	return S_OK;
}

void CSlasher_State::Update(const _double& dTimeDelta) {

}

void CSlasher_State::Start() {

}

void CSlasher_State::Key_Input() {

}

void CSlasher_State::Change_CommonState(const _double& dTimeDelta) {

}

void CSlasher_State::Change_State(const _double& dTimeDelta) {

}
