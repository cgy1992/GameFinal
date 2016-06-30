#ifndef __VIDEO_DRIVER_FACTORY_INTERFACE_H__
#define __VIDEO_DRIVER_FACTORY_INTERFACE_H__

#include "IReferenceCounted.h"
#include "IVideoDriver.h"
//#include "IInputDriver.h"

namespace gf
{
	class IVideoDriverFactory : public IReferenceCounted
	{
	public:
		virtual IVideoDriver* createVideoDriver(SAppSettings& settings) = 0;
	};
}


#endif

