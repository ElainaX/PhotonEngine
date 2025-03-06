#include "KeyEvent.h"

#include <format>

namespace photon 
{
	class KeyDownEvent : public KeyEvent
	{
	public:
		KeyDownEvent() = delete;
		KeyDownEvent(const KeyDownEvent& event)
			: KeyEvent(event), 
			m_RepeatCount(event.m_RepeatCount), 
			m_bIsPreviousPressed(event.m_bIsPreviousPressed)
		{
		}
		KeyDownEvent(const Win32WndProcInfo& wndProcInfo)
			: KeyEvent(wndProcInfo)
		{
			InitVars();
		}

		KeyDownEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: KeyEvent(hwnd, msg, wparam, lparam)
		{
			InitVars();
		}

		EventType GetType() const final
		{
			return EventType::KeyDown;
		}


		std::string ToString() const final
		{
			return std::format("[Key Down] Previous Pressed: {} > Repeat {} Times", m_bIsPreviousPressed, m_RepeatCount) + KeyEvent::ToString();
		}

		int GetRepeatCount() const { return m_RepeatCount; }
		bool IsRepeat() const { return m_RepeatCount >= 2; }
		bool IsPreviousPressed() const { return m_bIsPreviousPressed; }

	private:
		void InitVars() override
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_KEYDOWN || m_WndProcInfo.msg == WM_SYSKEYDOWN, "KeyDownEvent类型和传入的msg矛盾！");
			m_RepeatCount = static_cast<int>(m_WndProcInfo.lparam & 0xFFFF);
			m_bIsPreviousPressed = static_cast<bool>((m_WndProcInfo.lparam << 30) & 0x1);
		}

	private:
		int m_RepeatCount = 0;
		bool m_bIsPreviousPressed = false;
	};
}