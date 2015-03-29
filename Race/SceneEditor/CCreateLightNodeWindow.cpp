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
	EditorScene* scene = EditorScene::getInstance();
	std::vector<SLightNodeInfo> nodeInfos;

	for (auto it = scene->mLightNodeInfos.begin(); it != scene->mLightNodeInfos.end(); it++)
	{
		nodeInfos.push_back(it->second);
	}

	std::sort(nodeInfos.begin(), nodeInfos.end(), [](SLightNodeInfo& a, SLightNodeInfo& b){
		return a.Id < b.Id;
	});

	char text[256];
	for (u32 i = 0; i < nodeInfos.size(); i++)
	{
		sprintf_s(text, "%s(%d)", nodeInfos[i].Name.c_str(), nodeInfos[i].Id);
		int itemIndex = SendMessageA(mLightNodesList, LB_ADDSTRING, 0, (LPARAM)(text));
		if (itemIndex != LB_ERR)
		{
			ListBox_SetItemData(mLightNodesList, itemIndex, nodeInfos[i].Id);
		}
	}
}

void CCreateLightNodeWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_CREATE_LIGHT_BTN && event == BN_CLICKED)
		OnClickCreateButton();
	else if (id == IDC_LIGHT_NODE_LISTBOX && event == LBN_SELCHANGE)
		OnClickListItem();
	else if (id == IDC_LIGHT_NODE_LISTBOX && event == LBN_DBLCLK)
		OnDoubleClickListItem();
	else if (id == IDC_DELETE_LIGHT_BTN && event == BN_CLICKED)
		OnClickDeleteButton();
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

void CCreateLightNodeWindow::SelectListItem(u32 id)
{
	u32 count = ListBox_GetCount(mLightNodesList);
	for (u32 i = 0; i < count; i++)
	{
		u32 data = ListBox_GetItemData(mLightNodesList, i);
		if (data == id)
		{
			ListBox_SetCurSel(mLightNodesList, i);
			return;
		}
	}
}

void CCreateLightNodeWindow::GetNodeName(char text[])
{
	GetWindowTextA(mNameTextField, text, 256);
}

void CCreateLightNodeWindow::OnClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mLightNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mLightNodesList, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();

		scene->SelectLight(id);
		window->ShowNodeInfo(id);
		SetFocus(mParentHwnd);
	}
}

void CCreateLightNodeWindow::OnDoubleClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mLightNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mLightNodesList, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();

		if (scene->SelectLight(id))
		{
			scene->FocusSelectedLight();
			window->ShowNodeInfo(id);
		}

		SetFocus(mParentHwnd);
	}
}

void CCreateLightNodeWindow::OnClickDeleteButton()
{
	int ret = MessageBoxA(mParentHwnd, "Are you sure to delete node?", "Delete Confirm", MB_OKCANCEL);
	if (ret == IDCANCEL)
		return;

	int id = GetSelectedItemId();

	EditorScene* scene = EditorScene::getInstance();
	if (scene->DeleteLight(id))
	{
		int itemIndex = ListBox_GetCurSel(mLightNodesList);
		if (itemIndex != LB_ERR)
		{
			ListBox_DeleteString(mLightNodesList, itemIndex);
		}
	}
}

int CCreateLightNodeWindow::GetSelectedItemId()
{
	int itemIndex = ListBox_GetCurSel(mLightNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mLightNodesList, itemIndex);
		return id;
	}
	return -1;
}

