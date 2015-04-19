#include "stdafx.h"
#include "LoadingScene.h"
#include "CSceneLoader.h"
#include "RaceScene.h"
#include "GameController.h"

LoadingScene::LoadingScene(IDevice* device, IVideoDriver* driver)
:Scene(device, driver)
{
	mFinishLoading = false;
}


LoadingScene::~LoadingScene()
{

}

void LoadingScene::Enter()
{
	RaceScene* scene = new RaceScene(mDevice, mVideoDriver);
	mRaceScene = scene;

	//CSceneLoader::load("main.scene", mRaceScene);
	//mFinishLoading = true;
	setupScene();

	std::thread t([this]{
		CSceneLoader::load("main.scene", mRaceScene, mPhysicalInfo);
		mFinishLoading = true;
	});
	t.detach();


	//CSceneLoader::load("main.scene", mRaceScene, mPhysicalInfo);
	//mFinishLoading = true;
}
	

void LoadingScene::Update(float dt)
{
	mSceneManager->update(dt);

	if (mFinishLoading)
	{
		GameController* game = GameController::getInstance();
		game->SwitchScene(mRaceScene);
	}
}

void LoadingScene::Render()
{
	//mSceneManager->drawAll();
	mSceneManager->draw2DImage(mLoadingImage,
		math::Rect<s32>(0, 0, mLoadingImage->getWidth(), mLoadingImage->getHeight()),
		math::Rect<s32>(0, 0, mVideoDriver->getBackBufferWidth(), mVideoDriver->getBackBufferHeight()),
		XMFLOAT4(0, 0, 0, 0),
		false);
}

void LoadingScene::Exit()
{
	mSceneManager->destroy();
	CSceneLoader::LoadPhysics(mPhysicalInfo);
}

void LoadingScene::setupScene()
{
	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	ISceneManager* smgr = mDevice->createSceneManager(aabb);
	mSceneManager = smgr;
	
	IMeshManager* meshManager = IMeshManager::getInstance();
	ISimpleMesh* cubeMesh = meshManager->createCubeMesh("debug_loading", 3.0f, 3.0f, 3.0f);
	IMeshNode* meshNode = smgr->addMeshNode(cubeMesh, nullptr, nullptr, false);
	meshNode->setMaterialName("picking_cube_material");

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 0.0f, -15.0f),
		XMFLOAT3(0, 0, 0.0f), XMFLOAT3(0, 1.0f, 0), true);

	mVideoDriver->setDeferredShading(false);

	ITextureManager* textureManager = ITextureManager::getInstance();
	mLoadingImage = textureManager->get("loading_image.jpg");
}



