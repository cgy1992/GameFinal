#include "stdafx.h"

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

struct SPointLightInfo
{
	f32				RotateRadius;
	f32				AngularSpeed;
	f32				Height;
	f32				CurrentAngular;
	ILightNode*		LightNode;

	SPointLightInfo()
	{
		CurrentAngular = 0;
	}
};

SPointLightInfo g_pointLightInfo;

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

void setupSpheres(ISceneManager* smgr, f32 roomSize)
{
	IMeshManager* meshManager = IMeshManager::getInstance();
	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);

	const u32 countPerRow = 9;
	const f32 sphereRadius = 1.0f;
	const f32 sphereInterval = 1.0f;
	
	f32 startPos = -(countPerRow * sphereRadius * 2.0f + (countPerRow - 1) * sphereInterval) * 0.5f + sphereRadius;
	f32 x = startPos;

	for (u32 i = 0; i < countPerRow; i++)
	{
		f32 z = startPos;
		for (u32 j = 0; j < countPerRow; j++)
		{
			XMFLOAT3 pos(x, sphereRadius, z);
			IMeshNode* sphereNode = smgr->addMeshNode(sphereMesh, nullptr, nullptr, true, pos);
			sphereNode->setMaterialName("sphere_material");
			sphereNode->addShadow(2);
			z += sphereRadius * 2.0f + sphereInterval;
		}
		x += sphereRadius * 2.0f + sphereInterval;
	}
}

void setupScene(ISceneManager* smgr)
{
	IMeshManager* meshManager = IMeshManager::getInstance();

	// setup room
	const f32 roomSize = 30.0f;
	const f32 roomHeight = 14.0f; 
	ISimpleMesh* roomMesh = meshManager->createCubeMesh("room", roomSize, roomHeight, roomSize);
	IMeshNode* roomNode = smgr->addMeshNode(roomMesh, nullptr, nullptr, true, XMFLOAT3(0, roomHeight / 2, 0));
	roomNode->setMaterialName("wall_material");

	setupSpheres(smgr, roomSize);
}

void updatePointLights(f32 delta, SPointLightInfo& info)
{
	ILightNode* light = info.LightNode;
	XMMATRIX T = XMMatrixTranslation(info.RotateRadius, info.Height, 0);
	XMMATRIX R = XMMatrixRotationY(info.CurrentAngular);
	XMMATRIX M = T * R;
	light->setTransform(M);

	info.CurrentAngular += delta * info.AngularSpeed;
}

int _tmain(int argc, _TCHAR* argv[])
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 1;
	settings.MultiSamplingQuality = 0;
	settings.DepthBits = 32;
	settings.StencilBits = 0;

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0, 0);
	aabb.Extents = XMFLOAT3(30.0f, 30.0f, 30.0f);

	ISceneManager* smgr = device->createSceneManager(aabb);
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();
	IPipelineManager* pipelineManager = driver->getPipelineManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	setupScene(smgr);

	

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
	//light->enableShadow(true);

	// add point light
	ILightNode* light2 = smgr->addPointLight(2, nullptr, false, XMFLOAT3(2.0f, 3.0f, 0), 10.0f);
	light2->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light2->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light2->setAttenuation(0.1f, 1.2f, 0.2f);
	light2->enableShadow(true);
	light2->setShadowMapSize(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	g_pointLightInfo.AngularSpeed = 0.3f;
	g_pointLightInfo.RotateRadius = 7.0f;
	g_pointLightInfo.Height = 3.0f;
	g_pointLightInfo.LightNode = light2;

	updatePointLights(0, g_pointLightInfo);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setFarZ(500.0f);
	camera->setShadowRange(100.0f);

	smgr->setAmbient(XMFLOAT4(0.3f, 0.3f, 0.3f, 0));
	smgr->setAmbient(XMFLOAT4(0, 0, 0, 0));

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	driver->setDeferredShading(false);
	smgr->setDeferredShadingPipeline("my_deferred_pipeline");

	while (device->run())
	{
		if (GetAsyncKeyState(0x31) & 0x8000)
		{
			driver->setDeferredShading(false);
		}
		else if (GetAsyncKeyState(0x32) & 0x8000)
		{
			driver->setDeferredShading(true);
		}

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		u32 ms = timer->tick();
		float dt = ms * 0.001f;

		updateCamera(camera, dt);
		updateLightDirection(dt, light);
		//updateCarPosition(dt, carNode, camera);
		//updatePointLights(dt, g_pointLightInfo);

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


