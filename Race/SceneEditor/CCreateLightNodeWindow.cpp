#include "stdafx.h"
#include "CCreateLightNodeWindow.h"
#include "ControlIDs.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include <tchar.h>

void CCreateLightNodeWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);
	HINSTANCE hInst = GetModuleHandle(NULL);

	mNameTextField = CreateControl(TEXT("edit"), TEXT("PointLight"),
		WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		10, 10, 150, 26, IDC_LIGHT_NAME_TEXTFIELD, NULL);

	mCreateBtn = CreateControl(TEXT("button"), TEXT("Create"),
		WS_CHILD | WS_VISIBLE,
		160, 40, 75, 26, IDC_CREATE_LIGHT_BTN, NULL);

	mLightNodesList = CreateControl(TEXT("listbox"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
		10, 130, 220, 600,
		IDC_LIGHT_NODE_LISTBOX, NULL);

	mDeleteBtn = CreateControl(TEXT("button"), TEXT("Delete"),
		WS_CHILD | WS_VISIBLE,
		10, 740, 75, 25, IDC_DELETE_LIGHT_BTN, NULL);
}

void CCreateLightNodeWindow::Init()
{

}

void CCreateLightNodeWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_CREATE_LIGHT_BTN && event == BN_CLICKED)
		OnClickCreateButton();
}

void CCreateLightNodeWindow::OnClickCreateButton()
{
	//char text[256];
	//GetWindowTextA(mNameTextField, text, 256);

	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();

	scene->PrepareAddingLight();
	window->SetMouseState(EMS_ADD_LIGHT);
	EnableControl(IDC_CREATE_LIGHT_BTN, FALSE);
}

void CCreateLightNodeWindow::AddListItem(u32 id)
{
	TCHAR text[256];
	GetWindowText(mNameTextField, text, 256);
	TCHAR szNodeName[256];
	_stprintf_s(szNodeName, TEXT("%s(%d)"), text, id);

	int itemIndex = ListBox_AddString(mLightNodesList, szNodeName);
	if (itemIndex != LB_ERR)
	{
		ListBox_SetItemData(mLightNodesList, itemIndex, id);
	}
}

