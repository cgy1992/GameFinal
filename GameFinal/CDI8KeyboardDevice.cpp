#include "stdafx.h"
#include "CDI8KeyboardDevice.h"
#include "D3DUtil.h"
#include "gfUtil.h"

namespace gf
{
	CDI8KeyboardDevice::CDI8KeyboardDevice()
		:mdIApplication(nullptr)
	{
		ZeroMemory(mKeys, sizeof(mKeys));
		ZeroMemory(mPrevKeys, sizeof(mPrevKeys));
	}


	bool CDI8KeyboardDevice::init(IDirectInput8* pDirectInput, HWND hwnd)
	{
		if (mdIApplication) {
			mdIApplication->Unacquire();
			ReleaseCOM(mdIApplication);
		}

		HRESULT hr;

		hr = pDirectInput->CreateDevice(GUID_SysKeyboard, &mdIApplication, NULL);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("Create Keyboard device failed!\n");
			return false;
		}

		hr = mdIApplication->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("Set Keyboard data format failed!\n");
			return false;
		}

		DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
		hr = mdIApplication->SetCooperativeLevel(hwnd, dwFlags);
		if (FAILED(hr)) {
			GF_PRINT_CONSOLE_INFO("SetCooplevel for keyboard failed!\n");
			return false;
		}

		mdIApplication->Acquire();
		GF_PRINT_CONSOLE_INFO("KeyBoard Device has been initialized.\n");
		return true;
	}


	CDI8KeyboardDevice::~CDI8KeyboardDevice()
	{
		ReleaseCOM(mdIApplication);
	}

	void CDI8KeyboardDevice::update()
	{
		memcpy(mPrevKeys, mKeys, sizeof(mKeys));

		HRESULT hr;
		hr = mdIApplication->GetDeviceState(sizeof(mKeys), mKeys);

		if (FAILED(hr)) 
		{
			if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST)) {
				hr = mdIApplication->Acquire();
				while (hr == DIERR_INPUTLOST)
					hr = mdIApplication->Acquire();

				if (hr == DIERR_OTHERAPPHASPRIO) 
					return;

				if (SUCCEEDED(hr)) {
					hr = mdIApplication->GetDeviceState(sizeof(mKeys), mKeys);
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

