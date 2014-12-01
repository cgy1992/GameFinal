#include <Windows.h>
#include <GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 800;
const u32 SCREEN_HEIGHT = 600;
const f32 CAMERA_MOVE_UNIT = 5.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;

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
	IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600, EWS_NONE, true, settings);

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->createSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	//resourceGroupManager->loadResourceGroup("General");

	XMFLOAT3 vertices[4];
	vertices[0] = XMFLOAT3(-10.0f, 0.0f, 10.0f);
	vertices[1] = XMFLOAT3(10.0f, 0.0f, 10.0f);
	vertices[2] = XMFLOAT3(-10.0f, 0.0f, -10.0f);
	vertices[3] = XMFLOAT3(10.0f, 0.0f, -10.0f);

	ISimpleMesh* mesh = meshManager->createSimpleMesh("pointlist", EVF_POSITION, 
		vertices, NULL, 4, sizeof(XMFLOAT3), 0, math::SAxisAlignedBox(), false);
	IMeshNode* meshNode = smgr->addMeshNode(mesh, nullptr, nullptr);
	meshNode->setMaterialName("test/ts_material");
	meshNode->setNeedCulling(false);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f));
	char caption[200];

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

		sprintf(caption, "FPS:%f", getFps(dt));
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

