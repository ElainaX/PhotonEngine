#pragma once

#include "../Event.h"
#include "Core/Math/Vector2i.h"

#include <windowsx.h>


namespace photon 
{
	class MouseEvent : public Event
	{
	public:
		MouseEvent() = delete;
		MouseEvent(const MouseEvent& event);
		MouseEvent(const Win32WndProcInfo& wndProcInfo);
		MouseEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		constexpr EventCategory GetCategory() const final;
		constexpr virtual EventType GetType() const override = 0;


		virtual std::string ToString() const override;

	public:
		bool IsLButtonPressed() const;
		bool IsRButtonPressed() const;
		bool IsMButtonPressed() const;
		bool IsShiftPressed() const;
		bool IsControlPressed() const;

		Vector2i GetMousePos() const;

	protected:
		virtual void InitVars();

	protected:
		bool m_bLButtonPressed = false;
		bool m_bRButtonPressed = false;
		bool m_bMButtonPressed = false;
		bool m_bShiftPressed = false;
		bool m_bControlPressed = false;

		Vector2i m_MousePos = { 0, 0 };
	};
	
}