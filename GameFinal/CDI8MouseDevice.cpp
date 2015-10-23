#include "stdafx.h"
#include "CDI8MouseDevice.h"
#include "D3DUtil.h"
#include "gfUtil.h"
#include "IInputDriver.h"

#define BUFFER_SIZE 16

namespace gf
{
	CDI8MouseDevice::CDI8MouseDevice()
		:mdiDevice(nullptr)
	{
		for (int i = 0; i < 3; i++) {
			mKeys[i] = false;
			mPrevKeys[i] = false;
		}

		mMouseX = 0;
		mMouseY = 0;
		mMouseZ = 0;
		mDeltaX = 0;
		mDeltaY = 0;
		mDeltaZ = 0;
	}

	CDI8MouseDevice::~CDI8MouseDevice()
	{
		ReleaseCOM(mdiDevice);
	}

	bool CDI8MouseDevice::init(IDirectInput8* pDirectInput, HWND hwnd, const RECT& cageRect)
	{
		this->mCageRect = cageRect;
		this->mMouseX = 0;
		this->mMouseY = 0;

		if (mdiDevice) {
			mdiDevice->Unacquire();
			ReleaseCOM(mdiDevice);
		}

		HRESULT hr;

		hr = pDirectInput->CreateDevice(GUID_SysMouse, &mdiDevice, NULL);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("Create Mouse device failed!\n");
			return false;
		}

		hr = mdiDevice->SetDataFormat(&c_dfDIMouse);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("Set Mouse data format failed!\n");
			return false;
		}

		DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
		hr = mdiDevice->SetCooperativeLevel(hwnd, dwFlags);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("SetCooplevel for mouse failed!\n");
			return false;
		}

		mEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (mEvent == INVALID_HANDLE_VALUE) {
			GF_PRINT_CONSOLE_INFO("CreateEvent(Mouse) failed!\n");
			return false;
		}

		hr = mdiDevice->SetEventNotification(mEvent);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("SetEventNotification(Mouse) failed\n");
			return false;
		}

		// create buffered input for the mouse
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = BUFFER_SIZE;

		hr = mdiDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("SetProperty(Mouse) failed\n");
			return false;
		}

		mdiDevice->Acquire();
		GF_PRINT_CONSOLE_INFO("Mouse device has been initialized.\n");
		return true;
	}

	void CDI8MouseDevice::update()
	{
		//memcpy(mPrevKeys, mKeys, sizeof(mKeys));
		mPrevKeys[0] = mKeys[0];
		mPrevKeys[1] = mKeys[1];
		mPrevKeys[2] = mKeys[2];

		DWORD  dwNum = BUFFER_SIZE;
		DIDEVICEOBJECTDATA data[BUFFER_SIZE];

		HRESULT hr;
		hr = mdiDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
			&data[0], &dwNum, 0);

		if (FAILED(hr))
		{
			if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST)) {
				hr = mdiDevice->Acquire();
				while (hr == DIERR_INPUTLOST)
					hr = mdiDevice->Acquire();

				if (hr == DIERR_OTHERAPPHASPRIO)
					return;

				if (SUCCEEDED(hr)) {
					hr = mdiDevice->GetDeviceData(
						sizeof(DIDEVICEOBJECTDATA),
						&data[0], &dwNum, 0);
				}
				if (FAILED(hr)) return;
			}
		}

		mDeltaX = 0;
		mDeltaY = 0;
		mDeltaZ = 0;

		int nBtn = 0;

		for (DWORD i = 0; i < dwNum; i++) {
			switch (data[i].dwOfs)
			{
			case DIMOFS_X:
				mMouseX += data[i].dwData;
				mDeltaX = data[i].dwData;
				if (mMouseX < mCageRect.left)
					mMouseX = mCageRect.left;
				else if (mMouseX > mCageRect.right)
					mMouseX = mCageRect.right;
				break;
			case DIMOFS_Y:
				mMouseY += data[i].dwData;
				mDeltaY = data[i].dwData;
				if (mMouseY < mCageRect.top)
					mMouseY = mCageRect.top;
				else if (mMouseY > mCageRect.bottom)
					mMouseY = mCageRect.bottom;
				break;
			case DIMOFS_Z:
				mMouseZ += data[i].dwData;
				mDeltaZ = data[i].dwData;
				break;
			case DIMOFS_BUTTON0:
			case DIMOFS_BUTTON1:
			case DIMOFS_BUTTON2:
				nBtn = data[i].dwOfs - DIMOFS_BUTTON0;
				if (data[i].dwData & 0x80) {
					mKeys[nBtn] = true;
				}
				else {
					mKeys[nBtn] = false;
				}
				break;
			default:
				break;
			}
		}
	}

	bool CDI8MouseDevice::isPressed(u32 nID) const
	{
		return mKeys[nID - GVK_LBUTTON];
	}

	bool CDI8MouseDevice::keyUp(u32 nID) const
	{
		u32 i = nID - GVK_LBUTTON;
		return mPrevKeys[i] && !mKeys[i];
	}

	bool CDI8MouseDevice::keyDown(u32 nID) const
	{
		u32 i = nID - GVK_LBUTTON;
		return !mPrevKeys[i] && mKeys[i];
	}

	void CDI8MouseDevice::getPosition(int& x, int& y, int& z) const
	{
		x = mMouseX;
		y = mMouseY;
		z = mMouseZ;
	}

	void CDI8MouseDevice::getMovement(int& x, int& y, int& z) const
	{
		x = mDeltaX;
		y = mDeltaY;
		z = mDeltaZ;
	}
}
