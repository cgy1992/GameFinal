#include "stdafx.h"
#include "CLightPanel.h"
#include "EditorWindow.h"
#include "EditorScene.h"
#include "ControlIDs.h"

void CLightPanel::OnCreate(HWND hwnd)
{
	mCreateLightNodeWindow.OnCreate(hwnd, 0, 30, EditorWindow::LEFT_SUB_WINDOW_WIDTH, 300);
}

void CLightPanel::SetActive(bool bActive)
{
	mCreateLightNodeWindow.SetVisible(bActive);
	EditorScene* scene = EditorScene::getInstance();

	if (scene)
		scene->mPointLightCollectionNode->setVisible(bActive);
}

void CLightPanel::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	mCreateLightNodeWindow.OnCommand(id, event, wParam, lParam);
}

void CLightPanel::OnSize(int cxClient, int cyClient)
{
	mCreateLightNodeWindow.OnSize(0, 30, cxClient, cyClient);
}

void CLightPanel::MouseMove(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	if (!window->CheckMouseInScene(xPos, yPos))
		return;

	u32 sx = xPos - EditorWindow::LEFT_SUB_WINDOW_WIDTH;
	u32 sy = yPos;
	E_MOUSE_STATE mouseState = window->GetMouseState();
	
	if (mouseState == EMS_ADD_LIGHT)
	{
		scene->ChangeAddingLightPosition(sx, sy);
	}
}

void CLightPanel::MouseLeftButtonDown(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	if (!window->CheckMouseInScene(xPos, yPos))
		return;
	
	E_MOUSE_STATE mouseState = window->GetMouseState();

	if (mouseState == EMS_ADD_LIGHT)
	{
		u32 id = scene->AddLightNode();
		if (id == -1)
			return;

		window->SetMouseState(EMS_PICKING);
		mCreateLightNodeWindow.EnableControl(IDC_CREATE_LIGHT_BTN, TRUE);
		mCreateLightNodeWindow.AddListItem(id);
	}

}

void CLightPanel::MouseRightButtonDown(int xPos, int yPos)
{

}

void CLightPanel::MouseRightButtonUp(int xPos, int yPos)
{

}

void CLightPanel::MouseDoubleClicked(int xPos, int yPos)
{

}

void CLightPanel::ShowNodeInfo(u32 id)
{

}
