#include "stdafx.h"
#include "CBaseApp.h"
#include "CThreadPool.h"
#include "IInputDriver.h"
#include "IVideoDriver.h"
#include "CVideoDriverFactory.h"
#include "CInputDriverFactory.h"
#include "CWin32Window.h"
#include "CWin32Timer.h"
#include "CSceneManager.h"
#include "CQt5Window.h"

namespace gf
{
	CBaseApp::CBaseApp(const SAppSettings& params)
		:IApplication(params)
		, mInputDriverFactory(nullptr)
		, mVideoDriverFactory(nullptr)
	{
		// TODO: get the current dir
		DWORD dwRet;
		dwRet = GetCurrentDirectoryA(MAX_PATH, mProcessPath);
		//printf("Current Directory: %s\n", Buffer);
		strcat_s(mProcessPath, "/");

		// create the factories
		
		mInputDriverFactory = new CInputDriverFactory();
		mVideoDriverFactory = new CVideoDriverFactory();
	}

	CBaseApp::~CBaseApp()
	{
		if (mThreadPool) {
			mThreadPool->stop();
			delete mThreadPool;
		}


		ReleaseReferenceCounted(mVideoDriver);
		ReleaseReferenceCounted(mInputDriver);

		ReleaseReferenceCounted(mWindow);

		ReleaseReferenceCounted(mInputDriverFactory);
		ReleaseReferenceCounted(mVideoDriverFactory);
	}

	void CBaseApp::init()
	{
		// create window first.
		if (mSettings.AppType == EAP_WIN32) {
			mWindow = new CWin32Window(this);
		}
		else if (mSettings.AppType == EAP_QT5) {
			mWindow = new CQt5Window(this);
		}
		//mWindow = new CWin32Window(this);
		mWindow->init(mSettings);

		// create video driver.
		mVideoDriver = mVideoDriverFactory->createVideoDriver(mSettings);

		// create input driver.
		mInputDriver = mInputDriverFactory->createInputDriver(mSettings);
	
		// create timer
		mTimer = new CWin32Timer();
		mTimer->reset();

		// create thread pool
		if (mSettings.Thread.ThreadPoolLimit > 0) {
			mThreadPool = new CThreadPool(mSettings.Thread.ThreadPoolLimit);
			mThreadPool->start();
		}
	}

	void CBaseApp::run()
	{
		mTimer->reset();
		mWindow->loop([this]()->bool{
			
			if (mInputDriver) {
				mInputDriver->update();
			}

			f32 dt = mTimer->tick();
			return mUpdateCallback(dt);
		});
	}


	void CBaseApp::setWindowTitle(const std::string& caption)
	{
		mWindow->setTitle(caption);
	}

	ISceneManager* CBaseApp::createSceneManager(const math::SAxisAlignedBox& aabb)
	{
		ISceneManager* sceneManager = new CSceneManager(this, aabb);
		return sceneManager;
	}

	ISceneManager* CBaseApp::createSceneManager()
	{
		math::SAxisAlignedBox aabb;
		aabb.Center = XMFLOAT3(0, 0, 0);
		aabb.Extents = XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
		return createSceneManager(aabb);
	}

	void CBaseApp::clientToScreen(int& x, int& y) const
	{
		mWindow->clientToScreen(x, y);
	}

	void CBaseApp::screenToClient(int& x, int& y) const
	{
		mWindow->screenToClient(x, y);
	}

	void CBaseApp::getMousePos(int& x, int& y) const
	{
		mWindow->getMousePos(x, y);
	}

}
