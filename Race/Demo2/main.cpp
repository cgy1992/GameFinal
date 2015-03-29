// Demo2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CSceneLoader.h"
#include "GrassLand.h"
#include "InputHandler.h"
#include "PhysicsEngine.h"
#include "GameController.h"

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "hkBase.lib")
#pragma comment(lib, "hkVisualize.lib")
#pragma comment(lib, "hkInternal.lib")
#pragma comment(lib, "hkSerialize.lib")
#pragma comment(lib, "hkGeometryUtilities.lib")
#pragma comment(lib, "hkImageUtilities.lib")
#pragma comment(lib, "hkcdInternal.lib")
#pragma comment(lib, "hkcdCollide.lib")
#pragma comment(lib, "hkpCollide.lib")
#pragma comment(lib, "hkpConstraint.lib")
#pragma comment(lib, "hkpConstraintSolver.lib")
#pragma comment(lib, "hkpDynamics.lib")
#pragma comment(lib, "hkpInternal.lib")
#pragma comment(lib, "hkpUtilities.lib")
#pragma comment(lib, "hkpVehicle.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1280;
const u32 SCREEN_HEIGHT = 960;
const f32 CAMERA_MOVE_UNIT = 30.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;
const f32 GROUND_SIZE = 300.0f;

GrassLand* gGrassLand = nullptr;

void updateCamera(ICameraNode* camera, f32 delta);
f32 getFps(float dt);

int _tmain(int argc, _TCHAR* argv[])
{
	GameController game(SCREEN_WIDTH, SCREEN_HEIGHT);
	game.Init();
	game.Run();
	return 0;
}

f32 getFps(float dt)
{
	static u32 frameCount = 0;
	static f32 elapseTime = 0.0f;
	static f32 fps = 1.0f;
	elapseTime += dt;
	frameCount++;
	if (elapseTime > 0.1f)
	{
		fps = static_cast<f32>(frameCount) / elapseTime;
		frameCount = 0;
		elapseTime = 0.0f;
	}
	return fps;
}