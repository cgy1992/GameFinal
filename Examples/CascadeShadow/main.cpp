#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 10.0f;
const f32 CAMERA_ROTATE_UNIT = 0.5f;

void updateCamera(ICameraNode* camera, f32 delta);
void updateCarPosition(f32 delta, IMeshNode* car, ICameraNode* followCamera);
void updateLightDirection(f32 delta, ILightNode* light);

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

void addBoxToScene(ISceneManager* smgr, ISimpleMesh* mesh)
{
	static const f32 min_width = 0.5f;
	static const f32 max_width = 3.0f;
	static const f32 min_height = 2.0f;
	static const f32 max_height = 6.0f;
	static const f32 pos_range = 50;

	//f32 width = math::RandomFloat(min_width, max_width);
	//f32 depth = math::RandomFloat(min_width, max_width);
	f32 height = math::RandomFloat(min_height, max_height);
	f32 width = 1.0f;
	f32 depth = 1.0f;
	//width = depth = height = 1.0f;
	f32 x = math::RandomFloat(-pos_range, pos_range);
	f32 z = math::RandomFloat(-pos_range, pos_range);
	f32 y = height / 2;

	XNA::AxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(x, y, z);
	aabb.Extents = XMFLOAT3(width / 2, height / 2, width / 2);

	IMeshNode* boxNode = smgr->addMeshNode(mesh, nullptr, nullptr, true, XMFLOAT3(x, y, z), 
		XMFLOAT3(0, 0, 0), XMFLOAT3(width, height, depth));
	boxNode->setMaterialName("box_material");
	boxNode->addShadow(1);
}

void createBoxes(ISceneManager* smgr)
{
	ISimpleMesh* boxMesh = IMeshManager::getInstance()->createCubeMesh("box1", 1.0f, 1.0f, 1.0f);

	for (u32 i = 0; i < 100; i++)
	{
		addBoxToScene(smgr, boxMesh);
	}
}

int main()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 10.0f, 0);
	aabb.Extents = XMFLOAT3(50.0f, 50.0f, 50.0f);
	ISceneManager* smgr = device->createSceneManager(aabb);
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();
	IPipelineManager* pipelineManager = driver->getPipelineManager();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	// create ground
	const f32 groundSize = 100.0f;
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", groundSize, groundSize, 10, 10, 30.0f, 30.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");
	// create sphere
	
	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
	ISimpleMesh* boxMesh = meshManager->createCubeMesh("box", 1.2f, 3.0f, 1.2f);
	IModelMesh* teapotMesh = meshManager->getModelMesh("teapot.mesh");

	IMeshNode* sphereNode1 = smgr->addMeshNode(sphereMesh, nullptr, nullptr, false, XMFLOAT3(-1.2f, 1, 0));
	IMeshNode* boxNode = smgr->addMeshNode(boxMesh, nullptr, nullptr, false, XMFLOAT3(1.0f, 1.5f, 1.0f));
	IMeshNode* teapot = smgr->addModelMeshNode(teapotMesh, nullptr, false, XMFLOAT3(1.2f, 0, -1.0f));
	sphereNode1->setMaterialName("sphere_material");
	boxNode->setMaterialName("box_material");
	teapot->setMaterialName("teapot_material");
	sphereNode1->addShadow(1);
	boxNode->addShadow(1);
	teapot->addShadow(1);
	createBoxes(smgr);

	// add directional light
	XMFLOAT3 light_dir(5.0f, -5.0f, -2.0f);
	f32 lightDistance = -20;
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, light_dir);
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->setShadowCameraOrthographicSize(10.0f, 7.0f);

	XMFLOAT4 unit_dir = light->getDirection();
	light->setPosition(XMFLOAT3(unit_dir.x * lightDistance, unit_dir.y * lightDistance, unit_dir.z * lightDistance));
	light->setShadowMapSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	light->setShadowCameraOrthographicSize(100.0f, 100.0f);
	light->enableShadow(true);
	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setNearZ(1.0f);
	camera->setFarZ(1000.0f);
	camera->setShadowRange(100.0f);
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	updateLightDirection(10.0f, light);

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		f32 dt = timer->tick();
		//float dt = ms * 0.001f;

		updateCamera(camera, dt);
		//updateLightDirection(dt, light);
		//updateCarPosition(dt, carNode, camera);

		smgr->update(dt);

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

void updateLightDirection(f32 delta, ILightNode* light)
{
	f32 fAngleSpeed = 0.3f;
	//f32 lightDistance = -80;
	XMFLOAT4 dir = light->getDirection();
	XMVECTOR dir_vec = XMLoadFloat4(&dir);

	//XMVECTOR axis = XMVectorSet(1.5f, 10.0f, 3.0f, 0.0f);
	XMVECTOR axis = XMVectorSet(0, 1.0f, 0, 0);
	XMMATRIX rotMat = XMMatrixRotationAxis(axis, fAngleSpeed * delta);
	dir_vec = XMVector3TransformNormal(dir_vec, rotMat);
	XMStoreFloat4(&dir, dir_vec);

	light->setDirection(XMFLOAT3(dir.x, dir.y, dir.z));
	//light->setPosition(XMFLOAT3(dir.x * lightDistance, dir.y * lightDistance, dir.z * lightDistance));

}
