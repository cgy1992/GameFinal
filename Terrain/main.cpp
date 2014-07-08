#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 800;
const u32 SCREEN_HEIGHT = 600;
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

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, 800, 600, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	resourceGroupManager->loadResourceGroup("General");

	ILightNode* light = smgr->addLightNode(1);
	light->setType(ELT_DIRECTIONAL);
	light->setAmbient(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	//light->setPosition(2.0f, 5.0f, -3.0f);
	XMFLOAT3 lightDir = XMFLOAT3(3.0f, -2.0f, 1.5f);
	XMVECTOR dir = XMLoadFloat3(&lightDir);
	dir = XMVector3Normalize(dir);
	XMStoreFloat3(&lightDir, dir);

	light->setDirection(lightDir);
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->setAttenuation(1.0f, 0.0f, 0.0f);
	light->setRange(10000.0f);

	materialManager->destroy(std::string("test/material02"));

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 30.0f, -4.0f), XMFLOAT3(0, 30.0f, 0.0f));

	char caption[200];
	
	std::string rawFileName("heightmap.raw");
	
	ITerrainMesh* mesh = meshManager->createTerrainMesh("terrain", rawFileName,
		1.0f, 0.1f, true, false, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(mesh);
	terrainNode->setMaterialName("terrain/tessellation_material");
	
	IRenderTarget* pRenderTarget = textureManager->getRenderTarget("target1");

		
	ITimer* timer = device->getTimer();
	timer->reset();

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		float dt = timer->tick() * 0.001f;

		updateCamera(camera, dt);
		
		smgr->update(dt);
		smgr->drawAll();

		driver->endScene();

		u32 num = smgr->getRenderedMeshNum();

		sprintf(caption, "FPS:%f num:%d", getFps(dt), num);
		device->setWindowCaption(caption);
	}

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

