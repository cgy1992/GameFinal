#include "stdafx.h"
#include "CQt5App.h"
#include "CQt5RenderWidget.h"
#include "CD3D11Driver.h"
#include "CDInput8Driver.h"
#include "CSceneManager.h"
#include "CWin32Timer.h"
#include "CThreadPool.h"

namespace gf
{

	CQt5App::CQt5App(const SAppSettings& params) 
		:IApplication(params),
		mRenderWidget(nullptr),
		mQApplication(nullptr)
	{
		DWORD dwRet;
		dwRet = GetCurrentDirectoryA(MAX_PATH, mProcessPath);
		//printf("Current Directory: %s\n", Buffer);
		strcat_s(mProcessPath, "/");

		//if (params.Window.WindowsProcedure)
		//	mDefinedWndProc = (WNDPROC)(params.Window.WindowsProcedure);
	}

	CQt5App::~CQt5App() 
	{
		if (mThreadPool) {
			mThreadPool->stop();
			delete mThreadPool;
		}

		ReleaseReferenceCounted(mVideoDriver);
		ReleaseReferenceCounted(mInputDriver);
	}

	void CQt5App::run()
	{
		mRenderWidget->show();
		mTimer->reset();
		if (mQApplication)
			mQApplication->exec();
	}

	bool CQt5App::init()
	{
		if (mSettings.Window.FrameWindowPtr == nullptr) {
			char* hello = "hello";
			int x = 1;
			mQApplication = new QApplication(x, &hello);
			//mRenderWidget = new CQt5RenderWidget(0, this);
			mRenderWidget->setFixedSize(mSettings.Window.Width, mSettings.Window.Height);
			
			mSettings.Window.FrameWindowHandle = mRenderWidget->winId();
			mSettings.Window.FrameWindowPtr = mRenderWidget;
			
		}
		else {
			QWidget* parent = (QWidget*)mSettings.Window.ParentWindowPtr;
			QWidget* frame = (QWidget*)mSettings.Window.FrameWindowPtr;
			if (parent == nullptr) {
				parent = frame;
			}

			//mRenderWidget = new CQt5RenderWidget(parent, this);
			mRenderWidget->setFixedSize(mSettings.Window.Width, mSettings.Window.Height);

			mSettings.Window.FrameWindowHandle = frame->winId();
			mSettings.Window.ParentWindowPtr = parent;
			mSettings.Window.ParentHandle = parent->winId();
		}

		mSettings.Window.BackBufferWindowHandle = mRenderWidget->winId();
		mSettings.Window.BackBufferWindowPtr = mRenderWidget;

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
				//QWidget* frameWidget = (QWidget*)mSettings.Window.FrameWindowPtr;
				CDInput8Driver* dinput = new CDInput8Driver((HWND)mSettings.Window.FrameWindowHandle);
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

		return true;
	}

	ISceneManager* CQt5App::createSceneManager(const math::SAxisAlignedBox& aabb)
	{
		ISceneManager* sceneManager = new CSceneManager(this, aabb);
		return sceneManager;
	}

	ISceneManager* CQt5App::createSceneManager()
	{
		math::SAxisAlignedBox aabb;
		aabb.Center = XMFLOAT3(0, 0, 0);
		aabb.Extents = XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
		return createSceneManager(aabb);
	}

	void CQt5App::setWindowCaption(const char* caption)
	{
		mRenderWidget->setWindowTitle(QString(caption));
	}

	void CQt5App::clientToScreen(u32& x, u32& y) const
	{
		POINT point = { x, y };
		ClientToScreen((HWND)mRenderWidget->winId(), &point);
		x = point.x;
		y = point.y;
	}

	void CQt5App::screenToClient(u32& x, u32& y) const
	{
		POINT point = { x, y };
		ScreenToClient((HWND)mRenderWidget->winId(), &point);
		x = point.x;
		y = point.y;
	}

	void CQt5App::step()
	{
		if (mInputDriver)
			mInputDriver->update();

		f32 dt = mTimer->tick();
		if (!mUpdateCallback(dt)) {
			mQApplication->quit();
		}
	}
}