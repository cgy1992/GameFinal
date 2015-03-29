#include "stdafx.h"
#include "CEditorScene.h"

CEditorScene* CEditorScene::_instance = nullptr;

CEditorScene::CEditorScene(IDevice* device)
:mDevice(device)
, mSceneManager(nullptr)
{
	mVideoDriver = device->getVideoDriver();
	mTextureManager = mVideoDriver->getTextureManager();

	mMeshManager = mVideoDriver->getMeshManager();
	mMaterialManager = mVideoDriver->getMaterialManager();

	mResourceGroupManager = mVideoDriver->getResourceGroupManager();
	mResourceGroupManager->init("Resources.cfg");

	BufferWndWidth = mVideoDriver->getBackBufferWidth();
	BufferWndHeight = mVideoDriver->getBackBufferHeight();
}


CEditorScene::~CEditorScene()
{

}

void CEditorScene::setupInitialScene()
{
	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	ISceneManager* smgr = mDevice->createSceneManager(aabb);
	mSceneManager = smgr;

	mTimer = mDevice->getTimer();
	smgr->update(0);
}

void CEditorScene::update(f32 dt)
{
	//CEditorWindow* window = CEditorWindow::getInstance();
	//if (window)
	//	window->OnKeyBoard(dt);
	mSceneManager->update(dt);
}

void CEditorScene::drawAll()
{
	mSceneManager->drawAll();
}


