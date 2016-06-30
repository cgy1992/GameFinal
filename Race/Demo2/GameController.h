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
	f32 GetFps(f32 dt);

private:

	u32				mScreenWidth;
	u32				mScreenHeight;
	IApplication*		mDevice;
	IVideoDriver*	mVideoDriver;
	ITimer*			mTimer;
	Scene*			mCurrentScene;
	Scene*			mSwitchedScene;

	static GameController* _instance;

};

#endif
