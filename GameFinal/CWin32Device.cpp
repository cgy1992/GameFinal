#include "stdafx.h"
#include "CWin32Device.h"
#include "CD3D11Driver.h"
#include "CSceneManager.h"
#include "CWin32Timer.h"
#include <iostream>
namespace gf
{

	static CWin32Device* g_pWin32Device = NULL;


	CWin32Device::CWin32Device(SCreationParameters& params)
		:IDevice(params)
	{
		DWORD dwRet;
		dwRet = GetCurrentDirectoryA(MAX_PATH, mProcessPath);
		//printf("Current Directory: %s\n", Buffer);
		strcat_s(mProcessPath, "/");
	}

	CWin32Device::~CWin32Device()
	{
		//ReleaseReferenceCounted(mSceneManager);
		ReleaseReferenceCounted(mVideoDriver);
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
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = gf::WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
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

		m_CreationParams.WindowHandle = (u32)mHwnd;
		//std::cout << sizeof(mHwnd) << std::endl;

		// Hide the mouse cursor.
		// ShowCursor(false);

		if (m_CreationParams.DriverType == EDT_DIRECT3D11)
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
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
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

		return true;
	}

	void CWin32Device::setWindowCaption(const char* caption)
	{
		SetWindowTextA(mHwnd, caption);
	}
}