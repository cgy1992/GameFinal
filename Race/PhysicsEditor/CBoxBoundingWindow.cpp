#include "stdafx.h"
#include "CBoxBoundingWindow.h"
#include "ControlIDs.h"

bool CBoxBoundingWindow::Update(SBoundingShape* shape)
{
	f32 extends[3];
	char text[256];
	for (u32 i = 0; i < 3; i++)
	{
		GetWindowTextA(mSizeTextFields[i], text, 256);
		int err = sscanf_s(text, "%f", &extends[i]);
		if (err < 1 || err == EOF)
		{
			MessageBoxA(mParentHwnd, "cannot convert position value", "Error", NULL);
			return false;
		}
	}

	SBoxBounding* box = dynamic_cast<SBoxBounding*>(shape);
	box->Size.x = extends[0];
	box->Size.y = extends[1];
	box->Size.z = extends[2];

	return true;
}

void CBoxBoundingWindow::ShowInfo(SBoundingShape* shape)
{
	SBoxBounding* boxShape = (SBoxBounding*)shape;
	
	char text[256];

	sprintf_s(text, "%f", boxShape->Size.x);
	SetWindowTextA(mSizeTextFields[0], text);

	sprintf_s(text, "%f", boxShape->Size.y);
	SetWindowTextA(mSizeTextFields[1], text);

	sprintf_s(text, "%f", boxShape->Size.z);
	SetWindowTextA(mSizeTextFields[2], text);

}

void CBoxBoundingWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);

	HINSTANCE hInst = GetModuleHandle(NULL);

	const int textFieldWidth = 90;
	const int intervalBetweenTextFields = 2;
	const int labelWidth = 60;

	HWND label1 = CreateLabel(IDC_BOX_LABELS, TEXT("Extends:"), 0, 10, labelWidth);

	for (u32 i = 0; i < 3; i++)
	{
		mSizeTextFields[i] = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
			labelWidth + i * (textFieldWidth + intervalBetweenTextFields), 10, textFieldWidth, 20,
			(IDC_BOX_SIZE_TEXTFIELD + i), NULL);
	}

}
