#ifndef __VIDEO_DRIVER_FACTORY_CLASS_H__
#define __VIDEO_DRIVER_FACTORY_CLASS_H__

#include "IVideoDriverFactory.h"

namespace gf
{
	class CVideoDriverFactory : public IVideoDriverFactory
	{
	public:
		virtual IVideoDriver* createVideoDriver(SAppSettings& settings);
	};
}

#endif

