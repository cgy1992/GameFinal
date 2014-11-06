#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;
//_DEFINE_SINGLETON_INSTANCE(IMaterialManager);
//_DEFINE_SINGLETON_INSTANCE(IPipelineManager);

const u32 SCREEN_WIDTH = 1280;
const u32 SCREEN_HEIGHT = 960;
const f32 CAMERA_MOVE_UNIT = 10.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;

void updateCamera(ICameraNode* camera, f32 delta);

void updateCarPosition(f32 delta, IMeshNode* car, ICameraNode* followCamera);

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
	IPipelineManager* pipelineManager = driver->getPipelineManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");



	const f32 groundSize = 1000.0f;

	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", groundSize, groundSize, 100, 100, 100.0f, 100.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");
	
	// add point lights
	const u32 lightColNum = 10;
	const u32 lightRowNum = 10;
	f32 xLightSpace = groundSize / (lightColNum - 1);
	f32 zLightSpace = groundSize / (lightRowNum - 1);

	for (u32 i = 0; i < lightRowNum; i++)
	{
		for (u32 j = 0; j < lightColNum; j++)
		{
			u32 id = i * lightColNum + j;
			f32 x = -groundSize * 0.5f + xLightSpace * j;
			f32 z = groundSize * 0.5f - zLightSpace * i;
			ILightNode* light = smgr->addPointLight(id, nullptr, true, XMFLOAT3(x, 50.0f, z), 200.0f);
			light->setSpecular(XMFLOAT4(12.0f, 12.0f, 12.0f, 32.0f));
			light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
			light->setAttenuation(1.0f, 0.1f, 0);
		}
	}

	IModelMesh* carMesh = meshManager->getModelMesh("myfirst.mesh");
	IMeshNode* carNode = smgr->addModelMeshNode(carMesh, NULL, false);
	carNode->translate(0, 0.1f, 0);
	carNode->setNeedCulling(false);
	carNode->scale(0.05f, 0.05f, 0.05f);
	carNode->pitch(XM_PIDIV2);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f));
	camera->setFarZ(3000.0f);

	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		u32 ms = timer->tick();
		float dt = ms * 0.001f;

		updateCamera(camera, dt);
		//updateCarPosition(dt, carNode, camera);

		

		smgr->update(ms);

		smgr->drawAll();

		driver->endScene();

		sprintf_s(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
	}

	//sphereMaterial.drop();
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

void updateCarPosition(f32 delta, IMeshNode* car, ICameraNode* followCamera)
{
	XMFLOAT3 axises[3];
	car->getLocalAxis(axises);
	XMFLOAT3 xAxis = axises[0];
	XMFLOAT3 zAxis = axises[2];
	f32 movement_x = xAxis.x * delta * CAMERA_MOVE_UNIT;
	f32 movement_y = xAxis.y * delta * CAMERA_MOVE_UNIT;
	f32 movement_z = xAxis.z * delta * CAMERA_MOVE_UNIT;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		car->translate(movement_x, movement_y, movement_z);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		car->translate(-movement_x, -movement_y, -movement_z);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		car->yaw(-CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		car->yaw(CAMERA_ROTATE_UNIT * delta);
	}

	f32 fCarCameraDist = 8.0f;

	XMFLOAT3 carPos = car->getPosition();
	
	
	XMVECTOR cameraPos = XMLoadFloat3(&followCamera->getPosition());
	XMVECTOR pos = XMLoadFloat3(&carPos);
	XMVECTOR invForword = XMVectorSet(-xAxis.x, -xAxis.y, -xAxis.z, 0.0f);
	XMVECTOR forwardRight = XMVectorSet(-zAxis.x, -zAxis.y, -zAxis.z, 0.0f);

	XMMATRIX rotM = XMMatrixRotationAxis(forwardRight, XM_PI / 10);
	XMVECTOR v = XMVector3TransformNormal(invForword, rotM);
	XMVECTOR dist = XMVectorSet(fCarCameraDist, fCarCameraDist, fCarCameraDist, 0.0f);
	XMVECTOR cameraTargetPos = XMVectorMultiplyAdd(v, dist, pos);

	XMVECTOR transVec = cameraTargetPos - cameraPos;
	transVec = XMVectorScale(transVec, 0.1f);

	XMVECTOR lookTargetVec = pos + XMVectorScale(invForword, -fCarCameraDist);
	XMFLOAT3 lookTarget;
	XMStoreFloat3(&lookTarget, lookTargetVec);

	XMFLOAT3 trans;
	XMStoreFloat3(&trans, transVec);

	followCamera->lookAt(lookTarget);
	followCamera->translate(trans.x, trans.y, trans.z);
	
}