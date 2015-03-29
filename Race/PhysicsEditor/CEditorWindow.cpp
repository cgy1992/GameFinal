#include "stdafx.h"
#include "CEditorWindow.h"

CEditorWindow* CEditorWindow::_instance = nullptr;

CEditorWindow::CEditorWindow(u32 clientWidth, u32 clientHeight)
:mBufferWndWidth(clientWidth),
mBufferWndHeight(clientHeight)
{

}

LRESULT CALLBACK CEditorWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return _instance->WindowsProcedure(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK CEditorWindow::WindowsProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CEditorWindow::_setInstance(CEditorWindow* instance)
{
	_instance = instance;
}

CEditorWindow* CEditorWindow::getInstance()
{
	return _instance;
}

void CEditorWindow::init()
{

}


