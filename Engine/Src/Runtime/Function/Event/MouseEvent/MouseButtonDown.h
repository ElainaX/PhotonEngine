#pragma once

#include "MouseEvent.h"

#include <cassert>

namespace photon
{
	class MouseButtonDownEvent : public MouseEvent
	{
	public:
		MouseButtonDownEvent() = delete;
		MouseButtonDownEvent(const MouseButtonDownEvent& event)
			: MouseEvent(event),
			m_bIsLButtonDown(event.m_bIsLButtonDown),
			m_bIsRButtonDown(event.m_bIsRButtonDown),
			m_bIsMButtonDown(event.m_bIsMButtonDown)
		{
		}
		MouseButtonDownEvent(const Win32WndProcInfo& wndProcInfo)
			: MouseEvent(wndProcInfo)
		{
			InitVars();
		}
		MouseButtonDownEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
			std::string whichDown = "";

			if (IsLButtonUp())
				whichDown = "Left Button";
			else if (IsRButtonUp())
				whichDown = "Right Button";
			else
				whichDown = "Middle Button";

			return "[" + whichDown + " Down] " + MouseEvent::ToString();

		}

	public:
		bool IsLButtonUp() const { return m_bIsLButtonDown; }
		bool IsRButtonUp() const { return m_bIsRButtonDown; }
		bool IsMButtonUp() const { return m_bIsMButtonDown; }

	private:
		virtual void InitVars() final
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_LBUTTONDOWN ||
				m_WndProcInfo.msg == WM_RBUTTONDOWN ||
				m_WndProcInfo.msg == WM_MBUTTONDOWN, "MouseDownEvent类型和传入的msg矛盾！");
			m_bIsLButtonDown = m_WndProcInfo.msg == WM_LBUTTONDOWN;
			m_bIsRButtonDown = m_WndProcInfo.msg == WM_RBUTTONDOWN;
			m_bIsMButtonDown = m_WndProcInfo.msg == WM_MBUTTONDOWN;
		}

	private:
		bool m_bIsLButtonDown = false;
		bool m_bIsRButtonDown = false;
		bool m_bIsMButtonDown = false;
	};

}