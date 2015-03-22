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

void CMeshNodePanel::MouseLeftButtonDown(int sx, int sy)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	E_MOUSE_STATE mouseState = window->GetMouseState();

	if (mouseState == EMS_ADD_OBJECT)
	{
		char text[256];
		mCreateMeshNodeWindow.GetNodeName(text);

		u32 id = scene->AddObject(text);
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

void CMeshNodePanel::MouseRightButtonDown(int sx, int sy)
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
		EditorScene* scene = EditorScene::getInstance();
		if (!scene)
			return;

		SNodeInfo* info = scene->GetSelectedNodeInfo();
		if (!info)
			return;

		window->SetMouseState(EMS_ROTATION);
		mRightMousePressPoint.x = sx;
		mRightMousePressPoint.y = sy;
		mOrientationBeforeRotate = info->Rotation.y;
	}
}

void CMeshNodePanel::MouseRightButtonUp(int sx, int sy)
{
	EditorWindow* window = EditorWindow::getInstance();
	window->SetMouseState(EMS_PICKING);
}

void CMeshNodePanel::MouseMove(int sx, int sy)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

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
		f32 rotation = (mRightMousePressPoint.x - sx) * 0.02f;
		f32 rotY = mOrientationBeforeRotate + rotation;

		info->Rotation.y = mOrientationBeforeRotate + rotation;

		mNodeInfoWindow.UpdateShowing(info);
		scene->UpdateNodeInfo(info);
		scene->PickingObject(sx, sy);
	}
}

void CMeshNodePanel::MouseDoubleClicked(int sx, int sy)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	E_MOUSE_STATE mouseState = window->GetMouseState();
	if (mouseState == EMS_PICKING)
	{
		int id = scene->SelectObject(sx, sy);
		if (id != -1)
		{
			scene->FocusSelectedObject();
			mCreateMeshNodeWindow.SelectListItem(id);
		}
	}
}

void CMeshNodePanel::Init()
{
	mCreateMeshNodeWindow.Init();
}

void CMeshNodePanel::AddCollectionNode(const char* nodeName, 
	const char* szMeshName, int maxNum)
{
	EditorScene* scene = EditorScene::getInstance();
	std::string meshName = szMeshName;
	int id = scene->AddCollectionNode(nodeName, meshName, maxNum);
	if (id != -1)
	{
		mCreateMeshNodeWindow.AddListItem(id);
	}
}

void CMeshNodePanel::OnKeyBoard(f32 delta)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	bool shift = false;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		updateSelectedObjectTransform(delta);
	}
	else
	{
		scene->UpdateCamera(delta); 
	}
}

void CMeshNodePanel::updateSelectedObjectTransform(f32 delta)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	SNodeInfo* info = scene->GetSelectedNodeInfo();
	if (!info)
		return;

	const f32 MOVE_UNIT = 10.0f;
	const f32 SCALING_UNIT = 5.0f;
	
	ICameraNode* camera = scene->GetCamera();
	XMFLOAT3 look = camera->getLookVector();
	XMFLOAT3 up(0, 1.0f, 0);
	XMFLOAT3 right = camera->getRightVector();

	XMVECTOR look_v = XMVectorSet(look.x, 0, look.z, 0);
	look_v = XMVector4Normalize(look_v);

	XMStoreFloat3(&look, look_v);

	XMFLOAT3 movement(0, 0, 0);
	XMFLOAT3 scaling(0, 0, 0);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		movement = math::VectorMultiply(look, delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		movement = math::VectorMultiply(look, -delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		movement = math::VectorMultiply(right, -delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		movement = math::VectorMultiply(right, delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		movement = math::VectorMultiply(up, delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		movement = math::VectorMultiply(up, -delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState(VK_ADD) & 0x8000)
	{
		scaling = XMFLOAT3(1.0f, 1.0f, 1.0f);
	}

	if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
	{
		scaling = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	}

	scaling = math::VectorMultiply(scaling, delta * SCALING_UNIT);

	info->Position = math::VectorAdd(info->Position, movement);
	info->Scaling = math::VectorAdd(info->Scaling, scaling);

	scene->UpdateNodeInfo(info);
	ShowNodeInfo(info);
}



