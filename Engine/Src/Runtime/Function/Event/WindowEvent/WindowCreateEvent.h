#pragma once
#include "WindowEvent.h"

namespace photon
{
	class WindowCreateEvent : public WindowEvent
	{
	public:
		WindowCreateEvent() = delete;
		WindowCreateEvent(const WindowCreateEvent& event)
			: WindowEvent(event) {
		}
		WindowCreateEvent(const Win32WndProcInfo& wndProcInfo)
			: WindowEvent(wndProcInfo) {
		}
		WindowCreateEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: WindowEvent(hwnd, msg, wparam, lparam) {
		}

		virtual EventType GetType() const final
		{
			return EventType::WindowCreate;
		}


		virtual std::string ToString() const final
		{
			return "[WindowCreate] " + WindowEvent::ToString();
		}

	};
}