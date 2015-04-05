#include "stdafx.h"
#include "GameController.h"
#include "InputHandler.h"
#include "PhysicsEngine.h"
#include "RaceScene.h"
#include "LoadingScene.h"
#include "PhysicsLoader.h"

using namespace gf;

GameController* GameController::_instance = nullptr;

GameController::GameController(u32 screenWidth, u32 screenHeight)
: mScreenWidth(screenWidth)
, mScreenHeight(screenHeight)
, mDevice(nullptr)
, mVideoDriver(nullptr)
, mTimer(nullptr)
, mCurrentScene(nullptr)
, mSwitchedScene(nullptr)
{
	_instance = this;
}

GameController::~GameController()
{
	if (mCurrentScene)
	{
		mCurrentScene->Exit();
		delete mCurrentScene;
	}

	mDevice->drop();
	PhysicsEngine::deleteInstance();
}

bool GameController::Init()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 1;
	settings.MultiSamplingQuality = 0;

	mDevice = gf::createDevice(EDT_DIRECT3D11, mScreenWidth, mScreenHeight, 
		EWS_NONE, true, settings);
	mVideoDriver = mDevice->getVideoDriver();

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	IResourceGroupManager* resourceGroupManager = mVideoDriver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	InputHandler::setKeyMapping(E_MOVE_FORWARD, 'W');
	InputHandler::setKeyMapping(E_MOVE_BACK, 'S');
	InputHandler::setKeyMapping(E_TURE_LEFT, 'A');
	InputHandler::setKeyMapping(E_TURE_RIGHT, 'D');
	InputHandler::setKeyMapping(E_MOVE_UP, 'R');
	InputHandler::setKeyMapping(E_MOVE_DOWN, 'F');
	InputHandler::setKeyMapping(E_RESET_ROTATION, 'Q');
	InputHandler::setKeyMapping(E_LOOK_LEFT, VK_LEFT);
	InputHandler::setKeyMapping(E_LOOK_RIGHT, VK_RIGHT);
	InputHandler::setKeyMapping(E_LOOK_UP, VK_UP);
	InputHandler::setKeyMapping(E_LOOK_DOWN, VK_DOWN);
	InputHandler::setKeyMapping(E_SWITCH_CAMERA, VK_LSHIFT);

	PhysicsLoader::Load("main.physics.xml");
	PhysicsEngine::initInstance();	

	mTimer = mDevice->getTimer();
	mTimer->reset();

	LoadingScene* scene = new LoadingScene(mDevice, mVideoDriver);
	mCurrentScene = scene;
	mCurrentScene->Enter();
	
	return true;
}

void GameController::Run()
{
	while (mDevice->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		mVideoDriver->beginScene(true, true, clearColor);

		f32 dt = mTimer->tick();
		if (dt > 0.03f)
			dt = 0.03f;

		InputHandler::update();

		if (mCurrentScene)
		{
			mCurrentScene->Update(dt);
			mCurrentScene->Render();
		}

		mVideoDriver->endScene();

		if (mSwitchedScene)
		{
			mCurrentScene->Exit();
			delete mCurrentScene;
			mCurrentScene = mSwitchedScene;
			mCurrentScene->Enter();
			mSwitchedScene = nullptr;
		}
	}
}

GameController* GameController::getInstance()
{
	return _instance;
}

void GameController::SwitchScene(Scene* scene)
{
	mSwitchedScene = scene;
}




