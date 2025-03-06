#pragma once
#include "WindowEvent.h"

namespace photon 
{
	class WindowDestroyEvent : public WindowEvent
	{
	public:
		WindowDestroyEvent() = delete;
		WindowDestroyEvent(const WindowDestroyEvent& event)
			: WindowEvent(event){}
		WindowDestroyEvent(const Win32WndProcInfo& wndProcInfo)
			: WindowEvent(wndProcInfo){}
		WindowDestroyEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: WindowEvent(hwnd, msg, wparam, lparam){}

		virtual EventType GetType() const final
		{
			return EventType::WindowDestory;
		}


		virtual std::string ToString() const final
		{
			return "[WindowDestroy] " + WindowEvent::ToString();
		}

	};
}