#ifndef __DI8_MOUSE_DEVICE_CLASS_H__
#define __DI8_MOUSE_DEVICE_CLASS_H__

#include "IMouseDevice.h"

namespace gf
{
	class CDI8MouseDevice : public IMouseDevice
	{
	public:
		CDI8MouseDevice();

		bool init(IDirectInput8* pDirectInput, HWND hwnd, const RECT& cageRect);

		virtual void update();
		virtual bool isPressed(u32 nID) const;
		virtual bool keyUp(u32 nID) const;
		virtual bool keyDown(u32 nID) const;
		virtual void getPosition(int& x, int& y, int& z) const;
		virtual void getMovement(int& x, int& y, int& z) const;
		void setCurrentPos(int x, int y);
		virtual void showCursor(bool bDisplay);
		virtual ~CDI8MouseDevice();

	private:
		IDirectInputDevice8*		mdIApplication;
		HANDLE						mEvent;
		RECT						mCageRect;
		bool						mKeys[4];
		bool						mPrevKeys[4];
		long						mMouseX;
		long						mMouseY;
		long						mMouseZ;
		long						mDeltaX;
		long						mDeltaY;
		long						mDeltaZ;
	};

}

#endif

