#include "stdafx.h"
#include <windowsx.h>
#include "CreateMeshNodeWindow.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "ControlIDs.h"

#define IDC_NEWBTN (IDC_NEW_MODEL_NDOE_BTN)
#define IDC_MESH_COMBO (IDC_ALL_MESH_COMBO)

void CreateMeshNodeWindow::OnCreate(HWND frameHwnd, HWND parentHwnd)
{
	mFrameHwnd = frameHwnd;
	mParentHwnd = parentHwnd;

	HINSTANCE hInst = GetModuleHandle(NULL);

	mNewButton = CreateWindow(TEXT("button"), TEXT("New"),
		WS_CHILD | WS_VISIBLE, 0, 0, 40, 20, mParentHwnd,
		(HMENU)IDC_NEWBTN, hInst, NULL);
	
	mMeshNamesComboBox = CreateWindow(TEXT("combobox"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
		10, 80, 150, 400, mParentHwnd, (HMENU)IDC_MESH_COMBO, hInst, NULL);
}

void CreateMeshNodeWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_NEWBTN && event == BN_CLICKED)
		OnNewMeshNodeButton();
}

void CreateMeshNodeWindow::OnNewMeshNodeButton()
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

void CreateMeshNodeWindow::Init()
{
	// fill mesh combo box
	IResourceGroupManager* rgmr = IResourceGroupManager::getInstance();
	std::vector<std::string> names;
	rgmr->listResourceNames(ERFT_MESH, names);
	for (u32 i = 0; i < names.size(); i++)
	{
		SendMessageA(mMeshNamesComboBox, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPCTSTR)(names[i].c_str()));
	}
}

void CreateMeshNodeWindow::Enable(int id, BOOL enabled)
{
	HWND hwnd = GetDlgItem(mFrameHwnd, id);
	EnableWindow(hwnd, enabled);
}

