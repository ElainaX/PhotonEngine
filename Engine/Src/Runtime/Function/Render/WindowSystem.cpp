#include "WindowSystem.h"
#include "Function/Global/RuntimeGlobalContext.h"
#include "Macro.h"

#include <windowsx.h>

namespace photon 
{

	WindowSystem::~WindowSystem()
	{
		CloseAllWindows();
	}

	void WindowSystem::Initialize(WindowCreateInfo createInfo)
	{
		// 创建Window
		m_Width = createInfo.width;
		m_Height = createInfo.height;
		m_Title = std::wstring(createInfo.title);
		m_WndClass = { sizeof(WNDCLASSEXW) };
		// 窗口类风格设置
		m_WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		// 指定窗口过程函数
		m_WndClass.lpfnWndProc = WndProc;
		// 额外的类信息，这里设为 0
		m_WndClass.cbClsExtra = 0;
		// 额外的窗口信息，这里设为 0
		m_WndClass.cbWndExtra = 0;
		// 应用程序实例句柄
		m_WndClass.hInstance = nullptr;
		// 加载默认图标
		m_WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		// 加载默认光标
		m_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		// 设置窗口背景画刷为白色
		m_WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		// 菜单名，这里设为 NULL 表示没有菜单
		m_WndClass.lpszMenuName = NULL;
		// 窗口类名
		m_WndClass.lpszClassName = L"PhotonMainWndClass";
		// 加载默认小图标
		m_WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		// 注册窗口类
		if (!RegisterClassExW(&m_WndClass)) {
			MessageBoxW(NULL, L"Window class registration failed!", L"Error", MB_OK | MB_ICONERROR);
			return;
		}

		// 创建窗口
		m_WndHandle = CreateWindowExW(
			0,
			L"PhotonMainWndClass",
			m_Title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			m_Width, m_Height,
			NULL,
			NULL,
			nullptr,
			NULL
		);
		if(!m_WndHandle)
		{
			MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_OK | MB_ICONERROR);
			return;
		}

