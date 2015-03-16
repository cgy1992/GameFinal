#include "stdafx.h"
#include "CMeshNodePanel.h"
#include "EditorWindow.h"
#include "EditorScene.h"
#include "ControlIDs.h"

void CMeshNodePanel::OnCreate(HWND hwnd)
{
	mCreateMeshNodeWindow.OnCreate(hwnd, 0, 30, EditorWindow::LEFT_SUB_WINDOW_WIDTH, 130);
	mNodeInfoWindow.OnCreate(hwnd, EditorWindow::WINDOW_INITIAL_WIDTH - EditorWindow::RIGHT_SUB_WINDOW_WIDTH,
		0, EditorWindow::RIGHT_SUB_WINDOW_WIDTH, 400);
	mInstanceNodeWindow.OnCreate(hwnd, EditorWindow::WINDOW_INITIAL_WIDTH - EditorWindow::RIGHT_SUB_WINDOW_WIDTH, 
		200, EditorWindow::RIGHT_SUB_WINDOW_WIDTH, 400);
	mInstanceNodeWindow.SetVisible(false);
}

void CMeshNodePanel::SetActive(bool bActive)
{
	mCreateMeshNodeWindow.SetVisible(bActive);
	mNodeInfoWindow.SetVisible(bActive);
	mInstanceNodeWindow.SetVisible(bActive);

	EditorScene* scene = EditorScene::getInstance();

	if (bActive)
	{
		SNodeInfo* info = mCreateMeshNodeWindow.GetSelectedItemNodeInfo();
		if (!info)
			return;

		if (info->Category == COLLECTION_CATEGORY)
			mInstanceNodeWindow.SetVisible(true);
		else {
			mInstanceNodeWindow.SetVisible(false);
			scene->SelectObject(info->Id);
		}
			
		ShowNodeInfo(info);
	}
	else
	{
		scene->CancelSelectObject();
	}
}

void CMeshNodePanel::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	mCreateMeshNodeWindow.OnCommand(id, event, wParam, lParam);
	mNodeInfoWindow.OnCommand(id, event, wParam, lParam);
	mInstanceNodeWindow.OnCommand(id, event, wParam, lParam);
}

void CMeshNodePanel::MouseLeftButtonDown(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	if (!window->CheckMouseInScene(xPos, yPos))
		return;

	E_MOUSE_STATE mouseState = window->GetMouseState();

	if (mouseState == EMS_ADD_OBJECT)
	{
		u32 id = scene->AddObject();
		window->SetMouseState(EMS_PICKING);
		mCreateMeshNodeWindow.EnableControl(IDC_NEW_MODEL_NDOE_BTN, TRUE);
		mCreateMeshNodeWindow.AddListItem(id);
		mCreateMeshNodeWindow.SelectListItem(id);
		scene->SelectObject(id);
	}
	else if (mouseState == EMS_ADD_INSTANCE)
	{
		u32 id = scene->AddInstanceNode();
		window->SetMouseState(EMS_PICKING);
		mInstanceNodeWindow.AddListBoxItem(id);
		mInstanceNodeWindow.EnableControl(IDC_INSTANCE_CREATE_BTN, TRUE);
		mInstanceNodeWindow.SelectListItem(id);
		scene->SelectObject(id);
	}
	else if (mouseState == EMS_PICKING)
	{
		u32 sx = xPos - EditorWindow::LEFT_SUB_WINDOW_WIDTH;
		u32 sy = yPos;
		int id = scene->SelectObject(sx, sy);
		if (id != -1)
		{
			SNodeInfo* info = scene->GetNodeInfoById(id);
			if (info->Category == MESH_CATEGORY)
			{
				ShowNodeInfo(id);
				mCreateMeshNodeWindow.SelectListItem(id);
				mInstanceNodeWindow.SetVisible(false);
			}
			else if (info->Category == INSTANCE_CATEGORY)
			{
				ShowNodeInfo(id);
				mCreateMeshNodeWindow.SelectListItem(info->CollectionId);
				mInstanceNodeWindow.SetVisible(true);
				mInstanceNodeWindow.UpdateListBoxItems(info->CollectionId);
				mInstanceNodeWindow.SelectListItem(id);
			}
		}
	}
}

