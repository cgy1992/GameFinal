#include "stdafx.h"
#include "CWin32App.h"
#include "CD3D11Driver.h"
#include "CSceneManager.h"
#include "CWin32Timer.h"
#include "CDInput8Driver.h"
#include "CThreadPool.h"
#include <iostream>

namespace gf
{

	static CWin32App* g_pWin32Device = NULL;


	CWin32App::CWin32App(const SAppSettings& params)
		:IApplication(params)
		, mDefinedWndProc(NULL)
		, m_hParentWnd(0)
		, m_hFrameWnd(0)
		, m_hBufferWnd(0)
	{
		DWORD dwRet;
		dwRet = GetCurrentDirectoryA(MAX_PATH, mProcessPath);
		//printf("Current Directory: %s\n", Buffer);
		strcat_s(mProcessPath, "/");

		if (params.Window.WindowsProcedure)
			mDefinedWndProc = (WNDPROC)(params.Window.WindowsProcedure);
	}

	CWin32App::~CWin32App()
	{
		//ReleaseReferenceCounted(mSceneManager);
		if (mThreadPool) {
			mThreadPool->stop();
			delete mThreadPool;
		}

		ReleaseReferenceCounted(mVideoDriver);
		ReleaseReferenceCounted(mInputDriver);
	}

	HRESULT CWin32App::init()
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;

		// Get an external pointer to this object.	
		g_pWin32Device = this;

		// Get the instance of this application.
		m_hInstance = GetModuleHandle(NULL);

		if (mSettings.Window.FrameWindowHandle == 0) {
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
			if (mSettings.Window.Style & EWS_FULLRESOLUTION)
			{
				mSettings.Window.Width = GetSystemMetrics(SM_CXSCREEN);
				mSettings.Window.Height = GetSystemMetrics(SM_CYSCREEN);
				mSettings.Graphics.BackBufferWidth = mSettings.Window.Width;
				mSettings.Graphics.BackBufferHeight = mSettings.Window.Height;
				// m_CreationParams.BackBufferHeight = m_CreationParams.ClientHeight;
			}

			// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
			if (mSettings.Window.Style & EWS_FULLSCREEN)
			{
				// If full screen set the screen to maximum size of the users desktop and 32bit.
				memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
				dmScreenSettings.dmSize = sizeof(dmScreenSettings);
				dmScreenSettings.dmPelsWidth = (unsigned long)mSettings.Window.Width;
				dmScreenSettings.dmPelsHeight = (unsigned long)mSettings.Window.Height;
				dmScreenSettings.dmBitsPerPel = 32;
				dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				// Change the display settings to full screen.
				ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

				// Create the window with the screen settings and get the handle to it.
				m_hFrameWnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("Game Final App"),
					WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
					0, 0, mSettings.Window.Width,
					mSettings.Window.Height, NULL, NULL, m_hInstance, NULL);
			}
			else
			{
				int posX = (GetSystemMetrics(SM_CXSCREEN) - mSettings.Window.Width) / 2;
				int posY = (GetSystemMetrics(SM_CYSCREEN) - mSettings.Window.Height) / 2;

				RECT R = { 0, 0, mSettings.Window.Width, mSettings.Window.Height };
				AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

				// Create the window with the screen settings and get the handle to it.
				m_hFrameWnd = CreateWindowEx(WS_EX_APPWINDOW, className, TEXT("Game Final App"),
					WS_OVERLAPPEDWINDOW, posX, posY, R.right - R.left, R.bottom - R.top,
					NULL, NULL, m_hInstance, NULL);
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
			m_hParentWnd = (HWND)mSettings.Window.ParentHandle;
			m_hFrameWnd = (HWND)mSettings.Window.FrameWindowHandle;
			if (m_hParentWnd == 0) {
				m_hParentWnd = m_hFrameWnd;
			}
		}

		//m_CreationParams.FrameWindowHandle = (u32)mHwnd;
		//m_CreationParams.BackBufferWindowHandle = (u32)mHwnd;

		if (!(mSettings.Window.Style & EWS_FULLSCREEN))
		{
			HWND hBufferWnd = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_WHITERECT,
				0, 0, mSettings.Graphics.BackBufferWidth, mSettings.Graphics.BackBufferHeight,
				m_hParentWnd, (HMENU)9, m_hInstance, NULL);

			// m_CreationParams.BackBufferWindowHandle = (u32)hBufferWnd;
			m_hBufferWnd = hBufferWnd;
		}

		mSettings.Window.BackBufferWindowHandle = (u32)m_hBufferWnd;

		// 判断用户指定要使用的图形库，如果未指定则根据系统来指定
		if (mSettings.Graphics.Driver == EDT_UNKNOWN) {
#ifdef _WIN32
			mSettings.Graphics.Driver = EDT_DIRECT3D11;
#else
			mSettings.Graphics.Driver = EDT_OPENGL;
#endif // _WIN32
		}

		if (mSettings.Graphics.Driver == EDT_DIRECT3D11)
		{
			CD3D11Driver* pd3d11Driver = new CD3D11Driver(this);
			mVideoDriver = pd3d11Driver;
			IVideoDriver::_setInstance(mVideoDriver);
			if (!pd3d11Driver->init(mSettings))
			{
				throw std::runtime_error("Video Driver init failed!");
			}
		} 
		else
		{
			throw std::runtime_error("The Driver is not supported so far!");
		}

		if (mSettings.Input.CreateDriver)
		{
			if (mSettings.Input.Driver == EIDT_AUTO)
				mSettings.Input.Driver = EIDT_DIRECTINPUT8;

			if (mSettings.Input.Driver == EIDT_DIRECTINPUT8)
			{
				CDInput8Driver* dinput = new CDInput8Driver(m_hFrameWnd);
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

		// create thread pool
		if (mSettings.Thread.ThreadPoolLimit > 0) {
			mThreadPool = new CThreadPool(mSettings.Thread.ThreadPoolLimit);
			mThreadPool->start();
		}

		return S_OK;
	}

	ISceneManager* CWin32App::createSceneManager(const math::SAxisAlignedBox& aabb)
	{
		ISceneManager* sceneManager = new CSceneManager(this, aabb);
		return sceneManager;
	}

	ISceneManager* CWin32App::createSceneManager()
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

	LRESULT CALLBACK CWin32App::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
			if (mInputDriver && mSettings.Input.Driver == EIDT_DIRECTINPUT8)
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

	void CWin32App::run() {
		MSG msg;
		mStopped = false;
		ITimer* timer = this->getTimer();
		timer->reset();

		while (!mStopped) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					mStopped = true;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (mStopped)
				break;

			if (mInputDriver)
				mInputDriver->update();

			float dt = timer->tick();
			if (!mUpdateCallback(dt)) {
				mStopped = true;
			}
		}
	}

	/*
	bool CWin32App::run()
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
	*/

	void CWin32App::setWindowCaption(const char* caption)
	{
		SetWindowTextA(m_hFrameWnd, caption);
	}

	void CWin32App::clientToScreen(u32& x, u32& y) const
	{
		POINT point = { x, y };
		ClientToScreen(m_hFrameWnd, &point);
		x = point.x;
		y = point.y;
	}

	void CWin32App::screenToClient(u32& x, u32& y) const
	{
		POINT point = { x, y };
		ScreenToClient(m_hFrameWnd, &point);
		x = point.x;
		y = point.y;
	}
}