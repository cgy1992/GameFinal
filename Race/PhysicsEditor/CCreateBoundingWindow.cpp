#include "stdafx.h"
#include "CCreateBoundingWindow.h"
#include "ControlIDs.h"
#include "CEditorScene.h"
#include "CBoxBoundingWindow.h"

void CCreateBoundingWindow::Init()
{
	ComboBox_AddString(mBoundingCategoryComboBox, TEXT("Box"));
	ComboBox_SetItemData(mBoundingCategoryComboBox, 0, BOX_BOUNDING);
	ComboBox_AddString(mBoundingCategoryComboBox, TEXT("Sphere"));
	ComboBox_SetItemData(mBoundingCategoryComboBox, 1, SPHERE_BOUNDING);
	ComboBox_SetCurSel(mBoundingCategoryComboBox, 0);
}

void CCreateBoundingWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{

	CSubWindow::OnCreate(parent, xPos, yPos, width, height);

	HINSTANCE hInst = GetModuleHandle(NULL);

	mBoundingCategoryComboBox = CreateControl(TEXT("combobox"), NULL,
		WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
		10, 10, 100, 400, IDC_BOUNDING_CATEGORY_COMBO, NULL);

	mCreateButton = CreateControl(TEXT("button"), TEXT("Create"),
		WS_CHILD | WS_VISIBLE, 120, 10, 60, 24,
		(IDC_CREATE_BOUNDING_BTN), NULL);
	
	mDeleteButton = CreateControl(TEXT("button"), TEXT("Delete"),
		WS_CHILD | WS_VISIBLE, 190, 10, 60, 24,
		(IDC_DELETE_BOUNDING_BTN), NULL);

	mBoundingsList = CreateControl(TEXT("listbox"), NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
		10, 40, 220, 100,
		IDC_BOUNDINGS_LISTBOX, NULL);

	mUpdateButton = CreateControl(TEXT("button"), TEXT("Update"),
		WS_CHILD | WS_VISIBLE, 10, 150, 100, 24, IDC_BOUNDING_UPDATE_BUTTON, NULL);

	const int labelWidth = 60;
	const int textFieldWidth = 90;
	const int intervalBetweenTextFields = 2;
	HWND label1 = CreateLabel(IDC_CREATE_BOUNDING_LABELS, TEXT("Center:"), 0, 180, labelWidth);
	HWND label2 = CreateLabel(IDC_CREATE_BOUNDING_LABELS + 1, TEXT("Rotation:"), 0, 210, labelWidth);
	HWND label3 = CreateLabel(IDC_CREATE_BOUNDING_LABELS + 2, TEXT("Mass:"), 0, 240, labelWidth);
	HWND label4 = CreateLabel(IDC_CREATE_BOUNDING_LABELS + 3, TEXT("Friction:"), 180, 240, labelWidth);

	for (u32 i = 0; i < 3; i++)
	{
		mCenterTextFields[i] = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			labelWidth + i * (textFieldWidth + intervalBetweenTextFields), 180, textFieldWidth, 20,
			(IDC_BOUNDING_CENTER_TEXTFIELD + i), NULL);

		mRotTextFields[i] = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			labelWidth + i * (textFieldWidth + intervalBetweenTextFields), 210, textFieldWidth, 20,
			(IDC_BOUNDING_ROTATION_TEXTFIELD + i), NULL);
	}

	mMassTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		labelWidth, 240, textFieldWidth, 20, (IDC_BOUNDING_MASS_TEXTFIELD), NULL);

	mFrictionTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		labelWidth + 180, 
		240, textFieldWidth, 20, (IDC_BOUNDING_FRICTION_TEXTFIELD), NULL);

	mBoundingWindows[0] = new CBoxBoundingWindow();
	mBoundingWindows[1] = nullptr;

	for (u32 i = 0; i < BOUNDING_CATEGORY_COUNT; i++)
	{
		if (mBoundingWindows[i])
		{
			mBoundingWindows[i]->OnCreate(mParentHwnd, 0, 280, 300, 300);
		}
	}
}

void CCreateBoundingWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_BOUNDING_CATEGORY_COMBO && event == CBN_SELCHANGE)
		SetFocus(mParentHwnd);
	else if (id == IDC_CREATE_BOUNDING_BTN && event == BN_CLICKED)
		OnClickCreateButton();
	else if (id == IDC_DELETE_BOUNDING_BTN && event == BN_CLICKED)
		OnClickDeleteButton();
	else if (id == IDC_BOUNDINGS_LISTBOX && event == LBN_SELCHANGE)
		OnClickBoundingListItem();
	else if (id == IDC_BOUNDING_UPDATE_BUTTON && event == BN_CLICKED)
		OnClickUpdateButton();
}

void CCreateBoundingWindow::OnClickCreateButton()
{
	int selectedIndex = ComboBox_GetCurSel(mBoundingCategoryComboBox);
	if (selectedIndex == -1)
		return;

	TCHAR boundingName[128];
	SendMessage(mBoundingCategoryComboBox, CB_GETLBTEXT, selectedIndex, (LPARAM)boundingName);

	CEditorScene* scene = CEditorScene::getInstance();
	SBoxBounding* box = scene->AddBoxBounding();

	int boundingCategory = selectedIndex;
	int index = ListBox_AddString(mBoundingsList, boundingName);
	ListBox_SetItemData(mBoundingsList, index, boundingCategory);
}

void CCreateBoundingWindow::OnClickDeleteButton()
{
	int selectedIndex = ListBox_GetCurSel(mBoundingsList);
	if (selectedIndex == -1)
		return;

	CEditorScene* scene = CEditorScene::getInstance();
	scene->DeleteBounding(selectedIndex);
	ListBox_DeleteString(mBoundingsList, selectedIndex);
}

