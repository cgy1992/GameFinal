#include "stdafx.h"
#include "CVideoDriverFactory.h"
#include "CD3D11Driver.h"

namespace gf
{
	IVideoDriver* CVideoDriverFactory::createVideoDriver(SAppSettings& settings)
	{
		// 判断用户指定要使用的图形库，如果未指定则根据系统来指定
		if (settings.Graphics.Driver == EDT_UNKNOWN) {
#ifdef _WIN32
			settings.Graphics.Driver = EDT_DIRECT3D11;
#else
			settings.Graphics.Driver = EDT_OPENGL;
#endif // _WIN32
		}

		IApplication* app = IApplication::getInstance();
		if (settings.Graphics.Driver == EDT_DIRECT3D11)
		{
			CD3D11Driver* pd3d11Driver = new CD3D11Driver(app);
			//mVideoDriver = pd3d11Driver;
			IVideoDriver::_setInstance(pd3d11Driver);
			if (!pd3d11Driver->init(settings))
			{
				throw std::runtime_error("Video Driver init failed!");
			}
			return pd3d11Driver;
		}
		else
		{
			throw std::runtime_error("The Driver is not supported so far!");
		}

		return nullptr;
	}
}
