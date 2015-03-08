#ifndef __SUB_WINDOW_CLASS_H__
#define __SUB_WINDOW_CLASS_H__


class CSubWindow
{
public:
	struct SSubControl
	{
		int x, y;
		HWND hWnd;
	};

public:

	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	virtual void EnableControl(int id, BOOL enabled);
	HWND CreateLabel(int id, const char* text, int x, int y, int width);

protected:
	HWND			mParentHwnd;
	int				mPosX;
	int				mPosY;
	int				mWidth;
	int				mHeight;
};

#endif