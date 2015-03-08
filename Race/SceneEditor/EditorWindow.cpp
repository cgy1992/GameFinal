#include "stdafx.h"
#include "EditorWindow.h"
#include "EditorScene.h"
#include "ControlIDs.h"



EditorWindow* EditorWindow::_instance = nullptr;
const u32 EditorWindow::LEFT_SUB_WINDOW_WIDTH = 240;
const u32 EditorWindow::RIGHT_SUB_WINDOW_WIDTH = 300;
const u32 EditorWindow::WINDOW_INITIAL_WIDTH = 1700;
const u32 EditorWindow::WINDOW_INITIAL_HEIGHT = 900;

EditorWindow::EditorWindow(u32 clientWidth, u32 clientHeight)
:mBufferWndWidth(clientWidth),
mBufferWndHeight(clientHeight)
, mMouseState(EMS_PICKING)
{
	for (u32 i = 0; i < 3; i++)
		mMousePressed[0] = false;
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
		
		OnCreate(hInst, hwnd);
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
	case WM_RBUTTONUP:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		MouseRightButtonUp(xPos, yPos);
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
		mListNodesWindow.OnCommand(LOWORD(wParam), HIWORD(wParam), wParam, lParam);
		mNodeInfoWindow.OnCommand(LOWORD(wParam), HIWORD(wParam), wParam, lParam);
		break;
	case WM_CTLCOLORSTATIC:
		SetTextColor((HDC)wParam, RGB(230,0,0));   // iTextColor is the RGB() color you want for the text
		SetBkColor((HDC)wParam, RGB(230, 0, 0));  // iBkColor is the color you want for the text background 
		return (LRESULT)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
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
	MoveWindow(mHwnd, 100, 100, WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT, TRUE);
	mCreateMeshNodeWindow.Init();
}

void EditorWindow::MouseLeftButtonDown(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (!CheckMouseInScene(xPos, yPos))
		return;

	if (mMouseState == EMS_ADD_OBJECT)
	{
		u32 id = scene->AddObject();
		mMouseState = EMS_PICKING;
		mCreateMeshNodeWindow.EnableControl(IDC_NEW_MODEL_NDOE_BTN, TRUE);
		mListNodesWindow.AddListItem(id);
	}
	else if (mMouseState == EMS_PICKING)
	{
		u32 sx = xPos - LEFT_SUB_WINDOW_WIDTH;
		u32 sy = yPos;
		int id = scene->SelectObject(sx, sy);
		if (id != -1)
		{
			ShowNodeInfo(id);
			mListNodesWindow.SelectListItem(id);
		}
	}
}

void EditorWindow::MouseRightButtonDown(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (mMouseState == EMS_ADD_OBJECT)
	{
		mCreateMeshNodeWindow.EnableControl(IDC_NEW_MODEL_NDOE_BTN, TRUE);
		mMouseState = EMS_PICKING;
		scene->CancelAddingObject();
	}
	else if (mMouseState == EMS_PICKING)
	{
		if (!CheckMouseInScene(xPos, yPos))
			return;

		EditorScene* scene = EditorScene::getInstance();
		if (!scene)
			return;
		
		SNodeInfo* info = scene->GetSelectedNodeInfo();
		if (!info)
			return;

		mMousePressed[E_RIGHT_MOUSE_BUTTON] = true;
		mMouseState = EMS_ROTATION;
		mRightMousePressPoint.x = xPos;
		mRightMousePressPoint.y = yPos;
		mOrientationBeforeRotate = info->Rotation.y;
	}
}

void EditorWindow::MouseMove(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (!CheckMouseInScene(xPos, yPos))
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
	else if (mMouseState == EMS_ROTATION)
	{
		SNodeInfo* info = scene->GetSelectedNodeInfo();
		if (!info)
			return;
		f32 rotation = (mRightMousePressPoint.x - xPos) * 0.02f;
		f32 rotY = mOrientationBeforeRotate + rotation;

		info->Rotation.y = mOrientationBeforeRotate + rotation;


		mNodeInfoWindow.UpdateShowing(info);
		scene->UpdateNodeInfo(info);
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

	if (!CheckMouseInScene(xPos, yPos))
		return;

	if (mMouseState == EMS_PICKING)
	{
		u32 sx = xPos - LEFT_SUB_WINDOW_WIDTH;
		u32 sy = yPos;
		int id = scene->SelectObject(sx, sy);
		if (id != -1)
		{
			scene->BeginFocusingObject();
			mListNodesWindow.SelectListItem(id);
		}
	}
}

bool EditorWindow::CheckMouseInScene(int xPos, int yPos)
{
	if (xPos < LEFT_SUB_WINDOW_WIDTH || xPos > LEFT_SUB_WINDOW_WIDTH + mBufferWndWidth)
		return false;

	if (yPos < 0 || yPos > mBufferWndHeight)
		return false;

	return true;
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
			mBufferWndWidth = cxClient - LEFT_SUB_WINDOW_WIDTH - RIGHT_SUB_WINDOW_WIDTH;
			mBufferWndHeight = cyClient;
			scene->setRenderRegion(mBufferWndWidth, mBufferWndHeight);
			MoveWindow(hBufferHwnd, LEFT_SUB_WINDOW_WIDTH, 0, mBufferWndWidth, mBufferWndHeight, true);
		}
	}
}

void EditorWindow::OnCreate(HINSTANCE hInst, HWND hwnd)
{
	mHwnd = hwnd;

	mCreatePanelSelector = CreateWindow(TEXT("combobox"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
		10, 10, 150, 400, mHwnd, (HMENU)IDC_CREATE_PANEL_SELECTOR, hInst, NULL);

	mNodeNameTextField = CreateWindow(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		10, 40, LEFT_SUB_WINDOW_WIDTH - 30, 20, mHwnd, (HMENU)IDC_NODE_NAME_TEXTFIELD, hInst, NULL);

	mCreateMeshNodeWindow.OnCreate(mHwnd, 0, 70, LEFT_SUB_WINDOW_WIDTH, 130);
	mListNodesWindow.OnCreate(mHwnd, 0, 200, LEFT_SUB_WINDOW_WIDTH, 300);
	mNodeInfoWindow.OnCreate(mHwnd, WINDOW_INITIAL_WIDTH - RIGHT_SUB_WINDOW_WIDTH, 0, RIGHT_SUB_WINDOW_WIDTH, 400);
}

void EditorWindow::ShowNodeInfo(u32 id)
{
	EditorScene* scene = EditorScene::getInstance();
	SNodeInfo* nodeinfo = scene->GetNodeInfoById(id);

	mNodeInfoWindow.UpdateShowing(nodeinfo);
}

void EditorWindow::MouseRightButtonUp(int xPos, int yPos)
{
	mMouseState = EMS_PICKING;
	mMousePressed[E_RIGHT_MOUSE_BUTTON] = false;
}
