#include "stdafx.h"
#include "EditorWindow.h"
#include "EditorScene.h"
#include "ControlIDs.h"



EditorWindow* EditorWindow::_instance = nullptr;


EditorWindow::EditorWindow(u32 clientWidth, u32 clientHeight)
:mBufferWndWidth(clientWidth),
mBufferWndHeight(clientHeight)
, mMouseState(EMS_PICKING)
{

}


// static 
LRESULT CALLBACK EditorWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return _instance->WindowsProcedure(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK EditorWindow::WindowsProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int xPos, yPos;
	static HINSTANCE hInst = GetModuleHandle(NULL);
	HWND hBufferHwnd;
	char s[100];
	IDevice* device;
	int cxClient, cyClient;
	EditorScene* scene = EditorScene::getInstance();
	switch (msg)
	{
	case WM_CREATE:
		mHwnd = hwnd;
		mLeftSubWindow = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE,
			0, 0, LEFT_SUB_WINDOW_WIDTH, 0,
			mHwnd, (HMENU)IDC_LEFT_SUB_WINDOW, hInst, NULL);
		mRightSubWindow = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE,
			0, 0, RIGHT_SUB_WINDOW_WINDTH, 0,
			mHwnd, (HMENU)IDC_RIGHT_SUB_WINDOW, hInst, NULL);
		mCreateMeshNodeWindow.OnCreate(mHwnd, mHwnd);
		break;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		OnSize(cxClient, cyClient);
		break;
	case WM_LBUTTONDOWN:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		MouseLeftButtonDown(xPos, yPos);
		break;
	case WM_RBUTTONDOWN:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		MouseRightButtonDown(xPos, yPos);
		break;
	case WM_MOUSEMOVE:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		MouseMove(xPos, yPos);
		break;
	case WM_LBUTTONDBLCLK:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		MouseDoubleClicked(xPos, yPos);
		break;
	case WM_COMMAND:
		mCreateMeshNodeWindow.OnCommand(LOWORD(wParam), HIWORD(wParam), wParam, lParam);
		break;
	}

	return 0;
}

EditorWindow* EditorWindow::getInstance()
{
	return _instance;
}

void EditorWindow::_setInstance(EditorWindow* instance)
{
	_instance = instance;
}

void EditorWindow::init()
{
	MoveWindow(mHwnd, 100, 100, 1500, 900, TRUE);
	mCreateMeshNodeWindow.Init();
}

void EditorWindow::MouseLeftButtonDown(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (mMouseState == EMS_ADD_OBJECT)
	{
		scene->AddObject();
		mMouseState = EMS_PICKING;
		mCreateMeshNodeWindow.Enable(IDC_NEW_MODEL_NDOE_BTN, TRUE);
	}
	else if (mMouseState == EMS_PICKING)
	{
		u32 sx = xPos - 200;
		u32 sy = yPos;
		scene->SelectObject(sx, sy);
	}
}

void EditorWindow::MouseRightButtonDown(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (mMouseState == EMS_ADD_OBJECT)
	{
		mCreateMeshNodeWindow.Enable(IDC_NEW_MODEL_NDOE_BTN, TRUE);
		mMouseState = EMS_PICKING;
		scene->CancelAddingObject();
	}
}

void EditorWindow::MouseMove(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	u32 sx = xPos - LEFT_SUB_WINDOW_WIDTH;
	u32 sy = yPos;
	
	if (mMouseState == EMS_ADD_OBJECT)
	{
		scene->ChangeAddedObjectPosition(sx, sy);
	}
	else if (mMouseState == EMS_PICKING)
	{
		scene->PickingObject(sx, sy);
	}

}

void EditorWindow::OnNewMeshNodeButton(HWND hwnd)
{
	
}

void EditorWindow::MouseDoubleClicked(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (mMouseState == EMS_PICKING)
	{
		u32 sx = xPos - LEFT_SUB_WINDOW_WIDTH;
		u32 sy = yPos;
		scene->SelectObject(sx, sy);
		scene->BeginFocusingObject();
	}
}

void EditorWindow::OnSize(int cxClient, int cyClient)
{
	IDevice* device = IDevice::getInstance();
	EditorScene* scene = EditorScene::getInstance();
	if (scene)
	{
		HWND hBufferHwnd = (HWND)IDevice::getInstance()->getCreationParameters().BackBufferWindowHandle;
		if (hBufferHwnd)
		{
			mBufferWndWidth = cxClient - LEFT_SUB_WINDOW_WIDTH - RIGHT_SUB_WINDOW_WINDTH;
			mBufferWndHeight = cyClient;
			scene->setRenderRegion(mBufferWndWidth, mBufferWndHeight);
			MoveWindow(hBufferHwnd, LEFT_SUB_WINDOW_WIDTH, 0, mBufferWndWidth, mBufferWndHeight, true);
		}
	}

	MoveWindow(mLeftSubWindow, 0, 0, LEFT_SUB_WINDOW_WIDTH, cyClient, TRUE);
	MoveWindow(mRightSubWindow, cxClient - RIGHT_SUB_WINDOW_WINDTH, 0, RIGHT_SUB_WINDOW_WINDTH, cyClient, TRUE);

}
