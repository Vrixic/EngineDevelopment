// HelloWindowsDesktop.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <conio.h>

#include "renderer.h"
#include "dev_app.h"

#include "InputManager.h"

#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_impl_dx11.h"

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Engine Development");

HINSTANCE hInst;

bool ShowMouseCursor = true;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void create_console();
void destroy_console(int exit_code);
MSG begin_main_loop();

namespace
{
	HWND  main_hwnd = NULL;

	end::dev_app_t dev_app{};
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;// CHANGED FROM (HBRUSH)(COLOR_WINDOW + 1) TO REMOVE FLICKER
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Engine Development"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	main_hwnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_POPUPWINDOW | WS_CAPTION, // CHANGED FROM WS_OVERLAPPEDWINDOW TO DISABLE SIZING
		CW_USEDEFAULT, CW_USEDEFAULT,
		1280, 720,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!main_hwnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Engine Development"),
			NULL);

		return 1;
	}

	create_console();

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(main_hwnd, nCmdShow);
	UpdateWindow(main_hwnd);

	MSG msg = begin_main_loop();

	destroy_console((int)msg.wParam);
	FreeConsole();

	return (int)msg.wParam;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		/* Keyboard */
	case WM_KEYDOWN:
	{
		InputManager::GetInstance().SetKeyState((uint8)wParam);
		break;
	}

	case WM_KEYUP:
	{
		InputManager::GetInstance().UnsetKeyState((uint8)wParam);
		break;
	}

	/* Mouse Input */
	case WM_LBUTTONDOWN:
	{
		InputManager::GetInstance().SetKeyState(MOUSE_LEFT_DOWN);
		break;
	}

	case WM_LBUTTONUP:
	{
		InputManager::GetInstance().UnsetKeyState(MOUSE_LEFT_DOWN);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		InputManager::GetInstance().SetKeyState(MOUSE_RIGHT_DOWN);
		break;
	}

	case WM_RBUTTONUP:
	{
		InputManager::GetInstance().UnsetKeyState(MOUSE_RIGHT_DOWN);
		break;
	}

	case WM_MOUSEMOVE:
	{
		POINT P = { GET_X_LPARAM(lParam) , GET_Y_LPARAM(lParam) };
		InputManager::GetInstance().SetMousePosition(MousePoint(P.x, P.y));
		break;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void create_console()
{
#ifndef NDEBUG
	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::cout << "Debug Console Opened.\n"; // Don’t forget to include <iostream>
#endif
}

void destroy_console(int exit_code)
{
#ifndef NDEBUG

	if (exit_code)
	{
		std::cout << "Exit Code: " << exit_code << "\n";
		std::cout << "Press any key to continue...";
		_getch();
	}

	FreeConsole();
#endif
}

void RenderImGui(void)
{
	dev_app.RenderImGui();
}

MSG begin_main_loop()
{
	MSG msg;

	/* Init ImGui*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO();
	ImGui_ImplWin32_Init(main_hwnd);

	/* Also imgui will be initialized for d3d11 */

	end::renderer_t renderer(main_hwnd);

	ImGui::StyleColorsDark();

	renderer.SetImGuiFuncPointer(RenderImGui);

	dev_app.SetViewMatrix(renderer.default_view.view_mat);
	dev_app.UpdateCameraPositionByView();
	dev_app.SetRenderer(&renderer);

	dev_app.CreateBVH();

	// Main application loop:
	while (true)
	{
		// Process all messages, stop on WM_QUIT
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// WM_QUIT does not need to be // translated or dispatched
			if (msg.message == WM_QUIT)
				break;
			// Translates messages and sends them to WndProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			dev_app.update();
			renderer.default_view.view_mat = dev_app.GetViewMatrix();
			renderer.draw();

			/* Reset mouse deltas every frame */
			InputManager::GetInstance().ResetMouseDelta();
		}
	}

	//Releases COM references that ImGui was given on setup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return msg;
}