void CMeshNodePanel::ShowNodeInfo(u32 id)
{
	EditorScene* scene = EditorScene::getInstance();
	SNodeInfo* nodeinfo = scene->GetNodeInfoById(id);

	mNodeInfoWindow.UpdateShowing(nodeinfo);
}

void CMeshNodePanel::ShowNodeInfo(SNodeInfo* info)
{
	mNodeInfoWindow.UpdateShowing(info);
}

void CMeshNodePanel::OnSize(int cxClient, int cyClient)
{
	mCreateMeshNodeWindow.OnSize(0, 30, cxClient, cyClient);
	mNodeInfoWindow.OnSize(cxClient - EditorWindow::RIGHT_SUB_WINDOW_WIDTH, 0, cxClient, cyClient);
	mInstanceNodeWindow.OnSize(cxClient - EditorWindow::RIGHT_SUB_WINDOW_WIDTH, 200, cxClient, cyClient);
}

void CMeshNodePanel::MouseRightButtonDown(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	E_MOUSE_STATE mouseState = window->GetMouseState();

	if (mouseState == EMS_ADD_OBJECT)
	{
		mCreateMeshNodeWindow.EnableControl(IDC_NEW_MODEL_NDOE_BTN, TRUE);
		window->SetMouseState(EMS_PICKING);
		scene->CancelAddingObject();
	}
	if (mouseState == EMS_ADD_INSTANCE)
	{
		mInstanceNodeWindow.EnableControl(IDC_INSTANCE_CREATE_BTN, TRUE);
		window->SetMouseState(EMS_PICKING);
		scene->CancelAddingObject();
	}
	else if (mouseState == EMS_PICKING)
	{
		if (!window->CheckMouseInScene(xPos, yPos))
			return;

		EditorScene* scene = EditorScene::getInstance();
		if (!scene)
			return;

		SNodeInfo* info = scene->GetSelectedNodeInfo();
		if (!info)
			return;

		window->SetMouseState(EMS_ROTATION);
		mRightMousePressPoint.x = xPos;
		mRightMousePressPoint.y = yPos;
		mOrientationBeforeRotate = info->Rotation.y;
	}
}

void CMeshNodePanel::MouseRightButtonUp(int xPos, int yPos)
{
	EditorWindow* window = EditorWindow::getInstance();
	window->SetMouseState(EMS_PICKING);
}

void CMeshNodePanel::MouseMove(int xPos, int yPos)
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

	if (mouseState == EMS_ADD_OBJECT || mouseState == EMS_ADD_INSTANCE)
	{
		scene->ChangeAddedObjectPosition(sx, sy);
	}
	else if (mouseState == EMS_PICKING)
	{
		scene->PickingObject(sx, sy);
	}
	else if (mouseState == EMS_ROTATION)
	{
		SNodeInfo* info = scene->GetSelectedNodeInfo();
		if (!info)
			return;
		f32 rotation = (mRightMousePressPoint.x - xPos) * 0.02f;
		f32 rotY = mOrientationBeforeRotate + rotation;

		info->Rotation.y = mOrientationBeforeRotate + rotation;

		mNodeInfoWindow.UpdateShowing(info);
		scene->UpdateNodeInfo(info);
		scene->PickingObject(sx, sy);
	}
}

void CMeshNodePanel::MouseDoubleClicked(int xPos, int yPos)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	if (!window->CheckMouseInScene(xPos, yPos))
		return;
	E_MOUSE_STATE mouseState = window->GetMouseState();
	if (mouseState == EMS_PICKING)
	{
		u32 sx = xPos - EditorWindow::LEFT_SUB_WINDOW_WIDTH;
		u32 sy = yPos;
		int id = scene->SelectObject(sx, sy);
		if (id != -1)
		{
			scene->BeginFocusingObject();
			mCreateMeshNodeWindow.SelectListItem(id);
		}
	}
}

void CMeshNodePanel::Init()
{
	mCreateMeshNodeWindow.Init();
}

void CMeshNodePanel::AddCollectionNode(const char* szMeshName, int maxNum)
{
	EditorScene* scene = EditorScene::getInstance();
	std::string meshName = szMeshName;
	int id = scene->AddCollectionNode(meshName, maxNum);
	if (id != -1)
	{
		mCreateMeshNodeWindow.AddListItem(id);
	}
}



