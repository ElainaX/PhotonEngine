#pragma once
#include "WindowEvent.h"

#include <optional>

namespace photon
{
	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent() = delete;
		WindowResizeEvent(const WindowResizeEvent& event)
			: WindowEvent(event), m_bResizedOver(event.m_bResizedOver)
		{
		}
		WindowResizeEvent(const Win32WndProcInfo& wndProcInfo)
			: WindowEvent(wndProcInfo) 
		{
			InitVars();
		}
		WindowResizeEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: WindowEvent(hwnd, msg, wparam, lparam) 
		{
			InitVars();
		}

		virtual EventType GetType() const final
		{
			return EventType::WindowResize;
		}


		virtual std::string ToString() const final
		{
			if (IsResizedOver())
				return "[Window Moved Or Resized Over] " + WindowEvent::ToString();
			else
				return "[Window Is Resizing] " + WindowEvent::ToString();
		}

		bool IsResizedOver() const { return m_bResizedOver; }
		std::optional<bool> TryGetMinimumState() const 
		{
			if (m_WndProcInfo.msg == WM_SIZE &&
				(m_WndProcInfo.wparam & SIZE_MINIMIZED))
				return std::optional<bool>(true);

			return std::nullopt;
		}

	private:
		void InitVars() override
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_SIZE || 
				m_WndProcInfo.msg == WM_EXITSIZEMOVE, "WindowResizeEvent类型和传入的msg矛盾！");
			m_bResizedOver = (m_WndProcInfo.msg == WM_EXITSIZEMOVE);
		}

	private:
		bool m_bResizedOver = false;

	};
}