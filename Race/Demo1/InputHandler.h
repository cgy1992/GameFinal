#ifndef __INPUT_HANDLER_CLASS_H__
#define __INPUT_HANDLER_CLASS_H__

#include "Windows.h"
#include "GameFinal.h"
#include <map>

using namespace gf;

enum E_KEY_EVENT
{
	E_MOVE_FORWARD,
	E_MOVE_BACK,
	E_MOVE_UP,
	E_MOVE_DOWN,
	E_TURE_LEFT,
	E_TURE_RIGHT,
	E_LOOK_UP,
	E_LOOK_DOWN,
	E_LOOK_LEFT,
	E_LOOK_RIGHT,
	E_SWITCH_CAMERA,
	E_RESET_ROTATION,
};

class InputHandler
{
public:
	
	static void update();

	static bool keyPressed(E_KEY_EVENT key)
	{
		return mKeyPressed[key] == 1;
	}
	

	static void setKeyMapping(E_KEY_EVENT keyEvent, u8 vKey)
	{
		mKeyMappings[vKey] = keyEvent;
	}

	static void removeKeyMapping(u8 vKey)
	{
		auto it = mKeyMappings.find(vKey);
		if (it != mKeyMappings.end())
		{
			mKeyMappings.erase(it);
		}
	}

private:

	static u32							mKeyPressed[256];
	static std::map<u8, E_KEY_EVENT>	mKeyMappings;
};


#endif
