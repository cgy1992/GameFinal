// GameFinal.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CBaseApp.h"
#include "GameFinal.h"
#include "CWin32App.h"
#include "CQt5App.h"

namespace gf
{
	// 这是导出变量的一个示例
	GAMEFINAL_API int nGameFinal = 0;

	// 这是导出函数的一个示例。
	GAMEFINAL_API int fnGameFinal(void)
	{
		return 42;
	}

	GAMEFINAL_API IApplication* createApp(SAppSettings settings)
	{
		if (settings.Graphics.BackBufferWidth == 0)
			settings.Graphics.BackBufferWidth = settings.Window.Width;

		if (settings.Graphics.BackBufferHeight == 0)
			settings.Graphics.BackBufferHeight = settings.Window.Height;

		CBaseApp* app = new CBaseApp(settings);
		IApplication::_setInstance(app);
		app->init();
		return app;
	}

	/*
	GAMEFINAL_API IApplication* createDevice(E_VIDEO_DRIVER_TYPE driverType, u32 width, u32 height, u32 style, bool vsync, const SDeviceContextSettings& settings)
	{
		SCreationParameters creationParams;
		creationParams.ClientWidth = width;
		creationParams.ClientHeight = height;
		creationParams.VideoDriverType = driverType;
		creationParams.DepthBits = settings.DepthBits;
		creationParams.StencilBits = settings.StencilBits;
		creationParams.MultiSamplingCount = settings.MultiSamplingCount;
		creationParams.MultiSamplingQuality = settings.MultiSamplingQuality;
		creationParams.WindowsProcedure = settings.WindowsProcedure;
		creationParams.BackBufferWidth = settings.BackBufferWidth;
		creationParams.BackBufferHeight = settings.BackBufferHeight;
		creationParams.WindowStyle = style;
		creationParams.VsyncEnabled = vsync;
		
		creationParams.InputDriverType = settings.InputDriverType;
		creationParams.CreateInputDriver = settings.CreateInputDriver;
		creationParams.ThreadPoolLimit = settings.ThreadPoolLimit;

		if (settings.BackBufferWidth == 0)
			creationParams.BackBufferWidth = width;

		if (settings.BackBufferHeight == 0)
			creationParams.BackBufferHeight = height;

		CWin32App* device = new CWin32App(creationParams);
		HRESULT hr = device->init();
		if (FAILED(hr))
			return NULL;

		IApplication::_setInstance(device);

		return device;
	}
	*/
}

