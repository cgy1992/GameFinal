#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 10.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;

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

	// create ground

	const f32 groundSize = 100.0f;
	const f32 roomSize = 6.0f;
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", groundSize, groundSize, 10, 10, 30.0f, 30.0f);

	ISimpleMesh* roomMesh = meshManager->createCubeMesh("room", roomSize, roomSize, roomSize);
	IMeshNode* roomNode = smgr->addMeshNode(roomMesh, nullptr, nullptr, true, XMFLOAT3(0, roomSize / 2, 0));
	roomNode->setMaterialName("ground_material");

	//IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	//groundNode->setMaterialName("ground_material");


	// create sphere
	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
	ISimpleMesh* boxMesh = meshManager->createCubeMesh("box", 1.2f, 3.0f, 1.2f);
	IModelMesh* teapotMesh = meshManager->getModelMesh("teapot.mesh");

	IMeshNode* sphereNode1 = smgr->addMeshNode(sphereMesh, nullptr, nullptr, true, XMFLOAT3(-1.2f, 1, 0));
	IMeshNode* boxNode = smgr->addMeshNode(boxMesh, nullptr, nullptr, true, XMFLOAT3(1.0f, 1.5f, 1.0f));
	IMeshNode* teapot = smgr->addModelMeshNode(teapotMesh, nullptr, true, XMFLOAT3(1.2f, 0, -1.0f));
	sphereNode1->setMaterialName("sphere_material");
	boxNode->setMaterialName("box_material");
	teapot->setMaterialName("teapot_material");
//	sphereNode1->addShadow(1);
//	boxNode->addShadow(1);
//	teapot->addShadow(1);
	
	sphereNode1->addShadow(2);
	boxNode->addShadow(2);
	teapot->addShadow(2);
	

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
	light->setShadowCameraOrthographicSize(10.0f, 7.0f);
	light->enableShadow(true);

	// add point light
	ILightNode* light2 = smgr->addPointLight(2, nullptr, false, XMFLOAT3(0, 3.0f, 0), 100);
	light2->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light2->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light2->setAttenuation(1.0f, 0.1f, 0);
	light2->enableShadow(true);
	light2->setShadowMapSize(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setFarZ(3000.0f);
	
	/*
	camera->setPosition(XMFLOAT3(unit_dir.x * lightDistance, unit_dir.y * lightDistance, unit_dir.z * lightDistance));
	camera->look(XMFLOAT3(unit_dir.x, unit_dir.y, unit_dir.z));
	camera->setViewWidth(10);
	camera->setViewHeight(7);
	*/

	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		f32 dt = timer->tick();

		updateCamera(camera, dt);
		updateLightDirection(dt, light);
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
	f32 lightDistance = -20;
	XMFLOAT4 dir = light->getDirection();
	XMVECTOR dir_vec = XMLoadFloat4(&dir);
	
	//XMVECTOR axis = XMVectorSet(1.5f, 10.0f, 3.0f, 0.0f);
	XMVECTOR axis = XMVectorSet(0, 1.0f, 0, 0);
	XMMATRIX rotMat = XMMatrixRotationAxis(axis, fAngleSpeed * delta);
	dir_vec = XMVector3TransformNormal(dir_vec, rotMat);
	XMStoreFloat4(&dir, dir_vec);

	light->setDirection(XMFLOAT3(dir.x, dir.y, dir.z));
	light->setPosition(XMFLOAT3(dir.x * lightDistance, dir.y * lightDistance, dir.z * lightDistance));

}
