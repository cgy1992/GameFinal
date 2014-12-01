#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 30.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;
const f32 OBJECT_NUM_PER_ROW = 10;
const f32 OBJECT_SIZE = 1.0f;
const f32 OBJECT_INTERVAL = 2.0f;

void updateCamera(ICameraNode* camera, f32 delta);

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

int main()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->createSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
//	resourceGroupManager->loadResourceGroup("General");

	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(3.0f, -2.0f, 1.5f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));

	ILightNode* light2 = smgr->addDirectionalLight(2, nullptr, XMFLOAT3(-2.0f, -3.0f, -2.0f));
	light2->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light2->setDiffuse(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 30.0f, -4.0f), XMFLOAT3(0, 30.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setViewWidth(300);
	camera->setViewHeight(200);

	char caption[200];
	
	std::string rawFileName("heightmap.raw");
	ITerrainMesh* mesh = meshManager->createTerrainMesh("terrain", rawFileName,
		3.0f, 0.4f, false, true, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(mesh);
	//terrainNode->setMaterialName("terrain/tessellation_material");
	terrainNode->setMaterialName("terrain/terrain_material");

	ITextureCube* skyTexture = textureManager->loadCubeTexture("Snow.dds");
	smgr->setSkyDome(skyTexture);

	
	IRenderTarget* pRenderTarget = textureManager->getRenderTarget("target1");

		
	ITimer* timer = device->getTimer();
	timer->reset();

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		u32 ms = timer->tick();
		float dt = ms * 0.001f;
		updateCamera(camera, dt);

		XMFLOAT3 camPos = camera->getPosition();
		float terrainHeight = terrainNode->getHeight(camPos.x, camPos.z, true);
		camPos.y = terrainHeight + 3.0f;
		//camera->setPosition(camPos);

		smgr->update(ms);
		smgr->drawAll();

		driver->endScene();

		u32 num = smgr->getRenderedMeshNum();

		sprintf(caption, "FPS:%f num:%d", getFps(dt), num);
		device->setWindowCaption(caption);
	}

	smgr->destroy();
	device->drop();

	return 0;
}

void updateCamera(ICameraNode* camera, f32 delta)
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		camera->walk(CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		camera->walk(-CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		camera->strafe(-CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		camera->strafe(CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		camera->fly(CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		camera->fly(-CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		camera->pitch(-CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		camera->pitch(CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		camera->yaw(-CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		camera->yaw(CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState('N') & 0x8000)
	{
		camera->roll(CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState('M') & 0x8000)
	{
		camera->roll(-CAMERA_ROTATE_UNIT * delta);
	}
}

