#include "stdafx.h"
#include "CDInput8Driver.h"
#include "gfUtil.h"
#include "CDI8KeyboardDevice.h"
#include "CDI8MouseDevice.h"

namespace gf
{

	CDInput8Driver::~CDInput8Driver()
	{
		ReleaseReferenceCounted(mKeyboardDevice);
		ReleaseReferenceCounted(mMouseDevice);
		ReleaseCOM(mDirectInput8);
		
	}

	bool CDInput8Driver::init()
	{
		HRESULT hr;

		HINSTANCE hInstance = GetModuleHandleA(NULL);

		hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**)&mDirectInput8,
			NULL);

		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("DirectInput8 create failed!\n");
			return false;
		}

		CDI8KeyboardDevice* diKeyboardDevice = new CDI8KeyboardDevice();
		if (!diKeyboardDevice->init(mDirectInput8, mHwnd)) {
			GF_PRINT_CONSOLE_INFO("Keyboard device initialize failed!\n");
			return false;
		}
		mKeyboardDevice = diKeyboardDevice;

		CDI8MouseDevice* diMouseDevice = new CDI8MouseDevice();
		
		RECT desktopRect = { 0, 0, 0, 0 };
		HDC hdc = GetDC(NULL);
		desktopRect.right = GetDeviceCaps(hdc, HORZRES) - 1;
		desktopRect.bottom = GetDeviceCaps(hdc, VERTRES) - 1;
		ReleaseDC(NULL, hdc);
		if (!diMouseDevice->init(mDirectInput8, mHwnd, desktopRect)) {
			GF_PRINT_CONSOLE_INFO("Mousedevice initialize failed!\n");
			return false;
		}
		mMouseDevice = diMouseDevice;
		
		return true;
	}

	void CDInput8Driver::update()
	{
		mKeyboardDevice->update();
		mMouseDevice->update();
	}

	void CDInput8Driver::setCurrentMousePos(int x, int y)
	{
		CDI8MouseDevice* mouse = dynamic_cast<CDI8MouseDevice*>(mMouseDevice);
		mouse->setCurrentPos(x, y);
	}

	bool CDInput8Driver::keyUp(u32 nID) const
	{
		if (nID >= GVK_LBUTTON)
			return mMouseDevice->keyUp(nID);
		return mKeyboardDevice->keyUp(nID);
	}

	bool CDInput8Driver::keyDown(u32 nID) const
	{
		if (nID >= GVK_LBUTTON)
			return mMouseDevice->keyDown(nID);
		return mKeyboardDevice->keyDown(nID);
	}

	bool CDInput8Driver::isPressed(u32 nID) const
	{
		if (nID >= GVK_LBUTTON)
			return mMouseDevice->isPressed(nID);
		return mKeyboardDevice->isPressed(nID);
	}

	void CDInput8Driver::getMouseMovement(int& x, int& y, int& z) const
	{
		mMouseDevice->getMovement(x, y, z);
	}

	void CDInput8Driver::getMousePos(int& x, int& y, int& z) const
	{
		mMouseDevice->getPosition(x, y, z);
	}

}

