#include "stdafx.h"
#include "EditorWindow.h"
#include "EditorScene.h"
#include <windowsx.h>

#define IDC_NEWBTN 200
#define IDC_CANCELBTN 201
#define IDC_MESH_COMBO 202

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
		mNewButton = CreateWindow(TEXT("button"), TEXT("New"),
			WS_CHILD | WS_VISIBLE, 0, 0, 40, 20, hwnd,
			(HMENU)IDC_NEWBTN, hInst, NULL);
		mCancelButton = CreateWindow(TEXT("button"), TEXT("Cancel"),
			WS_CHILD | WS_VISIBLE, 50, 0, 40, 20, hwnd,
			(HMENU)IDC_CANCELBTN, hInst, NULL);
		mMeshNamesComboBox = CreateWindow(TEXT("combobox"), NULL,
			WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
			10, 80, 150, 400, hwnd, (HMENU)IDC_MESH_COMBO, hInst, NULL);
		break;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		device = IDevice::getInstance();

		if (scene)
		{
			hBufferHwnd = (HWND)IDevice::getInstance()->getCreationParameters().BackBufferWindowHandle;
			if (hBufferHwnd)
			{
				mBufferWndWidth = cxClient - 400;
				mBufferWndHeight = cyClient;
				scene->setRenderRegion(mBufferWndWidth, mBufferWndHeight);
				MoveWindow(hBufferHwnd, 200, 0, mBufferWndWidth, mBufferWndHeight, true);
			}
		}
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
		if (LOWORD(wParam) == IDC_NEWBTN && HIWORD(wParam) == BN_CLICKED)
		{
			OnNewMeshNodeButton((HWND)lParam);
		}
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
	MoveWindow(mHwnd, 200, 100, 1500, 900, TRUE);
	// fill mesh combo box
	IResourceGroupManager* rgmr = IResourceGroupManager::getInstance();
	std::vector<std::string> names;
	rgmr->listResourceNames(ERFT_MESH, names);
	for (u32 i = 0; i < names.size(); i++)
	{
		SendMessageA(mMeshNamesComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPCTSTR)(names[i].c_str()));
	}
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
		EnableWindow(mNewButton, TRUE);
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
		EnableWindow(mNewButton, TRUE);
		mMouseState = EMS_PICKING;
		scene->CancelAddingObject();
	}
}

void EditorWindow::MouseMove(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	u32 sx = xPos - 200;
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
	EditorScene* scene = EditorScene::getInstance();
	EnableWindow(hwnd, FALSE);
	mMouseState = EMS_ADD_OBJECT;
	char szMeshName[128];
	int selectedIndex = ComboBox_GetCurSel(mMeshNamesComboBox);
	if (selectedIndex >= 0)
	{
		SendMessageA(mMeshNamesComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)szMeshName);
		if (scene)
			scene->PrepareAddingObject(szMeshName);
	}
}

void EditorWindow::MouseDoubleClicked(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (mMouseState == EMS_PICKING)
	{
		u32 sx = xPos - 200;
		u32 sy = yPos;
		scene->SelectObject(sx, sy);
		scene->BeginFocusingObject();
	}
}
