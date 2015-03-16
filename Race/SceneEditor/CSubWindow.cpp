#include "stdafx.h"
#include "CSubWindow.h"
#include "EditorWindow.h"
#include "CEditorPanel.h"
#include "CMeshNodePanel.h"
#include "CLightPanel.h"

void CSubWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	mParentHwnd = parent;
	mPosX = xPos;
	mPosY = yPos;
	mWidth = width;
	mHeight = height;

	EditorWindow* window = EditorWindow::getInstance();
}

void CSubWindow::EnableControl(int id, BOOL enabled)
{
	HWND hwnd = GetDlgItem(mParentHwnd, id);
	EnableWindow(hwnd, enabled);
}

HWND CSubWindow::CreateLabel(int id, LPCTSTR text, int x, int y, int width)
{
	static HINSTANCE hInst = GetModuleHandle(NULL);

	HWND hwnd = CreateControl(TEXT("static"), text, WS_CHILDWINDOW | WS_VISIBLE, x, y, width,
		20, id, NULL);

	return hwnd;
}

void CSubWindow::OnSize(int xPos, int yPos, int cxClient, int cyClient)
{
	mPosX = xPos;
	mPosY = yPos;

	for (u32 i = 0; i < mControls.size(); i++)
	{
		SSubControl control = mControls[i];
		MoveWindow(control.hWnd, xPos + control.x, yPos + control.y, control.width, control.height, TRUE);
	}
}

void CSubWindow::AddSubControl(HWND hwnd, int x, int y, int width, int height)
{
	SSubControl control(hwnd, x, y, width, height);
	mControls.push_back(control);
}


HWND WINAPI CSubWindow::CreateControl(
	LPCTSTR lpClassName,
	LPCTSTR lpWindowName,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	int hMenu,
	LPVOID lpParam)
{
	static HINSTANCE hInst = GetModuleHandle(NULL);

	HWND hwnd = CreateWindow(lpClassName, lpWindowName, dwStyle, mPosX + x, mPosY + y, nWidth, nHeight, mParentHwnd,
		(HMENU)hMenu, hInst, NULL);

	SSubControl control(hwnd, x, y, nWidth, nHeight);
	mControls.push_back(control);
	return hwnd;
}

bool CSubWindow::IsCheckBoxSelected(HWND hwnd)
{
	LRESULT r = SendMessage(hwnd, BM_GETCHECK, 0, 0);
	if (r == BST_CHECKED)
	{
		return true;
	}
	return false;
}

bool CSubWindow::GetIntFromTextField(HWND hwnd, int& x)
{
	char text[256];
	GetWindowTextA(hwnd, text, 256);
	int err = sscanf_s(text, "%d", &x);
	if (err < 1 || err == EOF)
		return false;

	return true;
}

void CSubWindow::SetVisible(bool bVisible)
{
	int showflag = SW_SHOW;
	if (!bVisible)
		showflag = SW_HIDE;

	for (u32 i = 0; i < mControls.size(); i++)
	{
		ShowWindow(mControls[i].hWnd, showflag);
	}
}

int CSubWindow::GetSelectedListItemId(HWND hwnd)
{
	int itemIndex = ListBox_GetCurSel(hwnd);
	if (itemIndex == LB_ERR)
		return -1;

	u32 id = ListBox_GetItemData(hwnd, itemIndex);
	return id;
}

int CSubWindow::GetSelectedListItemIndex(HWND hwnd)
{
	int itemIndex = ListBox_GetCurSel(hwnd);
	return itemIndex;
}



