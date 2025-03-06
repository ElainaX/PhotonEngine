#pragma once

#include "../Event.h"
#include "Core/Math/Vector2i.h"

namespace photon
{
	class WindowEvent : public Event
	{
	public:
		WindowEvent() = delete;
		WindowEvent(const WindowEvent& event);
		WindowEvent(const Win32WndProcInfo& wndProcInfo);
		WindowEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		EventCategory GetCategory() const override final { return EventCategory::Window; }


		virtual std::string ToString() const override;
		virtual EventType GetType() const override = 0;


		Vector2i GetWindowSize() const { return Vector2i(m_WindowRect.right - m_WindowRect.left, m_WindowRect.bottom-m_WindowRect.top); }
		Vector2i GetWindowPos() const { return Vector2i(m_WindowRect.left, m_WindowRect.top); }
		Vector2i GetClientWindowSize() const { return Vector2i(m_ClientRect.right - m_ClientRect.left, m_ClientRect.bottom - m_ClientRect.top); }
		Vector2i GetClientWindowPos() const { return Vector2i(m_ClientRect.left, m_ClientRect.top); }


	protected:
		virtual void InitVars();
		

	protected:
		RECT m_ClientRect = { 0, 0, 0, 0 };
		RECT m_WindowRect = { 0, 0, 0, 0 };
	};
}