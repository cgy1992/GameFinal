#include "stdafx.h"
#include "CEditorWindow.h"
#include "CEditorScene.h"

CEditorWindow* CEditorWindow::_instance = nullptr;

const u32 CEditorWindow::RIGHT_SUB_WINDOW_WIDTH = 350;


CEditorWindow::CEditorWindow(u32 bufferWndWidth, u32 bufferWndHeight)
:mBufferWndWidth(bufferWndWidth),
mBufferWndHeight(bufferWndHeight)
{

}

LRESULT CALLBACK CEditorWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return _instance->WindowsProcedure(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK CEditorWindow::WindowsProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInst = GetModuleHandle(NULL);
	int cxClient, cyClient;
	switch (msg)
	{
	case WM_CREATE:
		OnCreate(hInst, hwnd);
		break;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		OnSize(cxClient, cyClient);
		break;
	case WM_COMMAND:
		OnCommand(LOWORD(wParam), HIWORD(wParam), wParam, lParam);
		break;
	default:
		break;
	}

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
	MoveWindow(mHwnd, 100, 30, mBufferWndWidth + RIGHT_SUB_WINDOW_WIDTH, 1000, TRUE);
	mMeshInfoWindow.Init();
	mCreateBoundingWindow.Init();
}

void CEditorWindow::OnSize(int cxClient, int cyClient)
{
	IApplication* device = IApplication::getInstance();
	CEditorScene* scene = CEditorScene::getInstance();

	if (scene)
	{
		HWND hBufferHwnd = (HWND)IApplication::getInstance()->getCreationParameters().BackBufferWindowHandle;
		if (hBufferHwnd)
		{
			mBufferWndWidth = cxClient - RIGHT_SUB_WINDOW_WIDTH;
			mBufferWndHeight = cyClient;
			//scene->setRenderRegion(mBufferWndWidth, mBufferWndHeight);
			MoveWindow(hBufferHwnd, 0, 0, mBufferWndWidth, mBufferWndHeight, true);
		}
	}

	mMeshInfoWindow.OnSize(cxClient - RIGHT_SUB_WINDOW_WIDTH, 0, cxClient, cyClient);
	mCreateBoundingWindow.OnSize(cxClient - RIGHT_SUB_WINDOW_WIDTH, 140, cxClient, cyClient);
}

void CEditorWindow::OnCreate(HINSTANCE hInst, HWND hwnd)
{
	mHwnd = hwnd;
	mMeshInfoWindow.OnCreate(mHwnd, 1000, 0, RIGHT_SUB_WINDOW_WIDTH, 200);
	mCreateBoundingWindow.OnCreate(mHwnd, 1000, 200, RIGHT_SUB_WINDOW_WIDTH, 200);
}

void CEditorWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	mMeshInfoWindow.OnCommand(id, event, wParam, lParam);
	mCreateBoundingWindow.OnCommand(id, event, wParam, lParam);
}

void CEditorWindow::ShowMeshInfo(SMeshNodeInfo* info)
{
	mCreateBoundingWindow.ShowMeshInfo(info);
}



