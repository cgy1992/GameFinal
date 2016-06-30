#ifndef IApplication_H
#define IApplication_H

#include "gfTypes.h"
#include "gfUtil.h"
#include "ITimer.h"
#include "gfMath.h"
#include <functional>

namespace gf
{

	class IVideoDriver;
	class ISceneManager;
	class IInputDriver;
	class IThreadPool;

	enum E_WINDOW_STYLE
	{
		EWS_NONE = 0,
		EWS_FULLSCREEN = 1 << 0,
		EWS_FULLRESOLUTION = 1 << 1
	};

	enum E_VIDEO_DRIVER_TYPE
	{
		EDT_UNKNOWN,
		EDT_DIRECT3D11,
		EDT_DIRECT3D10,
		EDT_EIRECT3D9,
		EDT_OPENGL
	};

	enum E_INPUT_DRIVER_TYPE
	{
		EIDT_AUTO,
		EIDT_WINSDK,
		EIDT_DIRECTINPUT8,
	};

	enum E_APP_TYPE
	{
		EAP_WIN32,
		EAP_QT5,
	};



	// This is used as the parameters of the createDevice function by the user 
	//typedef LRESULT(CALLBACK* WNDPROC) (HWND, UINT, WPARAM, LPARAM);
	//typedef u32 (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

	struct SWindowSettings
	{
		u32			Width;
		u32			Height;
		u32			Style;
		u32			ParentHandle;
		u32			FrameWindowHandle;
		u32			BackBufferWindowHandle;
		void*		ParentWindowPtr;
		void*		FrameWindowPtr;
		void*		BackBufferWindowPtr;
		void*		WindowsProcedure;

		SWindowSettings()
		{
			Width = 800;
			Height = 600;
			Style = EWS_NONE;
			ParentHandle = 0;
			FrameWindowHandle = 0;
			BackBufferWindowHandle = 0;
			WindowsProcedure = NULL;
			ParentWindowPtr = nullptr;
			FrameWindowPtr = nullptr;
			BackBufferWindowPtr = nullptr;
		}
	};

	struct SGraphicsSettings
	{
		E_VIDEO_DRIVER_TYPE		Driver;
		u32						DepthBits;
		u32						StencilBits;
		u32						MultiSamplingCount;
		u32						MultiSamplingQuality;
		u32						BackBufferWidth;
		u32						BackBufferHeight;
		bool					VsyncEnabled;

		SGraphicsSettings() 
		{
			Driver = EDT_UNKNOWN;
			DepthBits = 24;
			StencilBits = 8;
			MultiSamplingCount = 1;
			MultiSamplingQuality = 0;
			BackBufferHeight = 0;
			BackBufferWidth = 0;
			VsyncEnabled = false;
		}
	};

	struct SThreadSettings
	{
		u32						ThreadPoolLimit; // the number of threads in thread pool, 0 means no thread pool.
		SThreadSettings() {
			ThreadPoolLimit = 0;
		}
	};

	struct SInputSettings
	{
		bool					CreateDriver; // create the input driver. If user wants to use his own input IO library, set this to false.
		E_INPUT_DRIVER_TYPE		Driver; // input-driver type, default is DirectInput8 Driver
		
		SInputSettings() 
		{
			CreateDriver = true;
			Driver = EIDT_AUTO;
		}
	};

	struct SAppSettings
	{
		E_APP_TYPE			AppType;
		SWindowSettings		Window;
		SGraphicsSettings	Graphics;
		SInputSettings		Input;
		SThreadSettings		Thread;

		SAppSettings()
		{
#ifdef _WIN32
			AppType = EAP_WIN32;
#else
			AppType = EAP_QT5;
#endif // _WIN32
			
		}
	};

