#ifndef __INPUT_DRIVER_FACTORY_INTERFACE_H__
#define __INPUT_DRIVER_FACTORY_INTERFACE_H__

#include "IInputDriver.h"
#include "IApplication.h"

namespace gf
{
	class IInputDriverFactory : public IReferenceCounted
	{
	public:
		virtual IInputDriver* createInputDriver(SAppSettings& settings) = 0;
	};
}

#endif
