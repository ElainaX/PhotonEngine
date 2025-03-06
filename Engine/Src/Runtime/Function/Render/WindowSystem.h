#pragma once
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
// We Use Win32 As The Window System
#include <windows.h>

#include "Function/Event/EventHeaders.h"

namespace photon
{
	struct WindowCreateInfo
	{
		int				width{ 1280 };
		int				height{ 720 };
		const wchar_t*  title{ L"photon" };
		bool			is_fullscreen{ false };
	};

	class WindowSystem
	{
	public:
		struct PriorityPairCompare
		{
			template<typename Func>
			bool operator()(const std::pair<int, Func>& lhs, const std::pair<int, Func>& rhs)
			{
				return lhs.first > rhs.first;
			}
		};

		WindowSystem() = default;
		~WindowSystem();

		void Initialize(WindowCreateInfo createInfo);
		void PollEvents() const;
		bool ShouldClose() const;
		void SetShouldClose(bool bShouldClose = true);
		void SetTitle(const std::wstring& title);

		void CloseAllWindows();

		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		using OnMouseButtonDownFunc = std::function<void(MouseButtonDownEvent&)>;
		using OnMouseButtonUpFunc = std::function<void(MouseButtonUpEvent&)>;
		using OnMouseWheelFunc = std::function<void(MouseWheelEvent&)>;
		using OnMouseMoveFunc = std::function<void(MouseMoveEvent&)>;
		using OnMouseDoubleClickFunc = std::function<void(MouseDoubleClickEvent&)>;

		using OnWindowCreateFunc = std::function<void(WindowCreateEvent&)>;
		using OnWindowDestroyFunc = std::function<void(WindowDestroyEvent&)>;
		using OnWindowMoveFunc = std::function<void(WindowMoveEvent&)>;
		using OnWindowResizeFunc = std::function<void(WindowResizeEvent&)>;

		using OnKeyDownFunc = std::function<void(KeyDownEvent&)>;
		using OnKeyUpFunc = std::function<void(KeyUpEvent&)>;
		using OnKeyCharFunc = std::function<void(KeyCharEvent&)>;



		// Priority从 -5 ~ 5 逐级重要程度递增，默认0级
		void RegisterOnMouseButtonDownCallback(OnMouseButtonDownFunc func, int priorty = 0);
		void RegisterOnMouseButtonUpCallback(OnMouseButtonUpFunc func, int priorty = 0);
		void RegisterOnMouseWheelCallback(OnMouseWheelFunc func, int priorty = 0);
		void RegisterOnMouseMoveCallback(OnMouseMoveFunc func, int priorty = 0);
		void RegisterOnMouseDoubleClickCallback(OnMouseDoubleClickFunc func, int priorty = 0);

		void RegisterOnWindowCreateCallback(OnWindowCreateFunc func, int priorty = 0);
		void RegisterOnWindowDestroyCallback(OnWindowDestroyFunc func, int priorty = 0);
		void RegisterOnWindowMoveCallback(OnWindowMoveFunc func, int priorty = 0);
		void RegisterOnWindowResizeCallback(OnWindowResizeFunc func, int priorty = 0);

		void RegisterOnKeyDownCallback(OnKeyDownFunc func, int priorty = 0);
		void RegisterOnKeyUpCallback(OnKeyUpFunc func, int priorty = 0);
		void RegisterOnKeyCharCallback(OnKeyCharFunc func, int priorty = 0);


	private:
		void OnMouseButtonDown(const MouseButtonDownEvent& mouseButtonDown);
		void OnMouseButtonUp(const MouseButtonUpEvent& mouseButtonUp);
		void OnMouseWheel(const MouseWheelEvent& mouseWheel);
		void OnMouseMove(const MouseMoveEvent& mouseMove);
		void OnMouseDoubleClick(const MouseDoubleClickEvent& mouseDoubleClick);

		void OnWindowCreate(const WindowCreateEvent& wndCreate);
		void OnWindowDestroy(const WindowDestroyEvent& wndDestroy);
		void OnWindowMove(const WindowMoveEvent& wndMove);
		void OnWindowResize(const WindowResizeEvent& wndResize);

		void OnKeyDown(const KeyDownEvent& keydown);
		void OnKeyUp(const KeyUpEvent& keyup);
		void OnKeyChar(const KeyCharEvent& keychar);



	private:
		HWND m_WndHandle;
		WNDCLASSEXW m_WndClass;
		std::wstring m_Title;
		int m_Width;
		int m_Height;

		bool m_bShouldClose = false;

		std::vector<std::pair<int, OnMouseButtonDownFunc>> m_OnMouseButtonDownCallbacks;
		std::vector<std::pair<int, OnMouseButtonUpFunc>> m_OnMouseButtonUpCallbacks;
		std::vector<std::pair<int, OnMouseWheelFunc>> m_OnMouseWheelCallbacks;
		std::vector<std::pair<int, OnMouseMoveFunc>> m_OnMouseMoveCallbacks;
		std::vector<std::pair<int, OnMouseDoubleClickFunc>> m_OnMouseDoubleClickCallbacks;

		std::vector<std::pair<int, OnWindowCreateFunc>> m_OnWindowCreateCallbacks;
		std::vector<std::pair<int, OnWindowDestroyFunc>> m_OnWindowDestroyCallbacks;
		std::vector<std::pair<int, OnWindowMoveFunc>> m_OnWindowMoveCallbacks;
		std::vector<std::pair<int, OnWindowResizeFunc>> m_OnWindowResizeCallbacks;

		std::vector<std::pair<int, OnKeyDownFunc>> m_OnKeyDownCallbacks;
		std::vector<std::pair<int, OnKeyUpFunc>> m_OnKeyUpCallbacks;
		std::vector<std::pair<int, OnKeyCharFunc>> m_OnKeyCharCallbacks;

	};
}