#pragma once

#include "../Event.h"

#include <windowsx.h>

namespace photon
{
	enum class Win32KeyMap
	{
		None = 0,
		backspace = VK_BACK,
		tab = VK_TAB,
		enter = VK_RETURN,
		shift = VK_SHIFT,
		ctrl = VK_CONTROL,
		alt = VK_MENU,
		capsLock = VK_CAPITAL,
		esc = VK_ESCAPE,
		space = VK_SPACE,
		pageUp = VK_PRIOR,
		pageDown = VK_NEXT,
		end = VK_END,
		home = VK_HOME,
		leftArrow = VK_LEFT,
		upArrow = VK_UP,
		rightArrow = VK_RIGHT,
		downArrow = VK_DOWN,
		_0 = 0x30, // 英文字母上方横排数字键
		_1 = 0x31,
		_2 = 0x32,
		_3 = 0x33,
		_4 = 0x34,
		_5 = 0x35,
		_6 = 0x36,
		_7 = 0x37,
		_8 = 0x38,
		_9 = 0x39,
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		M = 0x4D,
		N = 0x4E,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,
		leftWin = VK_LWIN,
		rightWin = VK_RWIN,
		num0 = VK_NUMPAD0, // 数字小键盘
		num1 = VK_NUMPAD1,
		num2 = VK_NUMPAD2,
		num3 = VK_NUMPAD3,
		num4 = VK_NUMPAD4,
		num5 = VK_NUMPAD5,
		num6 = VK_NUMPAD6,
		num7 = VK_NUMPAD7,
		num8 = VK_NUMPAD8,
		num9 = VK_NUMPAD9,
		mul = VK_MULTIPLY,
		add = VK_ADD,
		separator = VK_SEPARATOR, // 分隔符
		subtract = VK_SUBTRACT,
		decimal = VK_DECIMAL, // 句号
		div = VK_DIVIDE,
		f1 = VK_F1,
		f2 = VK_F2,
		f3 = VK_F3,
		f4 = VK_F4,
		f5 = VK_F5,
		f6 = VK_F6,
		f7 = VK_F7,
		f8 = VK_F8,
		f9 = VK_F9,
		f10 = VK_F10,
		f11 = VK_F11,
		f12 = VK_F12,
		lShift = VK_LSHIFT,
		rShift = VK_RSHIFT,
		lCtrl = VK_LCONTROL,
		rCtrl = VK_RCONTROL,
		lAlt = VK_LMENU,
		rAlt = VK_RMENU,
	};

	using VirtualKey = Win32KeyMap;


	class KeyEvent : public Event 
	{
	public:
		KeyEvent() = delete;
		KeyEvent(const KeyEvent& event);
		KeyEvent(const Win32WndProcInfo& wndProcInfo);
		KeyEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		EventCategory GetCategory() const final { return EventCategory::Key; }
		virtual EventType GetType() const override = 0;
		virtual std::string ToString() const override;

		VirtualKey GetVirtualKey() const { return m_Key; }

	protected:
		virtual void InitVars();

	protected:
		VirtualKey m_Key = VirtualKey::None;
	};
}