#include "stdafx.h"
#include <windowsx.h>
#include <set>
#include "CCreateMeshNodeWindow.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "ControlIDs.h"

#define IDC_NEWBTN (IDC_NEW_MODEL_NDOE_BTN)
#define IDC_MESH_COMBO (IDC_ALL_MESH_COMBO)

void CCreateMeshNodeWindow::OnCreate(HWND parentHwnd, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parentHwnd, xPos, yPos, width, height);

	HINSTANCE hInst = GetModuleHandle(NULL);

	mNodeNameTextField = CreateControl(TEXT("edit"), NULL, 
		WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		10, 10, 180, 24, IDC_NODE_NAME_TEXTFIELD, NULL);

	mMeshNamesComboBox = CreateControl(TEXT("combobox"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
		10, 40, 180, 400, IDC_MESH_COMBO, NULL);

	mStaticCheckBox = CreateControl(TEXT("button"), TEXT("Static"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		10, 75, 60, 20, IDC_CREATE_STATIC_CHECK_BOX, NULL);

	mMutipleCheckBox = CreateControl(TEXT("button"), TEXT("Multiple"), 
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		10, 105, 70, 20, IDC_CREATE_MULTIPLE_CHECKBOX, NULL);

	mMutipleNumTextField = CreateControl(TEXT("edit"), TEXT("5000"),
		WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		90, 105, 60, 20, IDC_CREATE_MULTIPLE_NUM_TEXTFILED, NULL);
	ShowWindow(mMutipleNumTextField, SW_HIDE);

	mNewButton = CreateControl(TEXT("button"), TEXT("Create"),
		WS_CHILD | WS_VISIBLE, 
		160, 105, 75, 26, IDC_NEWBTN,NULL);

	mNodesList = CreateControl(TEXT("listbox"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
		10, 140, 220, 600,
		IDC_NODES_LIST, NULL);

	mDeleteButton = CreateControl(TEXT("button"), TEXT("Delete"),
		WS_CHILD | WS_VISIBLE,
		10, 750, 75, 25, IDC_DELETE_MESH_NODE_BTN, NULL);
}

void CCreateMeshNodeWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_NEWBTN && event == BN_CLICKED)
		OnNewMeshNodeButton();
	else if (id == IDC_MESH_COMBO && event == CBN_SELCHANGE)
		UpdateNodeNameTextField();
	else if (id == IDC_CREATE_MULTIPLE_CHECKBOX && event == BN_CLICKED)
		OnClickMultipleCheckBox();
	else if (id == IDC_NODES_LIST && event == LBN_SELCHANGE)
		OnClickListItem();
	else if (id == IDC_NODES_LIST && event == LBN_DBLCLK)
		OnDoubleClickListItem();
	else if (id == IDC_DELETE_MESH_NODE_BTN && event == BN_CLICKED)
		OnClickDeleteButton();
}

void CCreateMeshNodeWindow::OnNewMeshNodeButton()
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	char szMeshName[128];
	int selectedIndex = ComboBox_GetCurSel(mMeshNamesComboBox);
	if (selectedIndex >= 0)
	{
		SendMessageA(mMeshNamesComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)szMeshName);

		if (IsCheckBoxSelected(mMutipleCheckBox))
		{
			int maxNum;
			if (!GetIntFromTextField(mMutipleNumTextField, maxNum))
			{
				MessageBoxA(mParentHwnd, "Cannot convert maxNum into an integer", NULL, 0);
			}
			char nodeName[256];
			GetNodeName(nodeName);
			window->mMeshNodePanel.AddCollectionNode(nodeName, szMeshName, maxNum);
		}
		else
		{
			EnableWindow(mNewButton, FALSE);
			window->SetMouseState(EMS_ADD_OBJECT);

			if (scene)
				scene->PrepareAddingObject(szMeshName);
		}
	}
}

void CCreateMeshNodeWindow::Init()
{
	// fill mesh combo box
	IResourceGroupManager* rgmr = IResourceGroupManager::getInstance();
	std::vector<std::string> names;
	rgmr->listResourceNames(ERFT_MESH, names);
	std::set<std::string> nameSet;
	for (u32 i = 0; i < names.size(); i++)
	{
		nameSet.insert(names[i]);
	}

	for (auto it = nameSet.begin(); it != nameSet.end(); it++)
	{
		std::string s = *it;
		SendMessageA(mMeshNamesComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPCTSTR)(s.c_str()));
	}

	EditorScene* scene = EditorScene::getInstance();

	std::vector<SNodeInfo> nodeInfos;
	for (auto it = scene->mNodeInfos.begin(); it != scene->mNodeInfos.end(); it++)
	{
		nodeInfos.push_back(it->second);
	}

	std::sort(nodeInfos.begin(), nodeInfos.end(), [](SNodeInfo& a, SNodeInfo& b){
		return a.Id < b.Id;
	});

	for (u32 i = 0; i < nodeInfos.size(); i++)
	{
		int itemIndex = SendMessageA(mNodesList, LB_ADDSTRING, 0, (LPARAM)(nodeInfos[i].Name.c_str()));
		if (itemIndex != LB_ERR)
		{
			ListBox_SetItemData(mNodesList, itemIndex, nodeInfos[i].Id);
		}
	}
}

