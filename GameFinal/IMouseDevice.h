#ifndef __MOUSE_DEVICE_INTERFACE_H__
#define __MOUSE_DEVICE_INTERFACE_H__

#include "IReferenceCounted.h"

namespace gf
{
	class IMouseDevice : public IReferenceCounted
	{
	public:
		virtual void update() = 0;
		virtual bool isPressed(u32 nID) const = 0;
		virtual bool keyUp(u32 nID) const = 0;
		virtual bool keyDown(u32 nID) const = 0;
		virtual void getPosition(int& x, int& y, int& z) const = 0;
		virtual void getMovement(int& x, int& y, int& z) const = 0;
		/*
		virtual int getMovementX() const = 0;
		virtual int getMovementY() const = 0;
		virtual int getWheelMovement();
		*/
	};
}

#endif

