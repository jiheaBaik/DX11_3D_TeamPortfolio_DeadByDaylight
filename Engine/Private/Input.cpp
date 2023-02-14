#include "Input.h"

IMPLEMENT_SINGLETON(CInput)

CInput::CInput(void)
	: CBase() {
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
	memset(m_bMouseButtonUp, true, sizeof(_bool) * static_cast<_ubyte>(EMouseButton::END));
	ZeroMemory(m_bMouseButtonDown, sizeof(_bool) * static_cast<_ubyte>(EMouseButton::END));
	ZeroMemory(m_byKeyboardState, sizeof(_byte) * 256);
	memset(m_bKeyUp, true, sizeof(_bool) * 256);
	ZeroMemory(m_bKeyDown, sizeof(_bool) * 256);
}

void CInput::Delete(void) {
}

const HRESULT CInput::Init(const HINSTANCE& hInst, const HWND& hWnd) {
	HRESULT hr = S_OK;
	hr |= DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_pInput), nullptr);
	hr |= m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr);

	hr |= m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr |= m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	hr |= m_pKeyboard->Acquire();

	hr |= m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr);
	hr |= m_pMouse->SetDataFormat(&c_dfDIMouse);
	hr |= m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	hr |= m_pMouse->Acquire();
	return hr;
}

void CInput::Update(void) {
	if (nullptr == m_pMouse || nullptr == m_pKeyboard)
		return;
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
	m_pKeyboard->GetDeviceState(256, m_byKeyboardState);
}

const _long& CInput::Get_MouseMove(const EMouseMove& eMouseMove) const {
	return *(reinterpret_cast<const _long*>(&m_MouseState) + static_cast<_ubyte>(eMouseMove));
}

const _byte CInput::Get_MouseButton(const EMouseButton& eMouseButton) const {
	return m_MouseState.rgbButtons[static_cast<_ubyte>(eMouseButton)];
}

const _bool CInput::Get_MouseButtonUp(const EMouseButton& eMouseButton) {
	if (!m_MouseState.rgbButtons[static_cast<_ubyte>(eMouseButton)])
		if (false == m_bMouseButtonUp[static_cast<_ubyte>(eMouseButton)]) {
			m_bMouseButtonUp[static_cast<_ubyte>(eMouseButton)] = true;
			return true;
		}
	if (m_MouseState.rgbButtons[static_cast<_ubyte>(eMouseButton)])
		m_bMouseButtonUp[static_cast<_ubyte>(eMouseButton)] = false;
	return false;
}

const _bool CInput::Get_MouseButtonDown(const EMouseButton& eMouseButton) {
	if (m_MouseState.rgbButtons[static_cast<_ubyte>(eMouseButton)])
		if (false == m_bMouseButtonDown[static_cast<_ubyte>(eMouseButton)]) {
			m_bMouseButtonDown[static_cast<_ubyte>(eMouseButton)] = true;
			return true;
		}
	if (!m_MouseState.rgbButtons[static_cast<_ubyte>(eMouseButton)])
		m_bMouseButtonDown[static_cast<_ubyte>(eMouseButton)] = false;
	return false;
}

const _byte& CInput::Get_KeyboardKey(const _ubyte& byKey) const {
	return m_byKeyboardState[byKey];
}

const _bool CInput::Get_KeyboardKeyUp(const _ubyte& byKey) {
	if (!(m_byKeyboardState[byKey] & 0x80))
		if (false == m_bKeyUp[byKey]) {
			m_bKeyUp[byKey] = true;
			return true;
		}
	if (m_byKeyboardState[byKey] & 0x80)
		m_bKeyUp[byKey] = false;
	return false;
}

const _bool CInput::Get_KeyboardKeyDown(const _ubyte& byKey) {
	if (m_byKeyboardState[byKey] & 0x80)
		if (false == m_bKeyDown[byKey]) {
			m_bKeyDown[byKey] = true;
			return true;
		}
	if (!(m_byKeyboardState[byKey] & 0x80))
		m_bKeyDown[byKey] = false;
	return false;
}
