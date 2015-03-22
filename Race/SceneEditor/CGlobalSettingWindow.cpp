#include "stdafx.h"
#include "CGlobalSettingWindow.h"
#include "EditorScene.h"
#include "ControlIDs.h"
#include "CFileParser.h"

void CGlobalSettingWindow::Init()
{
	UpdateShowing();
	EditorScene* scene = EditorScene::getInstance();
	if (scene->mDeferredShading)
		SendMessage(mDeferShadingCheckBox, BM_SETCHECK, BST_CHECKED, 0);
}

void CGlobalSettingWindow::OnCreate(HWND parent, int xPos, int yPos, int width, int height)
{
	CSubWindow::OnCreate(parent, xPos, yPos, width, height);
	HINSTANCE hInst = GetModuleHandle(NULL);

	mSaveBtn = CreateControl(TEXT("button"), TEXT("Save"),
		WS_CHILD | WS_VISIBLE, 10, 5, 70, 24,
		(IDC_SAVE_SCENE_BTN), NULL);

	HWND label1 = CreateLabel(IDC_GLOBAL_LABEL, TEXT("Ambient:"), 90, 5, 60);
	HWND label2 = CreateLabel(IDC_GLOBAL_LABEL + 1, TEXT("Diffuse:"), 210, 5, 60);
	HWND label3 = CreateLabel(IDC_GLOBAL_LABEL + 2, TEXT("Specular:"), 330, 5, 60);

	mAmbientTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		150, 5, 60, 20, IDC_SCENE_AMBIENT_TEXTFIELD, NULL);

	mDiffuseTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		270, 5, 60, 20, IDC_SCENE_DIFFUSE_TEXTFIELD, NULL);

	mSpecularTextField = CreateControl(TEXT("edit"), NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		390, 5, 60, 20, IDC_SCENE_SPECULAR_TEXTFIELD, NULL);

	mUpdateBtn = CreateControl(TEXT("button"), TEXT("Update"),
		WS_CHILD | WS_VISIBLE, 450, 5, 60, 22, IDC_SCENE_UPDATE_BTN, NULL);

	mDeferShadingCheckBox = CreateControl(TEXT("button"), TEXT("Deferred Shading"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		5, 30, 150, 20, IDC_DEFER_SHADING_CHECKBOX, NULL);
}

void CGlobalSettingWindow::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	char text[256];
	if (id == IDC_SCENE_UPDATE_BTN && event == BN_CLICKED)
	{
		OnClickUpdateButton();
	}
	else if (id == IDC_DEFER_SHADING_CHECKBOX && event == BN_CLICKED)
	{
		OnClickDeferredShadingCheckBox();
	}
	else if (id == IDC_SAVE_SCENE_BTN && event == BN_CLICKED)
	{
		OnClickSaveButton();
	}
}

void CGlobalSettingWindow::UpdateShowing()
{
	EditorScene* scene = EditorScene::getInstance();

	char text[256];
	ILightNode* light = scene->mDirectionalLightNode;

	sprintf_s(text, "%.5f", scene->mSceneManager->getAmbient().x);
	SetWindowTextA(mAmbientTextField, text);

	sprintf_s(text, "%.5f", light->getDiffuse().x);
	SetWindowTextA(mDiffuseTextField, text);

	sprintf_s(text, "%.5f", light->getSpecular().x);
	SetWindowTextA(mSpecularTextField, text);
}

void CGlobalSettingWindow::OnClickUpdateButton()
{
	float ambient, diffuse, specular;
	char text[256];
	int err;
	GetWindowTextA(mAmbientTextField, text, 256);
	err = sscanf_s(text, "%f", &ambient);
	if (err < 1 || err == EOF)
	{
		MessageBoxA(mParentHwnd, "cannot convert ambient value", "Error", NULL);
		return;
	}

	GetWindowTextA(mDiffuseTextField, text, 256);
	err = sscanf_s(text, "%f", &diffuse);
	if (err < 1 || err == EOF)
	{
		MessageBoxA(mParentHwnd, "cannot convert diffuse value", "Error", NULL);
		return;
	}

	GetWindowTextA(mSpecularTextField, text, 256);
	err = sscanf_s(text, "%f", &specular);
	if (err < 1 || err == EOF)
	{
		MessageBoxA(mParentHwnd, "cannot convert specular value", "Error", NULL);
		return;
	}

	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	scene->mSceneManager->setAmbient(XMFLOAT4(ambient, ambient, ambient, 1.0f));
	ILightNode* light = scene->mDirectionalLightNode;
	light->setDiffuse(XMFLOAT4(diffuse, diffuse, diffuse, 1.0f));
	light->setSpecular(XMFLOAT4(specular, specular, specular, 1.0f));

	scene->UpdateGlobalLighting();
}

void CGlobalSettingWindow::OnClickDeferredShadingCheckBox()
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	if (IsCheckBoxSelected(mDeferShadingCheckBox))
		scene->mDeferredShading = true;
	else
		scene->mDeferredShading = false;
}

void CGlobalSettingWindow::OnClickSaveButton()
{
	if (CFileParser::SaveScene("main.scene"))
	{
		MessageBoxA(mParentHwnd, "Save Complete.", "Success", 0);
	}
}
