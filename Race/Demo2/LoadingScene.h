#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "Scene.h"

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
	bool			mFinishLoading;
	RaceScene*			mRaceScene;
};

#endif
