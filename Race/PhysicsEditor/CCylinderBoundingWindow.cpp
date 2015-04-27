#include "stdafx.h"
#include "CCylinderBoundingWindow.h"
#include "ControlIDs.h"

void CCylinderBoundingWindow::OnCreate(HWND parent, 
	int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);

	const int textFieldWidth = 90;
	const int labelWidth = 60;
	
	HWND label1 = CreateLabel(IDC_CYLINDER_LABELS, TEXT("Height:"), 0, 10, labelWidth);
	mHeightTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		labelWidth, 10, textFieldWidth, 20,
		IDC_CYLINDER_HEIGHT_TEXTFIELD, NULL);

	HWND label2 = CreateLabel(IDC_CYLINDER_LABELS + 1, TEXT("Radius:"), 0, 40, labelWidth);
	mRadiusTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		labelWidth, 40, textFieldWidth, 20,
		IDC_CYLINDER_RADIUS_TEXTFIELD, NULL);
}

bool CCylinderBoundingWindow::Update(SBoundingShape* shape)
{
	f32 height, radius;
	int err;
	char text[256];
	GetWindowTextA(mHeightTextField, text, 256);
	err = sscanf_s(text, "%f", &height);
	if (err < 1 || err == EOF)
	{
		MessageBoxA(mParentHwnd, "cannot convert height value", "Error", NULL);
		return false;
	}

	GetWindowTextA(mRadiusTextField, text, 256);
	err = sscanf_s(text, "%f", &radius);
	if (err < 1 || err == EOF)
	{
		MessageBoxA(mParentHwnd, "cannot convert radius value", "Error", NULL);
		return false;
	}

	SCylinderBounding* cylinder = (SCylinderBounding*)shape;
	cylinder->Height = height;
	cylinder->Radius = radius;

	return true;
}

void CCylinderBoundingWindow::ShowInfo(SBoundingShape* shape)
{
	SCylinderBounding* cylinder = (SCylinderBounding*)shape;

	char text[256];

	sprintf_s(text, "%f", cylinder->Height);
	SetWindowTextA(mHeightTextField, text);

	sprintf_s(text, "%f", cylinder->Radius);
	SetWindowTextA(mRadiusTextField, text);
}
