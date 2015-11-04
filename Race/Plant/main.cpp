// Plant.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GrassLand.h"

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

//const u32 SCREEN_WIDTH = 1024;
//const u32 SCREEN_HEIGHT = 768;

const u32 SCREEN_WIDTH = 1280;
const u32 SCREEN_HEIGHT = 960;
const f32 CAMERA_MOVE_UNIT = 30.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;
const f32 GROUND_SIZE = 300.0f;

void updateCamera(ICameraNode* camera, f32 delta);
void buildGrassLand(ISceneManager* smgr);

GrassLand* g_grassLand = nullptr;

void setCaption(IDevice* device, ISceneManager* sceneManager, float dt);

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

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	ISceneManager* smgr = device->createSceneManager(aabb);
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(3.0f, -2.0f, 1.5f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));
	light->enableShadow(true);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 30.0f, -4.0f), 
		XMFLOAT3(0, 30.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setShadowRange(200.0f);

	buildGrassLand(smgr);

	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", GROUND_SIZE, GROUND_SIZE, 50, 50, 10.0f, 10.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");

//	ITextureCube* skyTexture = textureManager->loadCubeTexture("skybox1.dds");
	smgr->setSkyDome("skybox1.dds");

	IInputDriver* inputDriver = IInputDriver::getInstance();
	inputDriver->showCursor(false);

	ITimer* timer = device->getTimer();
	timer->reset();
	char caption[200];
	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		f32 dt = timer->tick();
		updateCamera(camera, dt);
		g_grassLand->update(dt);

		smgr->update(dt);
		smgr->drawAll();

		driver->endScene();

		setCaption(device, smgr, dt);
	}

	smgr->destroy();
	device->drop();

	return 0;
}

void testCreateTexture1D()
{
	ITextureManager* textureManager = ITextureManager::getInstance();
	u32 width = 256;
	std::vector<f32> data;
	data.resize(width);
	for (u32 i = 0; i < width; i++)
	{
		data[i] = i * 1.0f / width;
	}

	ITexture* texture = textureManager->createTexture1D("test_texture_1d", width, 
		ETBT_SHADER_RESOURCE, &data[0], 1, EGF_R32_FLOAT);
	
	IMaterial* material = IMaterialManager::getInstance()->get("ground_material");
	material->setTexture(0, texture);
}

void updateCamera(ICameraNode* camera, f32 delta)
{
	IInputDriver* inputDriver = IInputDriver::getInstance();
	IKeyboardDevice* keyboard = inputDriver->getKeyboard();
	IMouseDevice* mouse = inputDriver->getMouse();

	/*
	if (keyboard->isPressed(GVK_W))
		camera->walk(CAMERA_MOVE_UNIT * delta);

	if (keyboard->isPressed(GVK_S))
		camera->walk(-CAMERA_MOVE_UNIT * delta);

	if (keyboard->isPressed(GVK_A))
		camera->strafe(-CAMERA_MOVE_UNIT * delta);

	if (keyboard->isPressed(GVK_D))
		camera->strafe(CAMERA_MOVE_UNIT * delta);

	if (keyboard->isPressed(GVK_R))
		camera->fly(CAMERA_MOVE_UNIT * delta);

	if (keyboard->isPressed(GVK_F))
		camera->fly(-CAMERA_MOVE_UNIT * delta);
		*/


	if (keyboard->isPressed(GVK_UP))
		camera->pitch(-CAMERA_ROTATE_UNIT * delta);

	if (keyboard->isPressed(GVK_DOWN))
		camera->pitch(CAMERA_ROTATE_UNIT * delta);
	
	if (keyboard->isPressed(GVK_LEFT))
		camera->yaw(-CAMERA_ROTATE_UNIT * delta);

	if (keyboard->isPressed(GVK_RIGHT))
		camera->yaw(CAMERA_ROTATE_UNIT * delta);
		

	if (keyboard->keyDown(GVK_ESCAPE))
		std::cout << "Key Down K!!!!!!!!!!!!!!!!!" << std::endl;

	if (keyboard->keyUp(GVK_ESCAPE))
		std::cout << "Key Up K!!!!!!!!!!!!!!!!!" << std::endl;

	if (mouse->keyDown(GVK_LBUTTON))
		std::cout << "Left Button Down K!!!!!!!!!!!!!!!!!" << std::endl;

	if (mouse->keyUp(GVK_LBUTTON))
		std::cout << "Left Button Up K!!!!!!!!!!!!!!!!!" << std::endl;

}

void buildGrassLand(ISceneManager* smgr)
{
	IMeshManager* meshManager = IMeshManager::getInstance();

	std::string rawFileName("terrain-heightmap4.raw");
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", rawFileName,
		3.0f, 0.1f, false, true, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh);
	terrainNode->setMaterialName("terrain/terrain_material");

	f32 range = 100.0f;
	f32 grassWidth = 10.0f;
	f32 grassHeight = 10.0f;

	const u32 grassCount = 5000;
	const f32 grassY = 0;
	std::vector<XMFLOAT3> v(grassCount);
	for (u32 i = 0; i < grassCount; i++)
	{
		v[i].x = math::RandomFloat(-range, range);
		v[i].y = grassY;
		v[i].z = math::RandomFloat(-range, range);
	}

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, grassHeight * 0.5f, 0);
	aabb.Extents = XMFLOAT3(range + grassWidth * 0.5f, grassHeight, range + grassWidth * 0.5f); 

	g_grassLand = new GrassLand(smgr, terrainNode, v, range * 2, XMFLOAT2(grassWidth, grassHeight));
}

void setCaption(IDevice* device, ISceneManager* sceneManager, float dt)
{
	IInputDriver* inputDriver = IInputDriver::getInstance();
	IKeyboardDevice* keyboard = inputDriver->getKeyboard();
	IMouseDevice* mouse = inputDriver->getMouse();

	char caption[200];
	int num = sceneManager->getRenderedMeshNum();

	const char* midButtonPressed = (mouse->isPressed(GVK_MBUTTON)) ? "Middle Button Pressed!" : "Middle Button Not Pressed!";

	int x, y, z;
	mouse->getPosition(x, y, z);

	sprintf(caption, "FPS:%f num:%d %s (%d,%d,%d)", getFps(dt), num, midButtonPressed, x, y, z);
	device->setWindowCaption(caption);
}

