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

	mMeshNamesComboBox = CreateWindow(TEXT("combobox"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
		10, yPos + 0, 180, 400, mParentHwnd, (HMENU)IDC_MESH_COMBO, hInst, NULL);

	mStaticCheckBox = CreateWindow(TEXT("button"), TEXT("Static"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		10, yPos + 35, 60, 30, mParentHwnd, (HMENU)IDC_CREATE_STATIC_CHECK_BOX, hInst, NULL);

	mMutipleCheckBox = CreateWindow(TEXT("button"), TEXT("Multiple"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		70, yPos + 35, 70, 30, mParentHwnd, (HMENU)IDC_CREATE_STATIC_CHECK_BOX, hInst, NULL);

	mNewButton = CreateWindow(TEXT("button"), TEXT("Create"),
		WS_CHILD | WS_VISIBLE, 160, yPos + 35, 75, 26, mParentHwnd,
		(HMENU)IDC_NEWBTN, hInst, NULL);
}

void CCreateMeshNodeWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_NEWBTN && event == BN_CLICKED)
		OnNewMeshNodeButton();
	else if (id == IDC_MESH_COMBO && event == CBN_SELCHANGE)
		OnSelectMesh();
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
		EnableWindow(mNewButton, FALSE);
		window->SetMouseState(EMS_ADD_OBJECT);

		if (scene)
			scene->PrepareAddingObject(szMeshName);
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

	/*
	for (u32 i = 0; i < names.size(); i++)
	{
		SendMessageA(mMeshNamesComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPCTSTR)(names[i].c_str()));
	}
	*/
}

void CCreateMeshNodeWindow::OnSelectMesh()
{
	int selectedIndex = ComboBox_GetCurSel(mMeshNamesComboBox);
	char szMeshName[128];
	if (selectedIndex >= 0)
	{
		SendMessageA(mMeshNamesComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)szMeshName);
		int len = strlen(szMeshName);
		szMeshName[len - 5] = 0;
		
		HWND nameTextField = GetDlgItem(mParentHwnd, IDC_NODE_NAME_TEXTFIELD);
		SetWindowTextA(nameTextField, szMeshName);
	}
}
