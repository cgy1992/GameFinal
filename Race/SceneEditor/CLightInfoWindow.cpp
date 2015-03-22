#include "stdafx.h"
#include "CLightInfoWindow.h"
#include "ControlIDs.h"
#include "EditorScene.h"

void CLightInfoWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);

	HINSTANCE hInst = GetModuleHandle(NULL);

	HWND label1 = CreateLabel(IDC_LIGHT_INFO_LABEL, TEXT("Pos:"), 0, 10, 60);
	HWND label2 = CreateLabel(IDC_LIGHT_INFO_LABEL + 1, TEXT("Diff:"), 0, 40, 60);
	HWND label3 = CreateLabel(IDC_LIGHT_INFO_LABEL + 2, TEXT("Spec:"), 0, 70, 60);
	HWND label4 = CreateLabel(IDC_LIGHT_INFO_LABEL + 3, TEXT("Atten:"), 0, 100, 60);
	HWND label5 = CreateLabel(IDC_LIGHT_INFO_LABEL + 4, TEXT("Range:"), 0, 130, 60);

	const int textFieldWidth = 90;
	const int intervalBetweenTextFields = 2;

	for (u32 i = 0; i < 3; i++)
	{
		mPosTextFields[i] = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			60 + i * (textFieldWidth + intervalBetweenTextFields), 10, textFieldWidth, 20,
			(IDC_LIGHT_POS_TEXTFIELD + i), NULL);

		mDiffTextFields[i] = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			60 + i * (textFieldWidth + intervalBetweenTextFields), 40, textFieldWidth, 20,
			(IDC_LIGHT_DIFF_TEXTFIELD + i), NULL);

		mSpecTextFields[i] = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			60 + i * (textFieldWidth + intervalBetweenTextFields), 70, textFieldWidth, 20,
			(IDC_LIGHT_SPEC_TEXTFIELD + i), NULL);

		mAttenTextFields[i] = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			60 + i * (textFieldWidth + intervalBetweenTextFields), 100, textFieldWidth, 20,
			(IDC_LIGHT_ATTEN_TEXTFIELD + i), NULL);
	}

	mRangeTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		60, 130, textFieldWidth, 20,
		IDC_LIGHT_RANGE_TEXTFIELD, NULL);

	mUpdateBtn = CreateControl(TEXT("button"), TEXT("Update"),
		WS_CHILD | WS_VISIBLE, 10, 160, 100, 24,
		(IDC_LIGHT_UPDATE_BTN), NULL);

	mCancelBtn = CreateControl(TEXT("button"), TEXT("Cancel"),
		WS_CHILD | WS_VISIBLE, 115, 160, 100, 24,
		(IDC_LIGHT_CANCEL_BTN), NULL);

	mStaticCheckBox = CreateControl(TEXT("button"), TEXT("Static"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		10, 190, 60, 20, IDC_LIGHT_STATIC_CHECKBOX, NULL);

}

void CLightInfoWindow::UpdateShowing(SLightNodeInfo* info)
{
	char text[256];
	if (!info)
		return;

	sprintf_s(text, "%f", info->Position.x);
	SetWindowTextA(mPosTextFields[0], text);

	sprintf_s(text, "%f", info->Position.y);
	SetWindowTextA(mPosTextFields[1], text);

	sprintf_s(text, "%f", info->Position.z);
	SetWindowTextA(mPosTextFields[2], text);

	sprintf_s(text, "%f", info->Diffuse.x);
	SetWindowTextA(mDiffTextFields[0], text);

	sprintf_s(text, "%f", info->Diffuse.y);
	SetWindowTextA(mDiffTextFields[1], text);

	sprintf_s(text, "%f", info->Diffuse.z);
	SetWindowTextA(mDiffTextFields[2], text);

	sprintf_s(text, "%f", info->Specular.x);
	SetWindowTextA(mSpecTextFields[0], text);

	sprintf_s(text, "%f", info->Specular.y);
	SetWindowTextA(mSpecTextFields[1], text);

	sprintf_s(text, "%f", info->Specular.z);
	SetWindowTextA(mSpecTextFields[2], text);

	sprintf_s(text, "%f", info->Attenuations.x);
	SetWindowTextA(mAttenTextFields[0], text);

	sprintf_s(text, "%f", info->Attenuations.y);
	SetWindowTextA(mAttenTextFields[1], text);

	sprintf_s(text, "%f", info->Attenuations.z);
	SetWindowTextA(mAttenTextFields[2], text);

	sprintf_s(text, "%f", info->Range);
	SetWindowTextA(mRangeTextField, text);

	if (info->Static)
		SendMessage(mStaticCheckBox, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(mStaticCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);
}

void CLightInfoWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	if (id == IDC_LIGHT_UPDATE_BTN && event == BN_CLICKED)
		OnClickUpdateButton();
	else if (id == IDC_LIGHT_STATIC_CHECKBOX && event == BN_CLICKED)
		OnClickStaticCheckBox();
}

