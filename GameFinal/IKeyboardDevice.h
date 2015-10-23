#ifndef __KEYBOARD_DEVICE_INTERFACE_H__
#define __KEYBOARD_DEVICE_INTERFACE_H__

#include "IReferenceCounted.h"

namespace gf
{
	class IKeyboardDevice : public IReferenceCounted
	{
	public:
		virtual void update() = 0;
		virtual bool isPressed(u32 nID) const = 0;
		virtual bool keyUp(u32 nID) const = 0;
		virtual bool keyDown(u32 nID) const = 0;
	};
}




#endif

