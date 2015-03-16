#include "stdafx.h"
#include "CInstanceInfoWindow.h"
#include "EditorWindow.h"
#include "CListNodesWindow.h"
#include "EditorScene.h"
#include "ControlIDs.h"

void CInstanceInfoWindow::Init()
{

}

void CInstanceInfoWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);
	
	mNodeNameTextField = CreateControl(TEXT("edit"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		10, 5, 150, 20, IDC_INSTANCE_NAME_TEXTFILED, NULL);

	mCreateBtn = CreateControl(TEXT("button"), TEXT("Create"),
		WS_CHILD | WS_VISIBLE,
		170, 5, 80, 26, IDC_INSTANCE_CREATE_BTN, NULL);

	mInstancesListBox = CreateControl(TEXT("listbox"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
		10, 40, 220, 350, IDC_INSTANCE_LISTBOX, NULL);

	mDeleteBtn = CreateControl(TEXT("button"), TEXT("Delete"),
		WS_CHILD | WS_VISIBLE,
		10, 400, 75, 25, IDC_INSTANCE_DELETE_BTN, NULL);
}

void CInstanceInfoWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_INSTANCE_CREATE_BTN && event == BN_CLICKED)
		OnClickCreateButton();
	else if (id == IDC_INSTANCE_LISTBOX && event == LBN_SELCHANGE)
		OnClickListItem();
	else if (id == IDC_INSTANCE_LISTBOX && event == LBN_DBLCLK)
		OnDoubleClickListItem();
	else if (id == IDC_INSTANCE_DELETE_BTN && event == BN_CLICKED)
		OnClickDeleteButton();
}

void CInstanceInfoWindow::OnClickCreateButton()
{
	EditorWindow* window = EditorWindow::getInstance();
	EditorScene* scene = EditorScene::getInstance();
	SNodeInfo* info = window->mMeshNodePanel.mCreateMeshNodeWindow.GetSelectedItemNodeInfo();
	if (info->Category != COLLECTION_CATEGORY)
		return;

	scene->PrepareAddingInstance(info->Id);
	window->SetMouseState(EMS_ADD_INSTANCE);

	EnableControl(IDC_INSTANCE_CREATE_BTN, FALSE);
}

void CInstanceInfoWindow::UpdateListBoxItems(u32 collectionId)
{
	EditorScene* scene = EditorScene::getInstance();
	std::vector<int> ids = scene->GetInstanceIdsInOneCollection(collectionId);

	int itemCount = ListBox_GetCount(mInstancesListBox);
	for (int i = itemCount - 1; i >= 0; i--)
	{
		ListBox_DeleteString(mInstancesListBox, i);
	}

	for (u32 i = 0; i < ids.size(); i++)
	{
		AddListBoxItem(ids[i]);
	}

	SetFocus(mParentHwnd);
}

void CInstanceInfoWindow::AddListBoxItem(u32 id)
{
	TCHAR text[256];

	_stprintf_s(text, TEXT("ID%d"), id);
	int itemIndex = ListBox_AddString(mInstancesListBox, text);
	ListBox_SetItemData(mInstancesListBox, itemIndex, id);

	SetFocus(mParentHwnd);
}

void CInstanceInfoWindow::OnClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mInstancesListBox);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mInstancesListBox, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();
		SNodeInfo* info = scene->GetNodeInfoById(id);
		scene->SelectObject(id);
		window->ShowNodeInfo(id);
		SetFocus(mParentHwnd);
	}
}

void CInstanceInfoWindow::OnDoubleClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mInstancesListBox);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mInstancesListBox, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();

		if (scene->SelectObject(id))
		{
			scene->BeginFocusingObject();
			window->ShowNodeInfo(id);
		}
		SetFocus(mParentHwnd);
	}
}

void CInstanceInfoWindow::SelectListItem(u32 id)
{
	u32 count = ListBox_GetCount(mInstancesListBox);
	for (u32 i = 0; i < count; i++)
	{
		u32 data = ListBox_GetItemData(mInstancesListBox, i);
		if (data == id)
		{
			ListBox_SetCurSel(mInstancesListBox, i);
			return;
		}
	}
}

void CInstanceInfoWindow::OnClickDeleteButton()
{
	int id = GetSelectedListItemId(mInstancesListBox);
	if (id == -1)
		return;

	int ret = MessageBoxA(mParentHwnd, "Are you sure to delete instance?", "Delete Confirm", MB_OKCANCEL);
	if (ret == IDCANCEL)
		return;

	EditorScene* scene = EditorScene::getInstance();
	scene->DeleteNode(id);
	scene->CancelSelectObject();

	int itemIndex = ListBox_GetCurSel(mInstancesListBox);
	if (itemIndex != LB_ERR)
	{
		ListBox_DeleteString(mInstancesListBox, itemIndex);
	}
}

