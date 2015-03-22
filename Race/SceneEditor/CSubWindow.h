#ifndef __SUB_WINDOW_CLASS_H__
#define __SUB_WINDOW_CLASS_H__


class CSubWindow
{
public:
	struct SSubControl
	{
		int x, y;
		int width;
		int height;
		HWND hWnd;

		SSubControl()
			:hWnd(0), x(0), y(0), width(0), height(0)
		{

		}

		SSubControl(HWND hwnd, int x, int y, int width, int height)
			:hWnd(hwnd), x(x), y(y), width(width), height(height)
		{

		}		
	};

public:
	virtual void Init(){}
	virtual void OnCreate(HWND parent, int xPos, int yPos, int width, int height);
	virtual void EnableControl(int id, BOOL enabled);
	HWND CreateLabel(int id, LPCTSTR text, int x, int y, int width);
	virtual void OnSize(int xPos, int yPos, int cxClient, int cyClient);
	virtual void OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam) = 0;
	void AddSubControl(HWND hwnd, int x, int y, int width, int height);

	static bool IsCheckBoxSelected(HWND hwnd);
	static bool GetIntFromTextField(HWND hwnd, int& x);
	static int GetSelectedListItemIndex(HWND hwnd);
	static int GetSelectedListItemId(HWND hwnd);

	void SetVisible(bool bVisible);

	HWND WINAPI CreateControl(
		 LPCTSTR lpClassName,
		 LPCTSTR lpWindowName,
		 DWORD dwStyle,
		 int x,
		 int y,
		 int nWidth,
		 int nHeight,
		 int hMenu,
		 LPVOID lpParam);

protected:
	HWND			mParentHwnd;
	int				mPosX;
	int				mPosY;
	int				mWidth;
	int				mHeight;

	std::vector<SSubControl>		mControls;

};

#endif