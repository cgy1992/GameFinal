#include "InputHandler.h"
#include <map>

u32 InputHandler::mKeyPressed[] = { 0 };
std::map<u8, E_KEY_EVENT> InputHandler::mKeyMappings;

void InputHandler::update()
{
	ZeroMemory(mKeyPressed, sizeof(mKeyPressed));

	for (auto it = mKeyMappings.begin(); it != mKeyMappings.end(); it++)
	{
		u8 vKey = it->first;
		E_KEY_EVENT keyEvent = it->second;

		if (GetAsyncKeyState(vKey) & 0x8000)
		{
			mKeyPressed[keyEvent] = 1;
		}
	}
}
