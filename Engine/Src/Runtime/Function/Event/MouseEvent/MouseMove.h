#pragma once

#include "MouseEvent.h"

#include <cassert>

namespace photon
{
	class MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent() = delete;
		MouseMoveEvent(const MouseMoveEvent& event)
			: MouseEvent(event)
		{
		}
		MouseMoveEvent(const Win32WndProcInfo& wndProcInfo)
			: MouseEvent(wndProcInfo)
		{
			InitVars();
		}
		MouseMoveEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: MouseEvent(hwnd, msg, wparam, lparam)
		{
			InitVars();
		}

		constexpr virtual EventType GetType() const final
		{
			return EventType::MouseMove;
		}
		virtual std::string ToString() const final
		{
			return "[Mouse Move Event] " + MouseEvent::ToString();
		}

	private:
		virtual void InitVars() final
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_MOUSEMOVE, "MouseMoveEvent类型和传入的msg矛盾！");
		}

	};

}