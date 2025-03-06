#include "Event.h"

namespace photon 
{

	Event::Event(const Event& event)
		: m_bFinished(event.m_bFinished), m_WndProcInfo{ event.m_WndProcInfo }
	{
		
	}

	Event::Event(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		: m_bFinished(false), m_WndProcInfo{ hwnd, msg, wparam, lparam }
	{

	}

	Event::Event(const Win32WndProcInfo& wndProcInfo)
		: m_bFinished(false), m_WndProcInfo{ wndProcInfo }
	{

	}

}


