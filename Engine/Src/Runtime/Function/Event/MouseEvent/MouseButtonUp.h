#pragma once

#include "MouseEvent.h"

#include <cassert>

namespace photon
{
	class MouseButtonUpEvent : public MouseEvent
	{
	public:
		MouseButtonUpEvent() = delete;
		MouseButtonUpEvent(const MouseButtonUpEvent& event)
			: MouseEvent(event),
			m_bIsLButtonUp(event.m_bIsLButtonUp),
			m_bIsRButtonUp(event.m_bIsRButtonUp),
			m_bIsMButtonUp(event.m_bIsMButtonUp)
		{
		}
		MouseButtonUpEvent(const Win32WndProcInfo& wndProcInfo)
			: MouseEvent(wndProcInfo)
		{
			InitVars();
		}
		MouseButtonUpEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: MouseEvent(hwnd, msg, wparam, lparam)
		{
			InitVars();
		}

		constexpr virtual EventType GetType() const final
		{
			return EventType::MouseButtonUp;
		}
		virtual std::string ToString() const final
		{
			std::string whichUp = "";

			if (IsLButtonUp())
				whichUp = "Left Button";
			else if (IsRButtonUp())
				whichUp = "Right Button";
			else
				whichUp = "Middle Button";

			return "[" + whichUp + " Up] " + MouseEvent::ToString();

		}

	public:
		bool IsLButtonUp() const { return m_bIsLButtonUp; }
		bool IsRButtonUp() const { return m_bIsRButtonUp; }
		bool IsMButtonUp() const { return m_bIsMButtonUp; }

	private:
		virtual void InitVars() final
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_LBUTTONUP ||
				m_WndProcInfo.msg == WM_RBUTTONUP ||
				m_WndProcInfo.msg == WM_MBUTTONUP, "MouseUpEvent类型和传入的msg矛盾！");
			m_bIsLButtonUp = m_WndProcInfo.msg == WM_LBUTTONUP;
			m_bIsRButtonUp = m_WndProcInfo.msg == WM_RBUTTONUP;
			m_bIsMButtonUp = m_WndProcInfo.msg == WM_MBUTTONUP;
		}

	private:
		bool m_bIsLButtonUp = false;
		bool m_bIsRButtonUp = false;
		bool m_bIsMButtonUp = false;
	};

}