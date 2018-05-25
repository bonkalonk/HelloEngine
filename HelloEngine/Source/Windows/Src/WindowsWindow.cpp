#include "../../Core/HighestHeader.h"

#include "WindowsWindow.h"
#include "d3d11.h"

WindowsWindow WindowsWindow::_instance;

int CALLBACK WinMain( _In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	WindowsWindow::Assign( hInstance, nCmdShow );

	HWND& windowHandle = WindowsWindow::Get();

	ShowWindow( windowHandle, nCmdShow );
	UpdateWindow( windowHandle );

	constexpr UINT messageFilterMin = 0;
	constexpr UINT messageFilterMax = 0;

	MSG recievedMessage;
	while ( GetMessage( &recievedMessage, NULL, messageFilterMin, messageFilterMax ) )
	{
		TranslateMessage( &recievedMessage );
		DispatchMessage( &recievedMessage );
	};

	return (int)recievedMessage.wParam;
}

LRESULT CALLBACK WndProc( _In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam )
{
	// Do this properly

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

void WindowsWindow::Assign( HINSTANCE hInstance, int nCmdShow )
{
	assert( _instance._windowHandle == nullptr );

	_instance = WindowsWindow( hInstance, nCmdShow );
}

WindowsWindow::WindowsWindow( HINSTANCE hInstance, int nCmdShow )
{
	WNDCLASSEX windowClass;

	static TCHAR windowClassName[] = "HelloEngineWindow";
	static TCHAR windowTitle[] = "Hello Engine";

	windowClass.cbSize = sizeof( WNDCLASSEX );
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	windowClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );

	if ( !RegisterClassEx( &windowClass ) )
	{
		MessageBox( NULL, "Error Registering WindowsWindow ClassEx, aborting...", windowTitle, NULL);

		return;
	}

	constexpr int defaultWindowSizeX = 500;
	constexpr int defaultWindowSizeY = 100;

	//using windowParent = NULL;
	//using windowMenuBar = NULL;
	//using lastParam = NULL;

	_windowHandle = CreateWindow(
		windowClassName,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		defaultWindowSizeX, defaultWindowSizeY,
		NULL,		// windowParent
		NULL,		// windowMenuBar
		hInstance,
		NULL );		// lastParam

	if ( _windowHandle == nullptr )
	{
		MessageBox( NULL, "Error Creating WindowsWindow, aborting...", windowTitle, NULL );

		return;
	}

	D3d11Interface temporaryINT;
}