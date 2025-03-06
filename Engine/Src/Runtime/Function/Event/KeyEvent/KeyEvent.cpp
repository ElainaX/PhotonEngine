#include "KeyEvent.h"

#include <format>

namespace photon 
{
	
	KeyEvent::KeyEvent(const KeyEvent& event)
		: Event(event), m_Key(event.m_Key)
	{

	}

	KeyEvent::KeyEvent(const Win32WndProcInfo& wndProcInfo)
		: Event(wndProcInfo)
	{
		InitVars();
	}

	KeyEvent::KeyEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		: Event(hwnd, msg, wparam, lparam)
	{
		InitVars();
	}

	std::string KeyEvent::ToString() const
	{
		return std::format(" > VirtualKey: {:#X}", (int)m_Key);
	}

	void KeyEvent::InitVars()
	{
		m_Key = static_cast<VirtualKey>(m_WndProcInfo.wparam);
	}

}