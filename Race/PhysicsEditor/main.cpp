// SceneEditor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CEditorScene.h"
#include "CEditorWindow.h"

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1280;
const u32 SCREEN_HEIGHT = 960;
const f32 CAMERA_MOVE_UNIT = 30.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;
const f32 GROUND_SIZE = 300.0f;

ISceneManager* g_smgr = nullptr;

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

int _tmain(int argc, _TCHAR* argv[])
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;
	settings.WindowsProcedure = CEditorWindow::WndProc;
	settings.BackBufferWidth = 1000;
	settings.BackBufferHeight = 1000;

	CEditorWindow* window = new CEditorWindow(
		settings.BackBufferWidth,
		settings.BackBufferHeight);

	CEditorWindow::_setInstance(window);

	IApplication* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	CEditorScene* scene = new CEditorScene(device);
	CEditorScene::_setInstance(scene);


	scene->setupInitialScene();
	window->init();

	ITimer* timer = device->getTimer();
	timer->reset();
	char caption[200];
	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		f32 dt = timer->tick();
		scene->update(dt);
		scene->drawAll();

		driver->endScene();

		sprintf(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
	}

	delete scene;
	device->drop();

	return 0;
}
