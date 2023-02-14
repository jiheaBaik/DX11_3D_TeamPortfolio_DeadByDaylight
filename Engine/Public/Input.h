#pragma once
#include "Base.h"

BEGIN_NAMESPACE(Engine)
class CInput final : public CBase {
	DECLARE_SINGLETON(CInput)
public:
	enum class EMouseMove : _ubyte { X, Y, WHEEL, END };
	enum class EMouseButton : _ubyte { L, R, WHEEL, END };
private:
	explicit CInput(void);
	virtual ~CInput(void) override = default;
	virtual void Delete(void) override;
public:
	const HRESULT Init(const HINSTANCE& hInst, const HWND& hWnd);
	void Update(void);
public:
	const _long& Get_MouseMove(const EMouseMove& eMouseMove) const;
	const _byte Get_MouseButton(const EMouseButton& eMouseButton) const;
	const _bool Get_MouseButtonUp(const EMouseButton& eMouseButton);
	const _bool Get_MouseButtonDown(const EMouseButton& eMouseButton);
	const _byte& Get_KeyboardKey(const _ubyte& byKey) const;
	const _bool Get_KeyboardKeyUp(const _ubyte& byKey);
	const _bool Get_KeyboardKeyDown(const _ubyte& byKey);
private:
	LPDIRECTINPUT8 m_pInput = nullptr;
	LPDIRECTINPUTDEVICE8 m_pMouse = nullptr;
	LPDIRECTINPUTDEVICE8 m_pKeyboard = nullptr;
private:
	DIMOUSESTATE m_MouseState;
	_bool m_bMouseButtonUp[static_cast<_ubyte>(EMouseButton::END)];
	_bool m_bMouseButtonDown[static_cast<_ubyte>(EMouseButton::END)];
	_byte m_byKeyboardState[256];
	_bool m_bKeyUp[256];
	_bool m_bKeyDown[256];
};
END_NAMESPACE