	/*
	struct SDeviceContextSettings
	{
		u32						DepthBits;
		u32						StencilBits;
		u32						MultiSamplingCount;
		u32						MultiSamplingQuality;
		u32						BackBufferWidth;
		u32						BackBufferHeight;
		void*					WindowsProcedure;
		bool					CreateInputDriver; // create the input driver. If user wants to use his own input IO library, set this to false.
		E_INPUT_DRIVER_TYPE		InputDriverType; // input-driver type, default is DirectInput8 Driver
		u32						ThreadPoolLimit; // the number of threads in thread pool, 0 means no thread pool.

		SDeviceContextSettings()
		{
			DepthBits = 24;
			StencilBits = 8;
			MultiSamplingCount = 1;
			MultiSamplingQuality = 0;
			WindowsProcedure = nullptr;
			BackBufferHeight = 0;
			BackBufferWidth = 0;

			CreateInputDriver = true;
			InputDriverType = EIDT_AUTO;

			ThreadPoolLimit = 0;
		}
	};

	struct SCreationParameters
	{
		E_VIDEO_DRIVER_TYPE	VideoDriverType;
		E_INPUT_DRIVER_TYPE InputDriverType;
		u32				ClientWidth;
		u32				ClientHeight;
		u32				BackBufferWidth;
		u32				BackBufferHeight;
		u32				WindowStyle;
		bool			VsyncEnabled;
		u32				FrameWindowHandle;
		u32				BackBufferWindowHandle;

		u32				DepthBits;
		u32				StencilBits;
		u32				MultiSamplingCount;
		u32				MultiSamplingQuality;
		void*			WindowsProcedure;

		bool			CreateInputDriver;

		u32				ThreadPoolLimit;
		SCreationParameters()
		{
			VideoDriverType = EDT_DIRECT3D11;
			ClientHeight = 0;
			ClientWidth = 0;
			
			BackBufferWidth = 0;
			BackBufferHeight = 0;

			WindowStyle = EWS_NONE;
			VsyncEnabled = false;
			FrameWindowHandle = 0;
			BackBufferWindowHandle = 0;

			DepthBits = 24;
			StencilBits = 8;
			MultiSamplingCount = 1;
			MultiSamplingQuality = 0;
			WindowsProcedure = nullptr;

			ThreadPoolLimit = 0;
		}
	};
	*/

	typedef std::function<bool(f32)> AppUpdateCallbackFunction;

	class IWindow;

	class IApplication : public IReferenceCounted
	{

	public:
		IApplication(const SAppSettings& params) 
			: mSettings(params)
			, mWindow(nullptr)
			, mVideoDriver(nullptr)
			, mInputDriver(nullptr)
			, mThreadPool(nullptr)
			, mTimer(nullptr)
			, mStopped(false)
		{

		}

		virtual void setUpdateCallback(AppUpdateCallbackFunction func)
		{
			mUpdateCallback = func;
		}

		virtual void run() = 0;

		const SAppSettings& getSettings() const
		{
			return mSettings;
		}

		IVideoDriver*	getVideoDriver()
		{
			return mVideoDriver;
		}

		IInputDriver*	getInputDriver() 
		{
			return mInputDriver;
		}

		virtual ISceneManager* createSceneManager() = 0;

		virtual ISceneManager* createSceneManager(const math::SAxisAlignedBox& aabb) = 0;

		u32 getClientWidth() const
		{
			return mSettings.Window.Width;
		}

		u32 getClientHeight() const
		{
			return mSettings.Window.Height;
		}

		virtual void setWindowTitle(const std::string& caption) = 0;
		virtual void clientToScreen(int& x, int& y) const = 0;
		virtual void screenToClient(int& x, int& y) const = 0;

		virtual ITimer* getTimer() const
		{
			return mTimer;
		}

		virtual IThreadPool* getThreadPool()
		{
			return mThreadPool;
		}

		virtual void getMousePos(int& x, int& y) const = 0;

		IWindow* getWindow() { return mWindow; }

		virtual ~IApplication()
		{

		}

		const char* getProcessPath() const
		{
			return mProcessPath;
		}

		_DECLARE_SINGLETON_INSTANCE(IApplication);

	protected:
		SAppSettings			mSettings;
		// SCreationParameters		m_CreationParams;
		IVideoDriver*			mVideoDriver;
		IInputDriver*			mInputDriver;

		IThreadPool*			mThreadPool;

		IWindow*				mWindow;

		//ISceneManager*			mSceneManager;
		ITimer*					mTimer;
		char					mProcessPath[MAX_PATH + 1];
		AppUpdateCallbackFunction	mUpdateCallback;
		bool						mStopped;
	};

}
#endif