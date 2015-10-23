#ifndef __DI8_KEYBOARD_DEVICE_CLASS_H__
#define __DI8_KEYBOARD_DEVICE_CLASS_H__

#include "IKeyboardDevice.h"

namespace gf
{
	class CDI8KeyboardDevice : public IKeyboardDevice
	{
	public:
		CDI8KeyboardDevice();

		bool init(IDirectInput8* pDirectInput, HWND hwnd);

		virtual void update();
		
		virtual bool isPressed(u32 nID) const;
		
		virtual bool keyUp(u32 nID) const;

		virtual bool keyDown(u32 nID) const;

		virtual ~CDI8KeyboardDevice();


	private:
		IDirectInputDevice8*		mdiDevice;
		char						mKeys[256];
		char						mPrevKeys[256];

	};
}


#endif
