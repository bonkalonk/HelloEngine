#pragma once

#include <windows.h>

struct WindowsWindow
{
	static HWND& Get()
	{
		assert( _instance._windowHandle != nullptr );

		return _instance._windowHandle;
	}

	static void Assign( HINSTANCE hInstance, int nCmdShow );

protected:
	WindowsWindow() = default;
	WindowsWindow( HINSTANCE hInstance, int nCmdShow );

	static WindowsWindow _instance;
	HWND _windowHandle = nullptr;
};
