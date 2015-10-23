#include "stdafx.h"
#include "CDI8KeyboardDevice.h"
#include "D3DUtil.h"
#include "gfUtil.h"

namespace gf
{
	CDI8KeyboardDevice::CDI8KeyboardDevice()
		:mdiDevice(nullptr)
	{
		ZeroMemory(mKeys, sizeof(mKeys));
		ZeroMemory(mPrevKeys, sizeof(mPrevKeys));
	}


	bool CDI8KeyboardDevice::init(IDirectInput8* pDirectInput, HWND hwnd)
	{
		if (mdiDevice) {
			mdiDevice->Unacquire();
			ReleaseCOM(mdiDevice);
		}

		HRESULT hr;

		hr = pDirectInput->CreateDevice(GUID_SysKeyboard, &mdiDevice, NULL);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("Create Keyboard device failed!\n");
			return false;
		}

		hr = mdiDevice->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("Set Keyboard data format failed!\n");
			return false;
		}

		DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
		hr = mdiDevice->SetCooperativeLevel(hwnd, dwFlags);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("SetCooplevel for keyboard failed!\n");
			return false;
		}

		mdiDevice->Acquire();
		GF_PRINT_CONSOLE_INFO("KeyBoard Device has been initialized.\n");
		return true;
	}


	CDI8KeyboardDevice::~CDI8KeyboardDevice()
	{
		ReleaseCOM(mdiDevice);
	}

	void CDI8KeyboardDevice::update()
	{
		memcpy(mPrevKeys, mKeys, sizeof(mKeys));

		HRESULT hr;
		hr = mdiDevice->GetDeviceState(sizeof(mKeys), mKeys);

		if (FAILED(hr)) 
		{
			if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST)) {
				hr = mdiDevice->Acquire();
				while (hr == DIERR_INPUTLOST)
					hr = mdiDevice->Acquire();

				if (hr == DIERR_OTHERAPPHASPRIO) 
					return;

				if (SUCCEEDED(hr)) {
					hr = mdiDevice->GetDeviceState(sizeof(mKeys), mKeys);
				}
				if (FAILED(hr)) return;
			}
		}
	}

	bool CDI8KeyboardDevice::isPressed(u32 nID) const
	{
		if (mKeys[nID] & 0x80)
			return true;
		return false;
	}

	bool CDI8KeyboardDevice::keyUp(u32 nID) const
	{
		if ((mPrevKeys[nID] & 0x80) && !(mKeys[nID] & 0x80))
			return true;
		return false;
	}

	bool CDI8KeyboardDevice::keyDown(u32 nID) const
	{
		if (!(mPrevKeys[nID] & 0x80) && (mKeys[nID] & 0x80))
			return true;
		return false;
	}

	

}

