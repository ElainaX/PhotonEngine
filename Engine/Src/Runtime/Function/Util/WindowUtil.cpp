#include "WindowUtil.h"

namespace photon 
{

	

	photon::Vector2i WindowUtil::ScreenToClient(HWND hwnd, int x, int y)
	{
		POINT point = { x, y };
		::ScreenToClient(hwnd, &point);
		return Vector2i{ point.x, point.y };
	}

	photon::Vector2i WindowUtil::ScreenToClient(HWND hwnd, Vector2i p)
	{
		return ScreenToClient(hwnd, p.x, p.y);
	}

	photon::Vector2i WindowUtil::ClientToScreen(HWND hwnd, int x, int y)
	{
		POINT point = { x, y };
		::ClientToScreen(hwnd, &point);
		return Vector2i{ point.x, point.y };
	}

	photon::Vector2i WindowUtil::ClientToScreen(HWND hwnd, Vector2i p)
	{
		return ClientToScreen(hwnd, p);
	}


	photon::Vector2i WindowUtil::ClientToWindow(HWND hwnd, Vector2i p)
	{
		RECT clientRect;
		RECT wndRect;
		::GetClientRect(hwnd, &clientRect);
		::GetWindowRect(hwnd, &wndRect);
		Vector2i offset = { clientRect.left - wndRect.left,
			clientRect.top - wndRect.top };
		return offset + p;
	}

	photon::Vector2i WindowUtil::ClientToWindow(HWND hwnd, int x, int y)
	{
		return ClientToWindow(hwnd, Vector2i{ x, y });
	}

	photon::Vector2i WindowUtil::WindowToClient(HWND hwnd, Vector2i p)
	{
		RECT clientRect;
		RECT wndRect;
		::GetClientRect(hwnd, &clientRect);
		::GetWindowRect(hwnd, &wndRect);
		Vector2i offset = { clientRect.left - wndRect.left,
			clientRect.top - wndRect.top };
		return p - offset;
	}


	photon::Vector2i WindowUtil::WindowToClient(HWND hwnd, int x, int y)
	{
		return WindowToClient(hwnd, Vector2i{ x, y });
	}



}