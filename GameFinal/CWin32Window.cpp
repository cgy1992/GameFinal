#include "stdafx.h"
#include "CWin32Window.h"
#include "IApplication.h"
#include <functional> 

namespace gf
{
	static CWin32Window* g_pWin32Window = NULL;

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


	CWin32Window::CWin32Window(IApplication* app)
		: IWindow(app)
		, mDefinedWndProc(NULL)
		, m_hParentWnd(0)
		, m_hFrameWnd(0)
		, m_hBufferWnd(0)
	{

	}

	bool CWin32Window::init(SAppSettings& settings)
	{
		if (settings.Window.WindowsProcedure)
			mDefinedWndProc = (WNDPROC)(settings.Window.WindowsProcedure);

		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;

		// Get an external pointer to this object.	
		g_pWin32Window = this;

		// Get the instance of this application.
		m_hInstance = GetModuleHandle(NULL);

		if (settings.Window.FrameWindowHandle == 0) {
			// Give the application a name.
			static TCHAR className[] = TEXT("MyGame");

			// Setup the windows class with default settings.
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
			wc.lpfnWndProc = gf::WndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = sizeof(CWin32Window*);
			wc.hInstance = m_hInstance;
			wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
			wc.hIconSm = wc.hIcon;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			//wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
			HBRUSH bkBrush = CreateSolidBrush(RGB(240, 240, 240));

			wc.hbrBackground = bkBrush;
			wc.lpszMenuName = NULL;
			wc.lpszClassName = className;
			wc.cbSize = sizeof(WNDCLASSEX);

			// Register the window class.
			RegisterClassEx(&wc);

			// Determine the resolution of the clients desktop screen.
			if (settings.Window.Style & EWS_FULLRESOLUTION)
			{
				settings.Window.Width = GetSystemMetrics(SM_CXSCREEN);
				settings.Window.Height = GetSystemMetrics(SM_CYSCREEN);
				settings.Graphics.BackBufferWidth = settings.Window.Width;
				settings.Graphics.BackBufferHeight = settings.Window.Height;
				// m_CreationParams.BackBufferHeight = m_CreationParams.ClientHeight;
			}

			// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
			if (settings.Window.Style & EWS_FULLSCREEN)
			{
				// If full screen set the screen to maximum size of the users desktop and 32bit.
				memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
				dmScreenSettings.dmSize = sizeof(dmScreenSettings);
				dmScreenSettings.dmPelsWidth = (unsigned long)settings.Window.Width;
				dmScreenSettings.dmPelsHeight = (unsigned long)settings.Window.Height;
				dmScreenSettings.dmBitsPerPel = 32;
				dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				// Change the display settings to full screen.
				ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

				// Create the window with the screen settings and get the handle to it.
				m_hFrameWnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("Game Final App"),
					WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
					0, 0, settings.Window.Width,
					settings.Window.Height, NULL, NULL, m_hInstance, static_cast<LPVOID>(this));
			}
			else
			{
				int posX = (GetSystemMetrics(SM_CXSCREEN) - settings.Window.Width) / 2;
				int posY = (GetSystemMetrics(SM_CYSCREEN) - settings.Window.Height) / 2;

				RECT R = { 0, 0, settings.Window.Width, settings.Window.Height };
				AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

				// Create the window with the screen settings and get the handle to it.
				m_hFrameWnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("Game Final App"),
					WS_OVERLAPPEDWINDOW, posX, posY, R.right - R.left, R.bottom - R.top,
					NULL, NULL, m_hInstance, static_cast<LPVOID>(this));
			}

			// Bring the window up on the screen and set it as main focus.

			//mSettings.Window.ParentHandle = (u32)mHwnd;
			m_hBufferWnd = m_hFrameWnd;
			m_hParentWnd = m_hFrameWnd;
			ShowWindow(m_hFrameWnd, SW_SHOW);
			SetForegroundWindow(m_hFrameWnd);
			SetFocus(m_hFrameWnd);
		}
		else // if the user sets the parent hwnd
		{
			m_hParentWnd = (HWND)settings.Window.ParentHandle;
			m_hFrameWnd = (HWND)settings.Window.FrameWindowHandle;
			if (m_hParentWnd == 0) {
				m_hParentWnd = m_hFrameWnd;
			}
		}

		//m_CreationParams.FrameWindowHandle = (u32)mHwnd;
		//m_CreationParams.BackBufferWindowHandle = (u32)mHwnd;

		if (!(settings.Window.Style & EWS_FULLSCREEN))
		{
			HWND hBufferWnd = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_WHITERECT,
				0, 0, settings.Graphics.BackBufferWidth, settings.Graphics.BackBufferHeight,
				m_hParentWnd, (HMENU)9, m_hInstance, NULL);

			// m_CreationParams.BackBufferWindowHandle = (u32)hBufferWnd;
			m_hBufferWnd = hBufferWnd;
		}

		settings.Window.BackBufferWindowHandle = (u32)m_hBufferWnd;
		settings.Window.FrameWindowHandle = (u32)m_hFrameWnd;
		return true;
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Store instance pointer while handling the first message
		if (msg == WM_NCCREATE)
		{
			CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
			LPVOID pThis = pCS->lpCreateParams;
			SetWindowLongPtrW(hwnd, 0, reinterpret_cast<LONG_PTR>(pThis));
		}

		// At this point the instance pointer will always be available
		CWin32Window* pWnd = reinterpret_cast<CWin32Window*>(GetWindowLongPtrW(hwnd, 0));
		// see Note 1a below
		if (pWnd) {
			return pWnd->WndProc(hwnd, msg, wParam, lParam);
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK CWin32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (mDefinedWndProc)
		{
			mDefinedWndProc(hwnd, msg, wParam, lParam);
		}

		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_ACTIVATE:
			/*
			if (mInputDriver && mSettings.Input.Driver == EIDT_DIRECTINPUT8)
			{
				CDInput8Driver* input = dynamic_cast<CDInput8Driver*>(mInputDriver);
				POINT p;
				GetCursorPos(&p);
				input->setCurrentMousePos(p.x, p.y);
			}
			break;
			*/
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	}

	void CWin32Window::loop(std::function<bool()> stepfunc)
	{
		MSG msg;
		bool stopped = false;

		while (!stopped) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					stopped = true;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (stopped)
				break;

			if (!stepfunc())
				stopped = true;
		}
	}


	void CWin32Window::setTitle(const std::string& s)
	{
		SetWindowTextA(m_hFrameWnd, s.c_str());
	}

	void CWin32Window::clientToScreen(int& x, int& y) const
	{
		POINT point = { x, y };
		ClientToScreen(m_hBufferWnd, &point);
		x = point.x;
		y = point.y;
	}

	void CWin32Window::screenToClient(int& x, int& y) const
	{
		POINT point = { x, y };
		ScreenToClient(m_hBufferWnd, &point);
		x = point.x;
		y = point.y;
	}

	void CWin32Window::getMousePos(int& x, int& y) const
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(m_hBufferWnd, &p);
		x = p.x;
		y = p.y;
	}

}

