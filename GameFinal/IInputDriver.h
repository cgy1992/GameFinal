#ifndef __INPUTDRIVER_INTERFACE_H__
#define __INPUTDRIVER_INTERFACE_H__

#include "IReferenceCounted.h"
#include "IKeyboardDevice.h"
#include "IMouseDevice.h"
#include "gfUtil.h"

namespace gf
{
	class IInputDriver : public IReferenceCounted
	{
	public:
		IInputDriver() 
			:mKeyboardDevice(nullptr)
		{
			
		}

		virtual bool init() = 0;

		virtual void update() = 0;

		IKeyboardDevice*	getKeyboard() { return mKeyboardDevice; }

		IMouseDevice*		getMouse() { return mMouseDevice; }

		virtual bool keyUp(u32 nID) const = 0;

		virtual bool keyDown(u32 nID) const = 0;

		virtual bool isPressed(u32 nID) const = 0;

		virtual void getMouseMovement(int& x, int& y, int& z) const = 0;

		virtual void getMousePos(int& x, int& y, int& z) const = 0;

		_DECLARE_SINGLETON_INSTANCE(IInputDriver);
	
	protected:

		IKeyboardDevice*		mKeyboardDevice;
		IMouseDevice*			mMouseDevice;
	};
}

#define GVK_UNDEFINED		0x00
#define GVK_ESCAPE          0x01 // DIK_ESCAPE
#define GVK_TAB             0x0F
#define GVK_SPACE           0x39
#define GVK_RETURN          0x1C
#define GVK_BACK            0x0E
#define GVK_CAPITAL         0x3A

#define GVK_MINUS           0x0C
#define GVK_EQUALS          0x0D
#define GVK_LBRACKET        0x1A
#define GVK_RBRACKET        0x1B
#define GVK_SEMICOLON       0x27
#define GVK_APOSTROPHE      0x28
#define GVK_GRAVE           0x29
#define GVK_BACKSLASH       0x2B
#define GVK_COMMA           0x33
#define GVK_PERIOD          0x34
#define GVK_SLASH           0x35

// CHARACTER KEYS
#define GVK_A               0x1E
#define GVK_S               0x1F
#define GVK_D               0x20
#define GVK_F               0x21
#define GVK_G               0x22
#define GVK_H               0x23
#define GVK_J               0x24
#define GVK_K               0x25
#define GVK_L               0x26
#define GVK_Z               0x2C
#define GVK_X               0x2D
#define GVK_C               0x2E
#define GVK_V               0x2F
#define GVK_B               0x30
#define GVK_N               0x31
#define GVK_M               0x32
#define GVK_Q               0x10
#define GVK_W               0x11
#define GVK_E               0x12
#define GVK_R               0x13
#define GVK_T               0x14
#define GVK_Y               0x15
#define GVK_U               0x16
#define GVK_I               0x17
#define GVK_O               0x18
#define GVK_P               0x19

// MAIN NUM KEYS
#define GVK_1               0x02
#define GVK_2               0x03
#define GVK_3               0x04
#define GVK_4               0x05
#define GVK_5               0x06
#define GVK_6               0x07
#define GVK_7               0x08
#define GVK_8               0x09
#define GVK_9               0x0A
#define GVK_0               0x0B

// FUNCTION KEYS
#define GVK_F1              0x3B
#define GVK_F2              0x3C
#define GVK_F3              0x3D
#define GVK_F4              0x3E
#define GVK_F5              0x3F
#define GVK_F6              0x40
#define GVK_F7              0x41
#define GVK_F8              0x42
#define GVK_F9              0x43
#define GVK_F10             0x44
#define GVK_F11             0x57
#define GVK_F12             0x58

// ON NUMPAD
#define GVK_NPPLUS          0x4E
#define GVK_NPMINUS         0x4A
#define GVK_NPDECIMAL       0x53
#define GVK_NPCOMMA         0xB3
#define GVK_NPDIVIDE        0xB5
#define GVK_NPMULTIPLY      0x37
#define GVK_NPENTER         0x9C
#define GVK_NUMLOCK         0x45
#define GVK_NP1             0x4F
#define GVK_NP2             0x50
#define GVK_NP3             0x51
#define GVK_NP4             0x4B
#define GVK_NP5             0x4C
#define GVK_NP6             0x4D
#define GVK_NP7             0x47
#define GVK_NP8             0x48
#define GVK_NP9             0x49
#define GVK_NP0             0x52

// CTRL, ALT, SHFT and WINDWS
#define GVK_RSHIFT          0x36
#define GVK_LSHIFT          0x2A
#define GVK_RCTRL           0x9D
#define GVK_LCTRL           0x1D
#define GVK_RALT            0xB8
#define GVK_LALT            0x38
#define GVK_LWIN            0xDB
#define GVK_RWIN            0xDC

// ON ARROW KEYPAD
#define GVK_UP              0xC8
#define GVK_DOWN            0xD0
#define GVK_LEFT            0xCB
#define GVK_RIGHT           0xCD
#define GVK_INSERT          0xD2
#define GVK_DELETE          0xD3
#define GVK_HOME            0xC7
#define GVK_END             0xCF
#define GVK_PGDOWN          0xD1
#define GVK_PGUP            0xC9

#define GVK_PAUSE           0xC5    /* Pause */
#define GVK_SCROLL          0x46    /* Scroll Lock */

#define GVK_LBUTTON			0x101   // mouse's left button
#define GVK_RBUTTON			0x102	// mouse's right button
#define GVK_MBUTTON			0x103   // mouse's middle button

#endif
