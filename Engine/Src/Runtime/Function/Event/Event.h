#pragma once
#include <windows.h>
#include <string>

#include "Macro.h"

namespace photon 
{
	// Win32下窗口回调函数WndProc的参数
	// LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	struct Win32WndProcInfo
	{
		HWND hwnd;
		UINT msg;
		WPARAM wparam;
		LPARAM lparam;
	};

	enum class EventType
	{
		None = 0,
		MouseButtonDown, MouseButtonUp, MouseDoubleClick, MouseMove, MouseWheel,
		KeyDown, KeyUp, KeyChar,
		WindowCreate, WindowDestory, WindowResize, WindowMove,
		// Many Many
	};

	
	enum class EventCategory
	{
		None = 0,
		Mouse,
		Key,
		Window
	};


	// 创建我们自己的事件系统
	class Event 
	{
	public:
		Event() = delete;
		Event(const Event& event);
		Event(const Win32WndProcInfo& wndProcInfo);
		Event(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		// 拷贝构造和拷贝赋值均使用trival版本
		
		Win32WndProcInfo GetWndProcInfo() const { return m_WndProcInfo; }

		// 事件是否结束，如果结束则不再传播
		bool IsFinished() const { return m_bFinished;}
		void SetFinished(bool bFinished = true) { m_bFinished = bFinished; }
		virtual std::string ToString() const { return "RootEvent: NoDetail!"; }

		virtual EventCategory GetCategory() const = 0;
		virtual EventType GetType() const = 0;



	protected:
		bool m_bFinished = false;
		Win32WndProcInfo m_WndProcInfo = {0, 0, 0, 0};
	};
}