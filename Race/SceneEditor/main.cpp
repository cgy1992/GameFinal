// SceneEditor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "EditorScene.h"
#include "EditorWindow.h"

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
	SAppSettings settings;
	settings.Graphics.MultiSamplingCount = 1;
	settings.Graphics.MultiSamplingQuality = 0;
	settings.Window.WindowsProcedure = EditorWindow::WndProc;

	EditorWindow* window = new EditorWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	EditorWindow::_setInstance(window);

	IApplication* device = gf::createApp(settings);
	IVideoDriver* driver = device->getVideoDriver();
	EditorScene* scene = new EditorScene(device);
	EditorScene::_setInstance(scene);

	scene->setupInitialScene();
	window->init();

	device->setUpdateCallback([driver, scene, device](float dt)->bool{
		char caption[200];
		const float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);
		scene->update(dt);
		scene->drawAll();
		driver->endScene();
		sprintf(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
		return true;
	});

	device->run();

	delete scene;
	device->drop();

	return 0;
}
