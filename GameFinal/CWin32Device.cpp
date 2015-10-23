#include "stdafx.h"
#include "CWin32Device.h"
#include "CD3D11Driver.h"
#include "CSceneManager.h"
#include "CWin32Timer.h"
#include "CDInput8Driver.h"
#include <iostream>

namespace gf
{

	static CWin32Device* g_pWin32Device = NULL;


	CWin32Device::CWin32Device(SCreationParameters& params)
		:IDevice(params)
		, mDefinedWndProc(NULL)
	{
		DWORD dwRet;
		dwRet = GetCurrentDirectoryA(MAX_PATH, mProcessPath);
		//printf("Current Directory: %s\n", Buffer);
		strcat_s(mProcessPath, "/");

		if (params.WindowsProcedure)
			mDefinedWndProc = (WNDPROC)(params.WindowsProcedure);
	}

	CWin32Device::~CWin32Device()
	{
		//ReleaseReferenceCounted(mSceneManager);
		ReleaseReferenceCounted(mVideoDriver);
		ReleaseReferenceCounted(mInputDriver);
	}

	HRESULT CWin32Device::init()
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;

		// Get an external pointer to this object.	
		g_pWin32Device = this;

		// Get the instance of this application.
		m_hInstance = GetModuleHandle(NULL);

		// Give the application a name.
		static TCHAR className[] = TEXT("MyGame");

		// Setup the windows class with default settings.
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
		wc.lpfnWndProc = gf::WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
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
		if (m_CreationParams.WindowStyle & EWS_FULLRESOLUTION)
		{
			m_CreationParams.ClientWidth = GetSystemMetrics(SM_CXSCREEN);
			m_CreationParams.ClientHeight = GetSystemMetrics(SM_CYSCREEN);
			m_CreationParams.BackBufferWidth = m_CreationParams.ClientWidth;
			m_CreationParams.BackBufferHeight = m_CreationParams.ClientHeight;
		}

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if (m_CreationParams.WindowStyle & EWS_FULLSCREEN)
		{
			// If full screen set the screen to maximum size of the users desktop and 32bit.
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)m_CreationParams.ClientWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)m_CreationParams.ClientHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Change the display settings to full screen.
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			// Create the window with the screen settings and get the handle to it.
			mHwnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("Game Final App"),
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
				0, 0, m_CreationParams.ClientWidth,
				m_CreationParams.ClientHeight, NULL, NULL, m_hInstance, NULL);
		}
		else
		{
			int posX = (GetSystemMetrics(SM_CXSCREEN) - m_CreationParams.ClientWidth) / 2;
			int posY = (GetSystemMetrics(SM_CYSCREEN) - m_CreationParams.ClientHeight) / 2;

			RECT R = { 0, 0, m_CreationParams.ClientWidth, m_CreationParams.ClientHeight };
			AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

			// Create the window with the screen settings and get the handle to it.
			mHwnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("Game Final App"),
				WS_OVERLAPPEDWINDOW, posX, posY, R.right - R.left, R.bottom - R.top, NULL, NULL, m_hInstance, NULL);
		}

		// Bring the window up on the screen and set it as main focus.
		ShowWindow(mHwnd, SW_SHOW);
		SetForegroundWindow(mHwnd);
		SetFocus(mHwnd);

		m_CreationParams.FrameWindowHandle = (u32)mHwnd;
		m_CreationParams.BackBufferWindowHandle = (u32)mHwnd;

		if (!(m_CreationParams.WindowStyle & EWS_FULLSCREEN))
		{
			HWND hBufferWnd = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_WHITERECT,
				0, 0, m_CreationParams.BackBufferWidth, m_CreationParams.BackBufferHeight,
				mHwnd, (HMENU)9, m_hInstance, NULL);

			m_CreationParams.BackBufferWindowHandle = (u32)hBufferWnd;
		}

		if (m_CreationParams.VideoDriverType == EDT_DIRECT3D11)
		{
			mVideoDriver = new CD3D11Driver(this);
			IVideoDriver::_setInstance(mVideoDriver);
			if (!mVideoDriver->init(m_CreationParams))
			{
				throw std::runtime_error("Video Driver init failed!");
			}
		}
		else
		{
			throw std::runtime_error("The Driver is not supported so far!");
		}

		if (m_CreationParams.CreateInputDriver)
		{
			if (m_CreationParams.InputDriverType == EIDT_AUTO)
				m_CreationParams.InputDriverType = EIDT_DIRECTINPUT8;

			if (m_CreationParams.InputDriverType == EIDT_DIRECTINPUT8)
			{
				CDInput8Driver* dinput = new CDInput8Driver(mHwnd);
				mInputDriver = dinput;
				mInputDriver->_setInstance(mInputDriver);
				if (!mInputDriver->init()) {
					throw std::runtime_error("Input Driver init failed!");
				}

				POINT p;
				GetCursorPos(&p);
				dinput->setCurrentMousePos(p.x, p.y);
			}
			else
			{
				throw std::runtime_error("The Input Driver is not supported so far!");
			}

		}
		

		// create timer
		mTimer = new CWin32Timer;
		mTimer->reset();

		return S_OK;
	}

	ISceneManager* CWin32Device::createSceneManager(const math::SAxisAlignedBox& aabb)
	{
		ISceneManager* sceneManager = new CSceneManager(this, aabb);
		return sceneManager;
	}

	ISceneManager* CWin32Device::createSceneManager()
	{
		math::SAxisAlignedBox aabb;
		aabb.Center = XMFLOAT3(0, 0, 0);
		aabb.Extents = XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
		return createSceneManager(aabb);
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return g_pWin32Device->WndProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK CWin32Device::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
			if (mInputDriver && m_CreationParams.InputDriverType == EIDT_DIRECTINPUT8)
			{
				CDInput8Driver* input = dynamic_cast<CDInput8Driver*>(mInputDriver);				
				POINT p;
				GetCursorPos(&p);
				input->setCurrentMousePos(p.x, p.y);
			}
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	}

	bool CWin32Device::run()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (mInputDriver)
			mInputDriver->update();

		return true;
	}

	void CWin32Device::setWindowCaption(const char* caption)
	{
		SetWindowTextA(mHwnd, caption);
	}

	void CWin32Device::clientToScreen(u32& x, u32& y) const
	{
		POINT point = { x, y };
		ClientToScreen(mHwnd, &point);
		x = point.x;
		y = point.y;
	}

	void CWin32Device::screenToClient(u32& x, u32& y) const
	{
		POINT point = { x, y };
		ScreenToClient(mHwnd, &point);
		x = point.x;
		y = point.y;
	}
}