void CCreateMeshNodeWindow::UpdateNodeNameTextField()
{
	int selectedIndex = ComboBox_GetCurSel(mMeshNamesComboBox);
	char szMeshName[128];
	if (selectedIndex >= 0)
	{
		SendMessageA(mMeshNamesComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)szMeshName);
		int len = strlen(szMeshName);
		szMeshName[len - 5] = 0;

		if (IsCheckBoxSelected(mMutipleCheckBox))
		{
			strcat_s(szMeshName, "(Multiple)");
		}

		HWND nameTextField = GetDlgItem(mParentHwnd, IDC_NODE_NAME_TEXTFIELD);
		SetWindowTextA(nameTextField, szMeshName);
	}
}

void CCreateMeshNodeWindow::OnClickMultipleCheckBox()
{
	if (IsCheckBoxSelected(mMutipleCheckBox))
		ShowWindow(mMutipleNumTextField, SW_SHOW);
	else
		ShowWindow(mMutipleNumTextField, SW_HIDE);
	
	UpdateNodeNameTextField();
}



void CCreateMeshNodeWindow::AddListItem(u32 id)
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

void CCreateMeshNodeWindow::OnClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mNodesList, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();
		SNodeInfo* info = scene->GetNodeInfoById(id);
		if (info->Category == MESH_CATEGORY)
		{
			scene->SelectObject(id);
			window->ShowNodeInfo(id);
			window->mMeshNodePanel.mInstanceNodeWindow.SetVisible(false);
		}
		else if (info->Category == COLLECTION_CATEGORY)
		{
			window->mMeshNodePanel.mInstanceNodeWindow.SetVisible(true);
			window->mMeshNodePanel.mInstanceNodeWindow.UpdateListBoxItems(id);
		}
		SetFocus(mParentHwnd);
	}
}

void CCreateMeshNodeWindow::OnDoubleClickListItem()
{
	int itemIndex = ListBox_GetCurSel(mNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mNodesList, itemIndex);
		EditorScene* scene = EditorScene::getInstance();
		EditorWindow* window = EditorWindow::getInstance();

		if (scene->SelectObject(id))
		{
			//scene->BeginFocusingObject();
			scene->FocusSelectedObject();
			window->ShowNodeInfo(id);
		}

		SetFocus(mParentHwnd);
	}
}

void CCreateMeshNodeWindow::SelectListItem(u32 id)
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

int CCreateMeshNodeWindow::GetSelectedItemId()
{
	int itemIndex = ListBox_GetCurSel(mNodesList);
	if (itemIndex != LB_ERR)
	{
		u32 id = ListBox_GetItemData(mNodesList, itemIndex);
		return id;
	}
	return -1;
}

SNodeInfo* CCreateMeshNodeWindow::GetSelectedItemNodeInfo()
{
	int id = GetSelectedItemId();
	if (id == -1)
		return nullptr;

	EditorScene* scene = EditorScene::getInstance();
	return scene->GetNodeInfoById(id);
}

void CCreateMeshNodeWindow::OnClickDeleteButton()
{
	SNodeInfo* info = GetSelectedItemNodeInfo();
	if (!info)
		return;

	int ret = MessageBoxA(mParentHwnd, "Are you sure to delete node?", "Delete Confirm", MB_OKCANCEL);
	if (ret == IDCANCEL)
		return;

	EditorScene* scene = EditorScene::getInstance();
	scene->DeleteNode(info->Id);
	scene->CancelSelectObject();

	int itemIndex = ListBox_GetCurSel(mNodesList);
	if (itemIndex != LB_ERR)
	{
		ListBox_DeleteString(mNodesList, itemIndex);
	}

	EditorWindow* window = EditorWindow::getInstance();
	window->mMeshNodePanel.mInstanceNodeWindow.SetVisible(false);
}

void CCreateMeshNodeWindow::GetNodeName(char text[])
{
	GetWindowTextA(mNodeNameTextField, text, 256);
}

