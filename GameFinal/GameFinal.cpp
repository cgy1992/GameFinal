// GameFinal.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GameFinal.h"
#include "CWin32Device.h"

namespace gf
{
	// 这是导出变量的一个示例
	GAMEFINAL_API int nGameFinal = 0;

	// 这是导出函数的一个示例。
	GAMEFINAL_API int fnGameFinal(void)
	{
		return 42;
	}

	GAMEFINAL_API IDevice* createDevice(E_VIDEO_DRIVER_TYPE driverType, u32 width, u32 height, u32 style, bool vsync, const SDeviceContextSettings& settings)
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

		if (settings.BackBufferWidth == 0)
			creationParams.BackBufferWidth = width;

		if (settings.BackBufferHeight == 0)
			creationParams.BackBufferHeight = height;

		CWin32Device* device = new CWin32Device(creationParams);
		HRESULT hr = device->init();
		if (FAILED(hr))
			return NULL;

		IDevice::_setInstance(device);

		return device;
	}
}

