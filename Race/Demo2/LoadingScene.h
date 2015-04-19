#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "Scene.h"
#include "CSceneLoader.h"

class RaceScene;

class LoadingScene : public Scene
{
public:
	LoadingScene(IDevice* device, IVideoDriver* driver);
	
	virtual void Enter();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Exit();
	~LoadingScene();

protected:
	void setupScene();

	bool						mFinishLoading;
	ISceneManager*				mSceneManager;
	RaceScene*					mRaceScene;
	SPhysicalLoadingInfo		mPhysicalInfo;
	ITexture*					mLoadingImage;
};

#endif
