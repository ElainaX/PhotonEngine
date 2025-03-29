#include <windows.h>
#include <windowsx.h>

#include "Core/Math/Vector2i.h"

namespace photon 
{
	class WindowUtil
	{
	public:
		static Vector2i ScreenToClient(HWND hwnd, int x, int y);
		static Vector2i ScreenToClient(HWND hwnd, Vector2i p);

		static Vector2i ClientToScreen(HWND hwnd, int x, int y);
		static Vector2i ClientToScreen(HWND hwnd, Vector2i p);

		static Vector2i ClientToWindow(HWND hwnd, int x, int y);
		static Vector2i ClientToWindow(HWND hwnd, Vector2i p);

		static Vector2i WindowToClient(HWND hwnd, int x, int y);
		static Vector2i WindowToClient(HWND hwnd, Vector2i p);

		static Vector2i GetScreenMousePos();
		static void SetScreenMousePos(Vector2i pos);
		static Vector2i SetMousePosToWndCenter(HWND wnd);
	};
}