		// Show the window
		::ShowWindow(m_WndHandle, SW_SHOWDEFAULT);
		::UpdateWindow(m_WndHandle);
	}

	void WindowSystem::PollEvents() const
	{
		MSG msg{ 0 };
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	bool WindowSystem::ShouldClose() const
	{
		return m_bShouldClose;
	}

	void WindowSystem::SetShouldClose(bool bShouldClose /*= true*/)
	{
		m_bShouldClose = bShouldClose;
	}

	void WindowSystem::SetTitle(const std::wstring& title)
	{
		m_Title = title;
		::SetWindowTextW(m_WndHandle, m_Title.c_str());
	}

	void WindowSystem::SetFocusMode(bool bShouldFocus /*= true*/)
	{
		if(m_bFocusMode != bShouldFocus)
		{
			if(bShouldFocus)
			{
				m_BeforeFocusMousePos = WindowUtil::GetScreenMousePos();
				auto center = WindowUtil::SetMousePosToWndCenter(m_WndHandle);
				Win32WndProcInfo procInfo;
				procInfo.hwnd = m_WndHandle;
				procInfo.msg = WM_MOUSEMOVE;
				procInfo.wparam = 0;
				procInfo.lparam = MAKELPARAM(center.x, center.y);
				MouseMoveEvent e(procInfo);
				OnMouseMove(e);
			}
			else 
			{
				WindowUtil::SetScreenMousePos(m_BeforeFocusMousePos);
				Win32WndProcInfo procInfo;
				procInfo.hwnd = m_WndHandle;
				procInfo.msg = WM_MOUSEMOVE;
				procInfo.wparam = 0;
				procInfo.lparam = MAKELPARAM(m_BeforeFocusMousePos.x, m_BeforeFocusMousePos.y);
				MouseMoveEvent e(procInfo);
				OnMouseMove(e);
			}
			m_bFocusMode = bShouldFocus;
		}
	}

	void WindowSystem::SetViewportSize(Vector2i sz)
	{
		m_ViewportSize = sz;
	}

	void WindowSystem::CloseAllWindows()
	{
		::DestroyWindow(m_WndHandle);
	}

	LRESULT WINAPI WindowSystem::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		std::shared_ptr<WindowSystem> wndSystem = g_RuntimeGlobalContext.windowSystem;

		Win32WndProcInfo procInfo;
		procInfo.hwnd = hWnd;
		procInfo.msg = msg;
		procInfo.wparam = wParam;
		procInfo.lparam = lParam;

		bool shouldContinue = true;
		wndSystem->BeforeAllEvent(procInfo, shouldContinue);
		if (!shouldContinue)
			return true;

		if (wndSystem)
		{
			// Dispatch Msg
			switch (msg)
			{
			// Mouse Event
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			{
				MouseButtonDownEvent mouseButtonDownEvent(procInfo);
				wndSystem->OnMouseButtonDown(mouseButtonDownEvent);
				return 0;
			}

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			{
				MouseButtonUpEvent mouseButtonUpEvent(procInfo);
				wndSystem->OnMouseButtonUp(mouseButtonUpEvent);
				return 0;
			}

			case WM_MOUSEWHEEL:
			{
				MouseWheelEvent mouseWheelEvent(procInfo);
				wndSystem->OnMouseWheel(mouseWheelEvent);
				return 0;
			}

			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			{
				MouseDoubleClickEvent mouseDblClkEvent(procInfo);
				wndSystem->OnMouseDoubleClick(mouseDblClkEvent);
				return 0;
			}

			case WM_MOUSEMOVE:
			{
				MouseMoveEvent mouseMoveEvent(procInfo);
				wndSystem->OnMouseMove(mouseMoveEvent);
				return 0;
			}

			// Key Event
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				KeyDownEvent keyDownEvent(procInfo);
				wndSystem->OnKeyDown(keyDownEvent);
				return 0;
			}

			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				KeyUpEvent keyUpEvent(procInfo);
				wndSystem->OnKeyUp(keyUpEvent);
				return 0;
			}

			case WM_CHAR:
			{
				KeyCharEvent keyCharEvent(procInfo);
				wndSystem->OnKeyChar(keyCharEvent);
				return 0;
			}

			// Window Event
			case WM_SIZE:
			{
				WindowResizeEvent wndResizeEvent(procInfo);
				wndSystem->OnWindowResize(wndResizeEvent);
				return 0;
			}

			case WM_MOVE:
			{
				WindowMoveEvent wndMoveEvent(procInfo);
				wndSystem->OnWindowMove(wndMoveEvent);
				return 0;
			}


			case WM_EXITSIZEMOVE:
			{
				WindowMoveEvent wndMoveEvent(procInfo);
				WindowResizeEvent wndResizeEvent(procInfo);
				wndSystem->OnWindowResize(wndResizeEvent);
				wndSystem->OnWindowMove(wndMoveEvent);
				return 0;
			}

			case WM_CREATE:
			{
				WindowCreateEvent wndCreateEvent(procInfo);
				wndSystem->OnWindowCreate(wndCreateEvent);
				return 0;
			}

			case WM_DESTROY:
			{
				WindowDestroyEvent wndDestroyEvent(procInfo);
				wndSystem->OnWindowDestroy(wndDestroyEvent);
				return 0;
			}

			}
		}

		return ::DefWindowProc(procInfo.hwnd, procInfo.msg, procInfo.wparam, procInfo.lparam);

	}

	

	void WindowSystem::RegisterOnMouseButtonDownCallback(OnMouseButtonDownFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnMouseButtonDownFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnMouseButtonDownCallbacks.begin(),
			m_OnMouseButtonDownCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnMouseButtonDownCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnMouseButtonUpCallback(OnMouseButtonUpFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnMouseButtonUpFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnMouseButtonUpCallbacks.begin(),
			m_OnMouseButtonUpCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnMouseButtonUpCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnMouseWheelCallback(OnMouseWheelFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnMouseWheelFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnMouseWheelCallbacks.begin(),
			m_OnMouseWheelCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnMouseWheelCallbacks.insert(insertIter, currCallback);

	}

	void WindowSystem::RegisterOnMouseMoveCallback(OnMouseMoveFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnMouseMoveFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnMouseMoveCallbacks.begin(),
			m_OnMouseMoveCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnMouseMoveCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnMouseDoubleClickCallback(OnMouseDoubleClickFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnMouseDoubleClickFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnMouseDoubleClickCallbacks.begin(),
			m_OnMouseDoubleClickCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnMouseDoubleClickCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnWindowCreateCallback(OnWindowCreateFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnWindowCreateFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnWindowCreateCallbacks.begin(),
			m_OnWindowCreateCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnWindowCreateCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnWindowDestroyCallback(OnWindowDestroyFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnWindowDestroyFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnWindowDestroyCallbacks.begin(),
			m_OnWindowDestroyCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnWindowDestroyCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnWindowMoveCallback(OnWindowMoveFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnWindowMoveFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnWindowMoveCallbacks.begin(),
			m_OnWindowMoveCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnWindowMoveCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnWindowResizeCallback(OnWindowResizeFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnWindowResizeFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnWindowResizeCallbacks.begin(),
			m_OnWindowResizeCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnWindowResizeCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnKeyDownCallback(OnKeyDownFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnKeyDownFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnKeyDownCallbacks.begin(),
			m_OnKeyDownCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnKeyDownCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnKeyUpCallback(OnKeyUpFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnKeyUpFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnKeyUpCallbacks.begin(),
			m_OnKeyUpCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnKeyUpCallbacks.insert(insertIter, currCallback);
	}

	void WindowSystem::RegisterOnKeyCharCallback(OnKeyCharFunc func, int priorty /*= 0*/)
	{
		std::pair<int, OnKeyCharFunc> currCallback = std::make_pair(priorty, func);
		auto insertIter = std::lower_bound(m_OnKeyCharCallbacks.begin(),
			m_OnKeyCharCallbacks.end(), currCallback, PriorityPairCompare());
		m_OnKeyCharCallbacks.insert(insertIter, currCallback);
	}


	void WindowSystem::RegisterBeforeAllEventCallBack(BeforeAllEventFunc func)
	{
		m_BeforeAllEventCallbacks.push_back(func);
	}

	void WindowSystem::OnMouseButtonDown(const MouseButtonDownEvent& mouseButtonDown)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", mouseButtonDown.ToString());
#endif
		MouseButtonDownEvent e = mouseButtonDown;
		for(auto& callback : m_OnMouseButtonDownCallbacks)
		{
			if(!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnMouseButtonUp(const MouseButtonUpEvent& mouseButtonUp)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", mouseButtonUp.ToString());
#endif
		MouseButtonUpEvent e = mouseButtonUp;
		for (auto& callback : m_OnMouseButtonUpCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}


	void WindowSystem::OnMouseWheel(const MouseWheelEvent& mouseWheel)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", mouseWheel.ToString());
#endif
		MouseWheelEvent e = mouseWheel;
		for (auto& callback : m_OnMouseWheelCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnMouseMove(const MouseMoveEvent& mouseMove)
	{
//#ifdef DEBUG
//		LOG_INFO("{}", mouseMove.ToString());
//#endif
		MouseMoveEvent e = mouseMove;
		for (auto& callback : m_OnMouseMoveCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnMouseDoubleClick(const MouseDoubleClickEvent& mouseDoubleClick)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", mouseDoubleClick.ToString());
#endif
		MouseDoubleClickEvent e = mouseDoubleClick;
		for (auto& callback : m_OnMouseDoubleClickCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnWindowCreate(const WindowCreateEvent& wndCreate)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", wndCreate.ToString());
#endif
		WindowCreateEvent e = wndCreate;
		for (auto& callback : m_OnWindowCreateCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnWindowDestroy(const WindowDestroyEvent& wndDestroy)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", wndDestroy.ToString());
#endif
		WindowDestroyEvent e = wndDestroy;
		for (auto& callback : m_OnWindowDestroyCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}

		SetShouldClose();
	}

	void WindowSystem::OnWindowMove(const WindowMoveEvent& wndMove)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", wndMove.ToString());
#endif
		WindowMoveEvent e = wndMove;
		for (auto& callback : m_OnWindowMoveCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnWindowResize(const WindowResizeEvent& wndResize)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", wndResize.ToString());
#endif
		// 更新WindowSystem的参数
		Vector2i wndSize = wndResize.GetWindowSize();
		m_Width = wndSize.x;
		m_Height = wndSize.y;

		Vector2i wndClientSize = wndResize.GetClientWindowSize();
		m_ClientWidth = wndClientSize.x;
		m_ClientHeight = wndClientSize.y;

		WindowResizeEvent e = wndResize;
		for (auto& callback : m_OnWindowResizeCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnKeyDown(const KeyDownEvent& keydown)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", keydown.ToString());
#endif
		if (keydown.GetVirtualKey() == VirtualKey::esc)
			m_bShouldClose = true;

		KeyDownEvent e = keydown;
		for (auto& callback : m_OnKeyDownCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}


	}

	void WindowSystem::OnKeyUp(const KeyUpEvent& keyup)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", keyup.ToString());
#endif
		KeyUpEvent e = keyup;
		for (auto& callback : m_OnKeyUpCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::OnKeyChar(const KeyCharEvent& keychar)
	{
#ifdef DEBUG
		LOG_DEBUG("{}", keychar.ToString());
#endif
		KeyCharEvent e = keychar;
		for (auto& callback : m_OnKeyCharCallbacks)
		{
			if (!e.IsFinished())
			{
				callback.second(e);
			}
		}
	}

	void WindowSystem::BeforeAllEvent(Win32WndProcInfo& wndProcInfo, bool& bShouldContinue)
	{
		for(auto& callback : m_BeforeAllEventCallbacks)
		{
			if (bShouldContinue)
				callback(wndProcInfo, bShouldContinue);
			else
				break;
		}
	}

}