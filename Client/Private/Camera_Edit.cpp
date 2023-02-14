#include "pch.h"
#include "Camera_Edit.h"
#ifdef _DEBUG
#include "UI_Image.h"
#endif // _DEBUG


CCamera_Edit* const CCamera_Edit::Create(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork) {
	CCamera_Edit* pInstnace = new CCamera_Edit(pDevice, pContext, pNetwork);
	if (FAILED(pInstnace->Init_Create()))
		Safe_Release(pInstnace);
	return pInstnace;
}

CObject* const CCamera_Edit::Clone(void* const& pArg) {
	CCamera_Edit* pInstnace = new CCamera_Edit(*this);
	if (FAILED(pInstnace->Init_Clone(pArg)))
		Safe_Release(pInstnace);
	return pInstnace;
}

CCamera_Edit::CCamera_Edit(ID3D11Device* const& pDevice, ID3D11DeviceContext* const& pContext, CNetwork* const& pNetwork)
	: CCamera(pDevice, pContext, pNetwork) {
}

CCamera_Edit::CCamera_Edit(const CCamera_Edit& rhs)
	: CCamera(rhs) {
}

void CCamera_Edit::Delete(void) {
	__super::Delete();

//#ifdef _DEBUG
//	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
//	CUI_Image* pImage = dynamic_cast<CUI_Image*>(pGame_Instance->Get_Object_Clone(pGame_Instance->Get_Scene(), L"UI_Play_Panel_First"));
//	if (nullptr != pImage)
//		pImage->Set_Active(true);
//#endif // _DEBUG

}

const HRESULT CCamera_Edit::Init_Create(void) {
	return S_OK;
}

const HRESULT CCamera_Edit::Init_Clone(void* const& pArg) {
	HRESULT hr = S_OK;
	hr |= __super::Init_Clone(pArg);
	return hr;
}

const _ubyte CCamera_Edit::Update(const _double& dTimeDelta) {
	CGame_Instance* pGame_Instance = CGame_Instance::Get_Instance();
	if (pGame_Instance->Get_Input_KeyboardKey(DIK_UP) & 0x80)
		m_pTransform->Translate(m_pTransform->Get_Row(CTransform::ERow::LOOK), m_fSpeed, dTimeDelta);
	if (pGame_Instance->Get_Input_KeyboardKey(DIK_LEFT) & 0x80)
		m_pTransform->Translate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), -m_fSpeed, dTimeDelta);
	if (pGame_Instance->Get_Input_KeyboardKey(DIK_RIGHT) & 0x80)
		m_pTransform->Translate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), m_fSpeed, dTimeDelta);
	if (pGame_Instance->Get_Input_KeyboardKey(DIK_DOWN) & 0x80)
		m_pTransform->Translate(m_pTransform->Get_Row(CTransform::ERow::LOOK), -m_fSpeed, dTimeDelta);

	if (pGame_Instance->Get_Input_MouseButton(CInput::EMouseButton::R)) {
		_long MouseMove = 0;
		if (MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::X))
			m_pTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * 0.1f, dTimeDelta);
		if (MouseMove = pGame_Instance->Get_Input_MouseMove(CInput::EMouseMove::Y))
			m_pTransform->Rotate(m_pTransform->Get_Row(CTransform::ERow::RIGHT), MouseMove * 0.1f, dTimeDelta);
	}
	return OBJ_NOEVENT;
}

void CCamera_Edit::Update_Edit(void) {
	ImGui::DragFloat("Speed", &m_fSpeed);
	m_pTransform->Update_Edit(0);
}

void CCamera_Edit::Late_Update(const _double& dTimeDelta) {
	Set_ViewMatrix();
}

const HRESULT CCamera_Edit::Render(const CRenderer::EOrder& eOrder) {
	return S_OK;
}
