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

	CSceneLoader::load("main.scene", mRaceScene);
	mFinishLoading = true;
	
	/*
	std::thread t([this]{
		CSceneLoader::load("main.scene", mRaceScene);
		mFinishLoading = true;
	});

	t.detach();
	*/
}

void LoadingScene::Update(float dt)
{
	if (mFinishLoading)
	{
		GameController* game = GameController::getInstance();
		game->SwitchScene(mRaceScene);
	}
}

void LoadingScene::Render()
{

}

void LoadingScene::Exit()
{

}


