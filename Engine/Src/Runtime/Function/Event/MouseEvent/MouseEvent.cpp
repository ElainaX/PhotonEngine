#include "MouseEvent.h"

#include <format>
#include <windowsx.h>


namespace photon 
{
	/*
		MouseButtonDownEvent的实现
	*/
	MouseEvent::MouseEvent(const MouseEvent& event)
		: Event(event), m_bLButtonPressed(event.m_bLButtonPressed),
		m_bRButtonPressed(event.m_bRButtonPressed), m_bMButtonPressed(event.m_bMButtonPressed),
		m_bShiftPressed(event.m_bShiftPressed), m_bControlPressed(event.m_bControlPressed),
		m_MousePos(event.m_MousePos)
	{

	}
	MouseEvent::MouseEvent(const Win32WndProcInfo& wndProcInfo)
		: Event(wndProcInfo)
	{
		InitVars();
	}
	MouseEvent::MouseEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		: Event(hwnd, msg, wparam, lparam)
	{
		InitVars();
	}


	constexpr EventCategory MouseEvent::GetCategory() const
	{
		return EventCategory::Mouse;
	}

	std::string MouseEvent::ToString() const
	{
		return std::format(" > MousePos[{},{}] > LMR[{:d}{:d}{:d}] > CS[{:d}{:d}]", m_MousePos.x, m_MousePos.y, 
			IsLButtonPressed(), IsMButtonPressed(), IsRButtonPressed(), IsControlPressed(), IsShiftPressed());
	}

	bool MouseEvent::IsLButtonPressed() const
	{
		return m_bLButtonPressed;
	}

	bool MouseEvent::IsRButtonPressed() const
	{
		return m_bRButtonPressed;
	}

	bool MouseEvent::IsMButtonPressed() const
	{
		return m_bMButtonPressed;
	}

	bool MouseEvent::IsShiftPressed() const
	{
		return m_bShiftPressed;
	}

	bool MouseEvent::IsControlPressed() const
	{
		return m_bControlPressed;
	}

	Vector2i MouseEvent::GetMousePos() const
	{
		return m_MousePos;
	}

	void MouseEvent::InitVars()
	{
		m_bShiftPressed = m_WndProcInfo.wparam & MK_SHIFT;
		m_bControlPressed = m_WndProcInfo.wparam & MK_CONTROL;
		m_bLButtonPressed = m_WndProcInfo.wparam & MK_LBUTTON;
		m_bRButtonPressed = m_WndProcInfo.wparam & MK_RBUTTON;
		m_bMButtonPressed = m_WndProcInfo.wparam & MK_MBUTTON;

		m_MousePos[0] = GET_X_LPARAM(m_WndProcInfo.lparam);
		m_MousePos[1] = GET_Y_LPARAM(m_WndProcInfo.lparam);
	}	

	

}