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
, mActivePanel(nullptr)
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
		OnCommand(LOWORD(wParam), HIWORD(wParam), wParam, lParam);
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
	ShowWindow(mHwnd, SW_MAXIMIZE);

	const char* texts[] = {
		"Model", "Light"
	};

	for (u32 i = 0; i < 2; i++)
	{
		SendMessageA(mCreatePanelSelector, CB_ADDSTRING, (WPARAM)0, (LPARAM)texts[i]);
	}

	SendMessage(mCreatePanelSelector, CB_SETCURSEL, 0, 0);

	for (u32 i = 0; i < mEditorPanelPtrs.size(); i++)
		mEditorPanelPtrs[i]->Init();

}

void EditorWindow::MouseLeftButtonDown(int xPos, int yPos)
{
	if (mActivePanel)
		mActivePanel->MouseLeftButtonDown(xPos, yPos);
}

void EditorWindow::MouseRightButtonDown(int xPos, int yPos)
{
	if (mActivePanel)
		mActivePanel->MouseRightButtonDown(xPos, yPos);
}

void EditorWindow::MouseMove(int xPos, int yPos)
{
	if (mActivePanel)
		mActivePanel->MouseMove(xPos, yPos);
}

void EditorWindow::MouseDoubleClicked(int xPos, int yPos)
{
	if (mActivePanel)
		mActivePanel->MouseDoubleClicked(xPos, yPos);
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

	for (u32 i = 0; i < mEditorPanelPtrs.size(); i++)
		mEditorPanelPtrs[i]->OnSize(cxClient, cyClient);
}

void EditorWindow::OnCreate(HINSTANCE hInst, HWND hwnd)
{
	mHwnd = hwnd;

	mCreatePanelSelector = CreateWindow(TEXT("combobox"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
		10, 10, 150, 400, mHwnd, (HMENU)IDC_CREATE_PANEL_SELECTOR, hInst, NULL);

	mMeshNodePanel.OnCreate(hwnd);

	mLightPanel.OnCreate(hwnd);
	mLightPanel.SetActive(false);

	mEditorPanelPtrs.push_back(&mMeshNodePanel);
	mEditorPanelPtrs.push_back(&mLightPanel);

	mActivePanel = &mMeshNodePanel;
}

void EditorWindow::ShowNodeInfo(u32 id)
{
	mActivePanel->ShowNodeInfo(id);
}

void EditorWindow::MouseRightButtonUp(int xPos, int yPos)
{
	if (mActivePanel)
		mActivePanel->MouseRightButtonUp(xPos, yPos);
}

void EditorWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_CREATE_PANEL_SELECTOR && event == CBN_SELCHANGE)
		OnSelectCreatePanel();

	if (mActivePanel)
		mActivePanel->OnCommand(id, event, wParam, lParam);
}

void EditorWindow::OnSelectCreatePanel()
{
	int selectedIndex = ComboBox_GetCurSel(mCreatePanelSelector);
	if (selectedIndex == -1)
		return;
	
	mActivePanel->SetActive(false);
	if (selectedIndex == 0)
	{
		mActivePanel = &mMeshNodePanel;
	}
	else if (selectedIndex == 1)
	{
		mActivePanel = &mLightPanel;
	}

	mActivePanel->SetActive(true);
	SetFocus(mHwnd);
}
