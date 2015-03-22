#include "stdafx.h"
#include "CLightPanel.h"
#include "EditorWindow.h"
#include "EditorScene.h"
#include "ControlIDs.h"

void CLightPanel::Init()
{
	mCreateLightNodeWindow.Init();
}

void CLightPanel::OnCreate(HWND hwnd)
{
	mCreateLightNodeWindow.OnCreate(hwnd, 0, 30, EditorWindow::LEFT_SUB_WINDOW_WIDTH, 300);
	mLightInfoWindow.OnCreate(hwnd, EditorWindow::WINDOW_INITIAL_WIDTH - EditorWindow::RIGHT_SUB_WINDOW_WIDTH,
		0, EditorWindow::RIGHT_SUB_WINDOW_WIDTH, 400);
}

void CLightPanel::SetActive(bool bActive)
{
	mCreateLightNodeWindow.SetVisible(bActive);
	mLightInfoWindow.SetVisible(bActive);

	EditorScene* scene = EditorScene::getInstance();

	if (scene)
		scene->mPointLightCollectionNode->setVisible(bActive);
}

void CLightPanel::OnCommand(WORD id, WORD event, WPARAM wParam, LPARAM lParam)
{
	mCreateLightNodeWindow.OnCommand(id, event, wParam, lParam);
	mLightInfoWindow.OnCommand(id, event, wParam, lParam);
}

void CLightPanel::OnSize(int cxClient, int cyClient)
{
	mCreateLightNodeWindow.OnSize(0, 30, cxClient, cyClient);
	mLightInfoWindow.OnSize(cxClient - EditorWindow::RIGHT_SUB_WINDOW_WIDTH, 0, cxClient, cyClient);
}

void CLightPanel::MouseMove(int sx, int sy)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;
	E_MOUSE_STATE mouseState = window->GetMouseState();
	
	if (mouseState == EMS_ADD_LIGHT)
	{
		scene->ChangeAddingLightPosition(sx, sy);
	}
	else if (mouseState == EMS_PICKING)
	{
		//scene->PickingLight(sx, sy);
	}
}

void CLightPanel::MouseLeftButtonDown(int sx, int sy)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	E_MOUSE_STATE mouseState = window->GetMouseState();

	if (mouseState == EMS_ADD_LIGHT)
	{
		char text[256];
		mCreateLightNodeWindow.GetNodeName(text);

		u32 id = scene->AddLightNode(text);
		if (id == -1)
			return;

		window->SetMouseState(EMS_PICKING);
		mCreateLightNodeWindow.EnableControl(IDC_CREATE_LIGHT_BTN, TRUE);
		mCreateLightNodeWindow.AddListItem(id);
		mCreateLightNodeWindow.SelectListItem(id);
		ShowNodeInfo(id);
	}
	else if (mouseState == EMS_PICKING)
	{
		//int id = scene->SelectObject(sx, sy);
		int id = scene->SelectLight(sx, sy);
		if (id != -1)
		{
			mCreateLightNodeWindow.SelectListItem(id);
			ShowNodeInfo(id);
		}
	}

}

void CLightPanel::MouseRightButtonDown(int xPos, int yPos)
{

}

void CLightPanel::MouseRightButtonUp(int xPos, int yPos)
{

}

void CLightPanel::MouseDoubleClicked(int sx, int sy)
{
	EditorScene* scene = EditorScene::getInstance();
	EditorWindow* window = EditorWindow::getInstance();
	if (!scene)
		return;

	E_MOUSE_STATE mouseState = window->GetMouseState();
	if (mouseState == EMS_PICKING)
	{
		int id = scene->SelectLight(sx, sy);
		if (id != -1)
		{
			scene->FocusSelectedLight();
			mCreateLightNodeWindow.SelectListItem(id);
		}
	}
}

void CLightPanel::ShowNodeInfo(u32 id)
{
	EditorScene* scene = EditorScene::getInstance();
	SLightNodeInfo* nodeinfo = scene->GetLightNodeInfoById(id);

	mLightInfoWindow.UpdateShowing(nodeinfo);
}

void CLightPanel::OnKeyBoard(f32 delta)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	bool shift = false;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		updateSelectedLightInfo(delta);
	}
	else
	{
		scene->UpdateCamera(delta);
	}
}

void CLightPanel::updateSelectedLightInfo(f32 delta)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return;

	SLightNodeInfo* info = scene->GetSelectedLightNodeInfo();
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
	f32 scaling = 0;

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
		scaling = SCALING_UNIT * delta;
	}

	if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
	{
		scaling = -SCALING_UNIT * delta;
	}

	info->Position = math::VectorAdd(info->Position, movement);
	info->Range += scaling;
	if (info->Range < 0.1f)
		info->Range = 0.1f;

	scene->UpdateLightNodeInfo(info);
	ShowNodeInfo(info->Id);
}
