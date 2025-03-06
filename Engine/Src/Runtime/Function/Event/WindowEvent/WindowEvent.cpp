#include "WindowEvent.h"

#include <format>

namespace photon 
{

	WindowEvent::WindowEvent(const WindowEvent& event)
		: Event(event), m_WindowRect(event.m_WindowRect), m_ClientRect(event.m_ClientRect)
	{

	}

	WindowEvent::WindowEvent(const Win32WndProcInfo& wndProcInfo)
		: Event(wndProcInfo)
	{
		InitVars();
	}

	WindowEvent::WindowEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		: Event(hwnd, msg, wparam, lparam)
	{
		InitVars();
	}

	std::string WindowEvent::ToString() const
	{
		Vector2i wndSize = GetWindowSize();
		Vector2i wndPos = GetWindowPos();
		Vector2i clientSize = GetClientWindowSize();
		Vector2i clientPos = GetClientWindowPos();
		return std::format(" > WindowSize[{},{}] > WindowPos[{},{}] > ClientSize[{},{}] > ClientPos[{},{}]", 
			wndSize.x, wndSize.y, wndPos.x, wndPos.y,
			clientSize.x, clientSize.y, clientPos.x, clientPos.y);
	}

	void WindowEvent::InitVars()
	{
		::GetWindowRect(m_WndProcInfo.hwnd, &m_WindowRect);
		::GetClientRect(m_WndProcInfo.hwnd, &m_ClientRect);
	}

}