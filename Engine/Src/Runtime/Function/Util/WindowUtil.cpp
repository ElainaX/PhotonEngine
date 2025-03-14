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
		return ClientToScreen(hwnd, p.x, p.y);
	}


	photon::Vector2i WindowUtil::ClientToWindow(HWND hwnd, Vector2i p)
	{
		POINT point = {p.x, p.y};
		RECT wndRect;
		::ClientToScreen(hwnd, &point);
		::GetWindowRect(hwnd, &wndRect);
		return Vector2i{point.x - wndRect.left, point.y - wndRect.top};
	}

	photon::Vector2i WindowUtil::ClientToWindow(HWND hwnd, int x, int y)
	{
		return ClientToWindow(hwnd, Vector2i{ x, y });
	}

	photon::Vector2i WindowUtil::WindowToClient(HWND hwnd, Vector2i p)
	{
		POINT point{p.x, p.y};
		RECT wndRect;
		::GetWindowRect(hwnd, &wndRect);
		point.x += wndRect.left;
		point.y += wndRect.top;
		::ScreenToClient(hwnd, &point);
		return Vector2i{point.x, point.y};
	}


	photon::Vector2i WindowUtil::WindowToClient(HWND hwnd, int x, int y)
	{
		return WindowToClient(hwnd, Vector2i{ x, y });
	}



}