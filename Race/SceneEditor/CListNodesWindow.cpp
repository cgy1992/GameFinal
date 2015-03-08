#include "stdafx.h"
#include "ControlIDs.h"
#include "CListNodesWindow.h"
#include "EditorWindow.h"
#include "EditorScene.h"

void CListNodesWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);
	HINSTANCE hInst = GetModuleHandle(NULL);

	RECT rect;
	GetWindowRect(mParentHwnd, &rect);
	mHeight = rect.bottom - rect.top - yPos;

	mNodesList = CreateWindow(TEXT("listbox"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
		xPos + 10, yPos + 10, 220, 600,
		mParentHwnd, (HMENU)IDC_NODES_LIST, hInst, NULL);
}

void CListNodesWindow::Init()
{

}

void CListNodesWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_NODES_LIST)
	{
		if (event == LBN_SELCHANGE)
		{
			OnClickListItem();
		}
		else if (event == LBN_DBLCLK)
		{
			OnDoubleClickListItem();
		}
	}
}

void CListNodesWindow::AddListItem(u32 id)
{
	TCHAR szNodeName[256];
	HWND hNodeNameTextField = GetDlgItem(mParentHwnd, IDC_NODE_NAME_TEXTFIELD);
	GetWindowText(hNodeNameTextField, szNodeName, -1);

	int itemIndex = ListBox_AddString(mNodesList, szNodeName);
	if (itemIndex != LB_ERR)
	{
		ListBox_SetItemData(mNodesList, itemIndex, id);
	}
}

void CListNodesWindow::OnClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mNodesList, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();
		scene->SelectObject(id);
		window->ShowNodeInfo(id);
		SetFocus(mParentHwnd);
	}
}

void CListNodesWindow::OnDoubleClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mNodesList, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();
		scene->SelectObject(id);
		scene->BeginFocusingObject();
		window->ShowNodeInfo(id);
		SetFocus(mParentHwnd);
	}
}

void CListNodesWindow::SelectListItem(u32 id)
{
	u32 count = ListBox_GetCount(mNodesList);
	for (u32 i = 0; i < count; i++)
	{
		u32 data = ListBox_GetItemData(mNodesList, i);
		if (data == id)
		{
			ListBox_SetCurSel(mNodesList, i);
			return;
		}
	}

}
