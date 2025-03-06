#include "KeyEvent.h"

#include <format>

namespace photon
{
	class KeyUpEvent : public KeyEvent
	{
	public:
		KeyUpEvent() = delete;
		KeyUpEvent(const KeyUpEvent& event)
			: KeyEvent(event)
		{
		}
		KeyUpEvent(const Win32WndProcInfo& wndProcInfo)
			: KeyEvent(wndProcInfo)
		{
			InitVars();
		}

		KeyUpEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			: KeyEvent(hwnd, msg, wparam, lparam)
		{
			InitVars();
		}

		EventType GetType() const final
		{
			return EventType::KeyUp;
		}


		std::string ToString() const final
		{
			return std::format("[Key Up]") + KeyEvent::ToString();
		}

	private:
		void InitVars() override
		{
			PHOTON_ASSERT(m_WndProcInfo.msg == WM_KEYUP || m_WndProcInfo.msg == WM_SYSKEYUP, "KeyUpEvent类型和传入的msg矛盾！");
		}

	};
}