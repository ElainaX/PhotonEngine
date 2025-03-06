#pragma once

#include "MouseEvent.h"


namespace photon
{
	class MouseWheelEvent : public MouseEvent
	{
	public:
		MouseWheelEvent() = delete;
		MouseWheelEvent(const MouseWheelEvent& event)
			: MouseEvent(event), m_WheelDelta(event.m_WheelDelta)
		{
		}
		MouseWheelEvent(const Win32WndProcInfo& wndProcInfo)
			: MouseEvent(wndProcInfo)
		{
			InitVars();
		}
		MouseWheelEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: MouseEvent(hwnd, msg, wparam, lparam)
		{
			InitVars();
		}

		constexpr virtual EventType GetType() const final
		{
			return EventType::MouseWheel;
		}
		virtual std::string ToString() const final
		{
			return "[Mouse Wheel Event, Wheel_delta: " + std::to_string(m_WheelDelta) + "] " + MouseEvent::ToString();
		}

	public:
		int GetWheelDelta() const { return m_WheelDelta; }

	private:
		virtual void InitVars() final
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_MOUSEWHEEL, "MouseWheelEvent类型和传入的msg矛盾！");

			m_WheelDelta = GET_WHEEL_DELTA_WPARAM(m_WndProcInfo.wparam);
		}

	private:
		int m_WheelDelta = 0;

	};

}