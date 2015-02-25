#ifndef IDEVICE_H
#define IDEVICE_H

#include "gfTypes.h"
#include "gfUtil.h"
#include "ITimer.h"
#include "gfMath.h"

namespace gf
{

	class IVideoDriver;
	class ISceneManager;

	enum E_WINDOW_STYLE
	{
		EWS_NONE = 0,
		EWS_FULLSCREEN = 1 << 0,
		EWS_FULLRESOLUTION = 1 << 1
	};

	enum E_DRIVER_TYPE
	{
		EDT_DIRECT3D11,
		EDT_DIRECT3D10,
		EDT_EIRECT3D9,
		EDT_OPENGL
	};

	// This is used as the parameters of the createDevice function by the user 
	//typedef LRESULT(CALLBACK* WNDPROC) (HWND, UINT, WPARAM, LPARAM);
	//typedef u32 (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
	struct SDeviceContextSettings
	{
		u32				DepthBits;
		u32				StencilBits;
		u32				MultiSamplingCount;
		u32				MultiSamplingQuality;
		u32				BackBufferWidth;
		u32				BackBufferHeight;
		void*			WindowsProcedure;

		SDeviceContextSettings()
		{
			DepthBits = 24;
			StencilBits = 8;
			MultiSamplingCount = 1;
			MultiSamplingQuality = 0;
			WindowsProcedure = nullptr;
			BackBufferHeight = 0;
			BackBufferWidth = 0;
		}
	};

	struct SCreationParameters
	{
		E_DRIVER_TYPE	DriverType;
		u32				ClientWidth;
		u32				ClientHeight;
		u32				BackBufferWidth;
		u32				BackBufferHeight;
		E_WINDOW_STYLE  WindowStyle;
		bool			VsyncEnabled;
		u32				FrameWindowHandle;
		u32				BackBufferWindowHandle;

		/* fields below are from SDeviceContextSettings*/
		u32				DepthBits;
		u32				StencilBits;
		u32				MultiSamplingCount;
		u32				MultiSamplingQuality;
		void*			WindowsProcedure;

		SCreationParameters()
		{
			DriverType = EDT_DIRECT3D11;
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
		}
	};



	class IDevice : public IReferenceCounted
	{
	public:
		IDevice(SCreationParameters& params) 
			:m_CreationParams(params)
			, mVideoDriver(nullptr)
			, mTimer(nullptr)
		{

		}

		virtual bool run() = 0;

		const SCreationParameters& getCreationParameters() const
		{
			return m_CreationParams;
		}

		IVideoDriver*	getVideoDriver()
		{
			return mVideoDriver;
		}

		virtual ISceneManager* createSceneManager() = 0;

		virtual ISceneManager* createSceneManager(const math::SAxisAlignedBox& aabb) = 0;

		u32 getClientWidth() const
		{
			return m_CreationParams.ClientWidth;
		}

		u32 getClientHeight() const
		{
			return m_CreationParams.ClientHeight;
		}

		virtual void setWindowCaption(const char* caption) = 0;

		u32 getClientWidth()
		{
			return m_CreationParams.ClientWidth;
		}

		u32 getClientHeight()
		{
			return m_CreationParams.ClientHeight;
		}

		virtual void clientToScreen(u32& x, u32& y) const = 0;
		virtual void screenToClient(u32& x, u32& y) const = 0;

		virtual ITimer* getTimer() const
		{
			return mTimer;
		}

		virtual ~IDevice()
		{

		}

		const char* getProcessPath() const
		{
			return mProcessPath;
		}

		_DECLARE_SINGLETON_INSTANCE(IDevice);

	protected:
		SCreationParameters		m_CreationParams;
		IVideoDriver*			mVideoDriver;
		//ISceneManager*			mSceneManager;
		ITimer*					mTimer;
		char					mProcessPath[MAX_PATH + 1];

	};

}
#endif