void CCreateBoundingWindow::OnClickBoundingListItem()
{
	int selectedIndex = ListBox_GetCurSel(mBoundingsList);
	if (selectedIndex != -1)
	{
		CEditorScene* scene = CEditorScene::getInstance();
		SBoundingShape* shape = scene->SelectBoundingShape(selectedIndex);

		ShowBoundingShapeInfo(shape);
	}

	SetFocus(mParentHwnd);
}

void CCreateBoundingWindow::ShowMeshInfo(SMeshNodeInfo* info)
{
	// clear item list
	int itemCount = ListBox_GetCount(mBoundingsList);
	for (int i = itemCount - 1; i >= 0; i--)
	{
		ListBox_DeleteString(mBoundingsList, i);
	}

	for (u32 i = 0; i < info->BoundingShapes.size(); i++)
	{
		SBoundingShape* shape = info->BoundingShapes[i];
		int index = ListBox_AddString(mBoundingsList, shape->getShapeName());
		ListBox_SetItemData(mBoundingsList, index, shape->getCategory());
	}

}

void CCreateBoundingWindow::ShowBoundingShapeInfo(SBoundingShape* shape)
{
	char text[256];
	sprintf_s(text, "%f", shape->Center.x);
	SetWindowTextA(mCenterTextFields[0], text);

	sprintf_s(text, "%f", shape->Center.y);
	SetWindowTextA(mCenterTextFields[1], text);

	sprintf_s(text, "%f", shape->Center.z);
	SetWindowTextA(mCenterTextFields[2], text);

	sprintf_s(text, "%f", shape->Rotation.x * 180.0f / XM_PI);
	SetWindowTextA(mRotTextFields[0], text);

	sprintf_s(text, "%f", shape->Rotation.y * 180.0f / XM_PI);
	SetWindowTextA(mRotTextFields[1], text);

	sprintf_s(text, "%f", shape->Rotation.z * 180.0f / XM_PI);
	SetWindowTextA(mRotTextFields[2], text);

	sprintf_s(text, "%f", shape->Mass);
	SetWindowTextA(mMassTextField, text);

	sprintf_s(text, "%f", shape->Friction);
	SetWindowTextA(mFrictionTextField, text);

	for (u32 i = 0; i < BOUNDING_CATEGORY_COUNT; i++)
	{
		if (mBoundingWindows[i])
		{
			if (i == shape->getCategory())
			{
				mBoundingWindows[i]->SetVisible(true);
				mBoundingWindows[i]->ShowInfo(shape);
			}
			else
			{
				mBoundingWindows[i]->SetVisible(false);
			}
		}
	}

}

void CCreateBoundingWindow::OnSize(int xPos, int yPos, int cxClient, int cyClient)
{
	CSubWindow::OnSize(xPos, yPos, cxClient, cyClient);
	for (u32 i = 0; i < BOUNDING_CATEGORY_COUNT; i++)
	{
		if (mBoundingWindows[i])
			mBoundingWindows[i]->OnSize(xPos, yPos + 280, cxClient, cyClient);
	}
}

void CCreateBoundingWindow::OnClickUpdateButton()
{
	int selectedIndex = ListBox_GetCurSel(mBoundingsList);
	if (selectedIndex == -1)
		return;

	CEditorScene* scene = CEditorScene::getInstance();
	SBoundingShape* shape = scene->GetBoundingShapeByIndex(selectedIndex);

	f32 center[3];
	f32 rotation[3];
	f32 mass, friction;

	char text[256];
	int err;

	for (u32 i = 0; i < 3; i++)
	{
		GetWindowTextA(mCenterTextFields[i], text, 256);
		err = sscanf_s(text, "%f", &center[i]);
		if (err < 1 || err == EOF)
		{
			MessageBoxA(mParentHwnd, "cannot convert center value", "Error", NULL);
			return;
		}
	}

	for (u32 i = 0; i < 3; i++)
	{
		GetWindowTextA(mRotTextFields[i], text, 256);
		err = sscanf_s(text, "%f", &rotation[i]);
		if (err < 1 || err == EOF)
		{
			MessageBoxA(mParentHwnd, "cannot convert rotation value", "Error", NULL);
			return;
		}
	}

	GetWindowTextA(mMassTextField, text, 256);
	err = sscanf_s(text, "%f", &mass);
	if (err < 1 || err == EOF)
	{
		MessageBoxA(mParentHwnd, "cannot convert mass value", "Error", NULL);
		return;
	}

	GetWindowTextA(mFrictionTextField, text, 256);
	err = sscanf_s(text, "%f", &friction);
	if (err < 1 || err == EOF)
	{
		MessageBoxA(mParentHwnd, "cannot convert friction value", "Error", NULL);
		return;
	}

	CBoundingWindow* boundingWindow = nullptr;
	for (u32 i = 0; i < BOUNDING_CATEGORY_COUNT; i++)
	{
		if (mBoundingWindows[i] && i == shape->getCategory())
			boundingWindow = mBoundingWindows[i];
	}

	if (!boundingWindow)
		return;

	if (!boundingWindow->Update(shape))
		return;

	shape->Center.x = center[0];
	shape->Center.y = center[1];
	shape->Center.z = center[2];

	shape->Rotation.x = rotation[0] * XM_PI / 180.0f;
	shape->Rotation.y = rotation[1] * XM_PI / 180.0f;
	shape->Rotation.z = rotation[2] * XM_PI / 180.0f;

	shape->Mass = mass;
	shape->Friction = friction;

	scene->UpdateWireFrameNode(shape);

}

