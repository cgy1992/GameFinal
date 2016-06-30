#include "stdafx.h"
#include "CInputDriverFactory.h"
#include "CDInput8Driver.h"

namespace gf
{
	IInputDriver* CInputDriverFactory::createInputDriver(SAppSettings& settings)
	{
		if (!settings.Input.CreateDriver)
			return nullptr;

		if (settings.Input.Driver == EIDT_AUTO)
			settings.Input.Driver = EIDT_DIRECTINPUT8;

		if (settings.Input.Driver == EIDT_DIRECTINPUT8)
		{
			CDInput8Driver* dinput = new CDInput8Driver((HWND)settings.Window.FrameWindowHandle);
			IInputDriver::_setInstance(dinput);
			if (!dinput->init()) {
				throw std::runtime_error("Input Driver init failed!");
			}

			POINT p;
			GetCursorPos(&p);
			dinput->setCurrentMousePos(p.x, p.y);
			return dinput;
		}
		else
		{
			throw std::runtime_error("The Input Driver is not supported so far!");
		}

		return nullptr;
	}

}
