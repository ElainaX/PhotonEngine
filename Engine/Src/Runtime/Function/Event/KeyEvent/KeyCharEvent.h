#include "KeyEvent.h"

#include <format>

namespace photon
{
	class KeyCharEvent : public KeyEvent
	{
	public:
		KeyCharEvent() = delete;
		KeyCharEvent(const KeyCharEvent& event)
			: KeyEvent(event),
			m_RepeatCount(event.m_RepeatCount),
			m_bIsAltPressed(event.m_bIsAltPressed),
			m_bIsPreviousPressed(event.m_bIsPreviousPressed),
			m_bIsCharDown(event.m_bIsCharDown),
			m_Char(event.m_Char)
		{
		}
		KeyCharEvent(const Win32WndProcInfo& wndProcInfo)
			: KeyEvent(wndProcInfo)
		{
			InitVars();
		}

		KeyCharEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: KeyEvent(hwnd, msg, wparam, lparam)
		{
			InitVars();
		}

		EventType GetType() const final
		{
			return EventType::KeyChar;
		}


		std::string ToString() const final
		{

			if (IsCharDown())
			{
				return std::format("[Char \'{}\' Down] > RepeatTime: {} > ALtPressed: {} > Previous Pressed: {}",
					m_Char, m_RepeatCount, m_bIsAltPressed, m_bIsPreviousPressed) + KeyEvent::ToString();
			}

			return std::format("[Char \'{}\' Down] > ALtPressed: {}",
				m_Char, m_bIsAltPressed) + KeyEvent::ToString();
		}

		int GetRepeatCount() const { return m_RepeatCount; }
		bool IsRepeat() const { return m_RepeatCount >= 2; }
		bool IsAltPressed() const { return m_bIsAltPressed; }
		bool IsPreviousPressed() const { return m_bIsPreviousPressed; }
		bool IsCharDown() const { return m_bIsCharDown; }
		bool IsCharUp() const { return !m_bIsCharDown; }

		char GetChar() const { return m_Char; }


	private:
		void InitVars() override
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_CHAR, "KeyCharEvent类型和传入的msg矛盾！");
			m_RepeatCount = static_cast<int>(m_WndProcInfo.lparam & 0xFFFF);
			m_bIsAltPressed = static_cast<bool>((m_WndProcInfo.lparam << 29) & 0x1);
			m_bIsPreviousPressed = static_cast<bool>((m_WndProcInfo.lparam << 30) & 0x1);
			m_bIsCharDown = !static_cast<bool>((m_WndProcInfo.lparam << 31) & 0x1);
			m_TChar = static_cast<TCHAR>(m_WndProcInfo.wparam);
			WideCharToMultiByte(CP_ACP, 0, &m_TChar, 1, &m_Char, 1, NULL, NULL);
			m_Key = WChar2VirtualKey(m_Char);
		}


		VirtualKey WChar2VirtualKey(char c) const
		{
			SHORT result = VkKeyScan(c);
			return static_cast<VirtualKey>(LOBYTE(result));
		}

	private:
		int m_RepeatCount = 0;
		bool m_bIsAltPressed = false;
		bool m_bIsPreviousPressed = false;
		bool m_bIsCharDown = false;

		char m_Char = '0';
		TCHAR m_TChar = L'\0';
	};
}