#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

using namespace std;

#include "Scene.h"

class GameController
{
public:
	GameController(u32 screenWidth, u32 screenHeight);
	bool Init();
	void Run();
	~GameController();
	static GameController* getInstance();
	void SwitchScene(Scene* scene);

private:
	u32				mScreenWidth;
	u32				mScreenHeight;
	IDevice*		mDevice;
	IVideoDriver*	mVideoDriver;
	ITimer*			mTimer;
	Scene*			mCurrentScene;
	Scene*			mSwitchedScene;

	static GameController* _instance;

};

#endif