void CLightInfoWindow::OnClickUpdateButton()
{
	EditorScene* scene = EditorScene::getInstance();
	SLightNodeInfo* info = scene->GetSelectedLightNodeInfo();
	if (!info)
		return;

	f32 position[3];
	f32 diffuse[3];
	f32 specular[3];
	f32 attenuations[3];
	f32 range;

	char text[256];
	for (u32 i = 0; i < 3; i++)
	{
		GetWindowTextA(mPosTextFields[i], text, 256);
		int err = sscanf_s(text, "%f", &position[i]);
		if (err < 1)
		{
			MessageBoxA(mParentHwnd, "cannot convert position value", "Error", NULL);
			return;
		}
	}

	for (u32 i = 0; i < 3; i++)
	{
		GetWindowTextA(mDiffTextFields[i], text, 256);
		int err = sscanf_s(text, "%f", &diffuse[i]);
		if (err < 1)
		{
			MessageBoxA(mParentHwnd, "cannot convert diffuse value", "Error", NULL);
			return;
		}
	}

	for (u32 i = 0; i < 3; i++)
	{
		GetWindowTextA(mSpecTextFields[i], text, 256);
		int err = sscanf_s(text, "%f", &specular[i]);
		if (err < 1)
		{
			MessageBoxA(mParentHwnd, "cannot convert specular value", "Error", NULL);
			return;
		}
	}

	for (u32 i = 0; i < 3; i++)
	{
		GetWindowTextA(mAttenTextFields[i], text, 256);
		int err = sscanf_s(text, "%f", &attenuations[i]);
		if (err < 1)
		{
			MessageBoxA(mParentHwnd, "cannot convert attenuation value", "Error", NULL);
			return;
		}
	}

	GetWindowTextA(mRangeTextField, text, 256);
	int err = sscanf_s(text, "%f", &range);
	if (err < 1)
	{
		MessageBoxA(mParentHwnd, "cannot convert range value", "Error", NULL);
		return;
	}

	info->Position.x = position[0];
	info->Position.y = position[1];
	info->Position.z = position[2];
	info->Diffuse.x = diffuse[0];
	info->Diffuse.y = diffuse[1];
	info->Diffuse.z = diffuse[2];
	info->Specular.x = specular[0];
	info->Specular.y = specular[1];
	info->Specular.z = specular[2];
	info->Attenuations.x = attenuations[0];
	info->Attenuations.y = attenuations[1];
	info->Attenuations.z = attenuations[2];
	info->Range = range;
	
	scene->UpdateLightNodeInfo(info);
	UpdateShowing(info);
}

void CLightInfoWindow::OnClickCancelButton()
{

}

void CLightInfoWindow::OnClickStaticCheckBox()
{
	EditorScene* scene = EditorScene::getInstance();
	SLightNodeInfo* info = scene->GetSelectedLightNodeInfo();
	if (!info)
		return;

	if (IsCheckBoxSelected(mStaticCheckBox))
		info->Static = true;
	else
		info->Static = false;
}
