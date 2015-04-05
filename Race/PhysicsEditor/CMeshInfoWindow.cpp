#include "stdafx.h"
#include "CMeshInfoWindow.h"
#include "ControlIDs.h"
#include "CEditorScene.h"
#include "CEditorWindow.h"
#include "CXmlFileManager.h"

void CMeshInfoWindow::Init()
{
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

	
}

void CMeshInfoWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);

	mSaveButton = CreateControl(TEXT("button"), TEXT("Save"),
		WS_CHILD | WS_VISIBLE, 10, 10, 80, 24,
		(IDC_SAVE_BUTTON), NULL);
	
	mMeshNamesComboBox = CreateControl(TEXT("combobox"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
		10, 40, 220, 400, IDC_MESH_COMBO, NULL);

	mMeshAabbCenterLabel = CreateLabel(IDC_MESH_AABB_CENTER_LABEL, TEXT("Center:"), 10, 70, 300);
	mMeshAabbExtendLabel = CreateLabel(IDC_MESH_AABB_CENTER_LABEL, TEXT("Extend:"), 10, 95, 300);
}

void CMeshInfoWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_MESH_COMBO && event == CBN_SELCHANGE)
		OnSelectMeshNamesComboBox();
	else if (id == IDC_SAVE_BUTTON && event == BN_CLICKED)
		OnClickSaveButton();
}

void CMeshInfoWindow::OnSelectMeshNamesComboBox()
{
	int selectedIndex = ComboBox_GetCurSel(mMeshNamesComboBox);
	char szMeshName[128];
	if (selectedIndex >= 0)
	{
		SendMessageA(mMeshNamesComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)szMeshName);
		CEditorScene* scene = CEditorScene::getInstance();
		std::string meshName = szMeshName;
	 	SMeshNodeInfo* info = scene->SelectMesh(meshName);

		// show aabb information
		IMesh* mesh = info->Node->getMesh();
		math::SAxisAlignedBox aabb = mesh->getAabb();

		char text[256];
		sprintf_s(text, "Center: (%f, %f, %f)", aabb.Center.x, aabb.Center.y, aabb.Center.z);
		SetWindowTextA(mMeshAabbCenterLabel, text);

		sprintf_s(text, "Extends: (%f, %f, %f)", aabb.Extents.x, aabb.Extents.y, aabb.Extents.z);
		SetWindowTextA(mMeshAabbExtendLabel, text);

		SetFocus(mParentHwnd);

		CEditorWindow* window = CEditorWindow::getInstance();
		window->ShowMeshInfo(info);
	}
}

void CMeshInfoWindow::OnClickSaveButton()
{
	CXmlFileManager::Save("main.physics.xml");
	MessageBoxA(mParentHwnd, "Save Complete.", "Success", 0);
}
