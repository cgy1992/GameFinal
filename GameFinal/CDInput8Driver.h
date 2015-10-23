#ifndef __DINPUTDRIVER_CLASS_H__
#define __DINPUTDRIVER_CLASS_H__

#include "IInputDriver.h"
#include "D3DUtil.h"

namespace gf
{
	class CDInput8Driver : public IInputDriver
	{
	public:
		CDInput8Driver(HWND hwnd)
			:mHwnd(hwnd)
		{
			mDirectInput8 = NULL;
		}

		virtual bool init();

		virtual void update();

		virtual ~CDInput8Driver();

	private:

		HWND					mHwnd;
		IDirectInput8*			mDirectInput8;



	};

}


#endif

