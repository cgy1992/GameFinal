#ifndef __RACE_SCENE_H__
#define __RACE_SCENE_H__

#include "Scene.h"
#include "GrassLand.h"
#include "Vehicle.h"

class RaceScene : public Scene
{
	friend class CSceneLoader;

public:
	RaceScene(IApplication* device, IVideoDriver* driver);
	~RaceScene();
	virtual void Enter();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Exit();

private:
	void updateCamera(ICameraNode* camera, f32 delta);
	bool			mDeferredShading;
	GrassLand*		mGrassLand;
	ITerrainNode*	mTerrainNode;
	ICameraNode*	mFreeCamera;
	ICameraNode*	mFollowCamera;
	ICameraNode*	mSideCamera;
	Vehicle*		mPlayerVehicle;
	
	ITexture*					mTireTrailTexture;
	IDepthStencilSurface*		mTireTrailDepthSurface;
};


#endif