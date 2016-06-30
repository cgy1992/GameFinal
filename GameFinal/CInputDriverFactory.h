#ifndef __INPUT_DRIVER_FACTORY_CLASS_H__
#define __INPUT_DRIVER_FACTORY_CLASS_H__

#include "IInputDriverFactory.h"

namespace gf
{
	class CInputDriverFactory : public IInputDriverFactory
	{
	public:
		virtual IInputDriver* createInputDriver(SAppSettings& settings);
	};
}

#endif