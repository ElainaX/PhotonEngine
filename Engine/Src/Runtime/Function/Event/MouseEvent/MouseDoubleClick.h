#pragma once

#include "MouseEvent.h"

#include <cassert>

namespace photon
{
	// Double Click事件必须确保窗口有CS_DBLCLKS样式
	class MouseDoubleClickEvent : public MouseEvent
	{
	public:
		MouseDoubleClickEvent() = delete;
		MouseDoubleClickEvent(const MouseDoubleClickEvent& event)
			: MouseEvent(event), 
			m_bIsLButtonDblClk(event.m_bIsLButtonDblClk),
			m_bIsRButtonDblClk(event.m_bIsRButtonDblClk),
			m_bIsMButtonDblClk(event.m_bIsMButtonDblClk)
		{

		}
		MouseDoubleClickEvent(const Win32WndProcInfo& wndProcInfo)
			: MouseEvent(wndProcInfo)
		{
			InitVars();
		}
		MouseDoubleClickEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: MouseEvent(hwnd, msg, wparam, lparam)
		{
			InitVars();
		}

		constexpr virtual EventType GetType() const final
		{
			return EventType::MouseDoubleClick;
		}
		virtual std::string ToString() const final
		{
			return "[Mouse DoubleClick Event] " + MouseEvent::ToString();
		}
	public:
		bool IsLButtonDoubleClick() const { return m_bIsLButtonDblClk; }
		bool IsRButtonDoubleClick() const { return m_bIsRButtonDblClk; }
		bool IsMButtonDoubleClick() const { return m_bIsMButtonDblClk; }


	private:
		virtual void InitVars() final
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_LBUTTONDBLCLK ||
				m_WndProcInfo.msg == WM_RBUTTONDBLCLK || 
				m_WndProcInfo.msg == WM_MBUTTONDBLCLK, "MouseDoubleClickEvent类型和传入的msg矛盾！");
		}

	private:
		bool m_bIsLButtonDblClk = false;
		bool m_bIsRButtonDblClk = false;
		bool m_bIsMButtonDblClk = false;
	};

}