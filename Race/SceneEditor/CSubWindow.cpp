#include "stdafx.h"
#include "CSubWindow.h"


void CSubWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	mParentHwnd = parent;
	mPosX = xPos;
	mPosY = yPos;
	mWidth = width;
	mHeight = height;
}

void CSubWindow::EnableControl(int id, BOOL enabled)
{
	HWND hwnd = GetDlgItem(mParentHwnd, id);
	EnableWindow(hwnd, enabled);
}

HWND CSubWindow::CreateLabel(int id, const char* text, int x, int y, int width)
{
	static HINSTANCE hInst = GetModuleHandle(NULL);
	HWND hwnd = CreateWindowA("static", text, WS_CHILDWINDOW | WS_VISIBLE, x, y, width,
		20, mParentHwnd, (HMENU)id, hInst, NULL);
	return hwnd;
}
