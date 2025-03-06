#pragma once
#include "WindowEvent.h"

#include <optional>

namespace photon
{
	class WindowMoveEvent : public WindowEvent
	{
	public:
		WindowMoveEvent() = delete;
		WindowMoveEvent(const WindowMoveEvent& event)
			: WindowEvent(event), m_bMovedOver(event.m_bMovedOver)
		{
		}
		WindowMoveEvent(const Win32WndProcInfo& wndProcInfo)
			: WindowEvent(wndProcInfo) 
		{
			InitVars();
		}
		WindowMoveEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: WindowEvent(hwnd, msg, wparam, lparam) 
		{
			InitVars();
		}

		virtual EventType GetType() const final
		{
			return EventType::WindowMove;
		}


		virtual std::string ToString() const final
		{
			if(IsMovedOver())
				return "[Window Moved Or Resized Over] " + WindowEvent::ToString();
			else 
				return "[Window Is Moving] " + WindowEvent::ToString();
		}

		bool IsMovedOver() const { return !m_bMovedOver; }

	private:
		void InitVars() override
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_MOVE ||
				m_WndProcInfo.msg == WM_EXITSIZEMOVE, "WindowMoveEvent类型和传入的msg矛盾！");
			m_bMovedOver = m_WndProcInfo.msg == WM_MOVE;
		}

	private:
		bool m_bMovedOver = false;
	};
}