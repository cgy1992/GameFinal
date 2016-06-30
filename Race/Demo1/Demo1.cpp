// Demo1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Car.h"
#include "InputHandler.h"
#include "PhysicsEngine.h"
#include "TerrainHeightFieldShape.h"

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "hkBase.lib")
#pragma comment(lib, "hkVisualize.lib")
#pragma comment(lib, "hkInternal.lib")
#pragma comment(lib, "hkSerialize.lib")
#pragma comment(lib, "hkGeometryUtilities.lib")
#pragma comment(lib, "hkImageUtilities.lib")
#pragma comment(lib, "hkcdInternal.lib")
#pragma comment(lib, "hkcdCollide.lib")
#pragma comment(lib, "hkpCollide.lib")
#pragma comment(lib, "hkpConstraint.lib")
#pragma comment(lib, "hkpConstraintSolver.lib")
#pragma comment(lib, "hkpDynamics.lib")
#pragma comment(lib, "hkpInternal.lib")
#pragma comment(lib, "hkpUtilities.lib")
#pragma comment(lib, "hkpVehicle.lib")

#define TERRAIN_ON (1)

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 10.0f;
const f32 CAMERA_ROTATE_UNIT = 0.5f;

void updateCamera(ICameraNode* camera, f32 delta);
void updateCarPosition(f32 delta, IMeshNode* car, ICameraNode* followCamera);
void updateLightDirection(f32 delta, ILightNode* light);
void setupPhysics(ISceneManager* smgr);
void updatePhysics(f32 dt);
void buildLandscape(ISceneManager* smgr);

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

void addBoxToScene(ISceneManager* smgr, ISimpleMesh* mesh, IInstanceCollectionNode* collection)
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

	IInstanceNode* boxNode = collection->addInstance(true, XMFLOAT3(x, y, z), XMFLOAT3(0, 0, 0), XMFLOAT3(width, height, depth));
	boxNode->addShadow(1);
}

void createBoxes(ISceneManager* smgr)
{
	ISimpleMesh* boxMesh = IMeshManager::getInstance()->createCubeMesh("box1", 1.0f, 1.0f, 1.0f);
	IInstanceCollectionNode* collection = smgr->addInstanceCollectionNode(boxMesh, nullptr, 100, 0);
	for (u32 i = 0; i < 100; i++)
	{
		addBoxToScene(smgr, boxMesh, collection);
	}
	collection->setMaterialName("multi_box_material");
	collection->addShadow(1);
}

void switchCamera(ISceneManager* smgr)
{
	if (GetAsyncKeyState(0x31) & 0x8000)
	{
		smgr->setActiveCamera(1);
	}

	if (GetAsyncKeyState(0x32) & 0x8000)
	{
		smgr->setActiveCamera(2);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IApplication* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);
	ISceneManager* smgr = device->createSceneManager(aabb);
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();
	IPipelineManager* pipelineManager = driver->getPipelineManager();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	InputHandler::setKeyMapping(E_MOVE_FORWARD, 'W');
	InputHandler::setKeyMapping(E_MOVE_BACK, 'S');
	InputHandler::setKeyMapping(E_TURE_LEFT, 'A');
	InputHandler::setKeyMapping(E_TURE_RIGHT, 'D');
	InputHandler::setKeyMapping(E_MOVE_UP, 'R');
	InputHandler::setKeyMapping(E_MOVE_DOWN, 'F');
	InputHandler::setKeyMapping(E_RESET_ROTATION, 'Q');
	InputHandler::setKeyMapping(E_LOOK_LEFT, VK_LEFT);
	InputHandler::setKeyMapping(E_LOOK_RIGHT, VK_RIGHT);
	InputHandler::setKeyMapping(E_LOOK_UP, VK_UP);
	InputHandler::setKeyMapping(E_LOOK_DOWN, VK_DOWN);
	InputHandler::setKeyMapping(E_SWITCH_CAMERA, VK_LSHIFT);

	PhysicsEngine::initInstance(); 
	setupPhysics(smgr);

	// skybox
	ITextureCube* skyboxTexture = textureManager->getTextureCube("skybox1.dds");
	smgr->setSkyDome(skyboxTexture);

	// create car
	Car* car = new Car(smgr, "carA");

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
	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -10.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setNearZ(1.0f);
	camera->setFarZ(1000.0f);
	camera->setShadowRange(300.0f);
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	ICameraNode* sideCamera = smgr->addFpsCameraNode(2, nullptr);
	sideCamera->setNearZ(1.0f);
	sideCamera->setFarZ(1000.0f);
	sideCamera->setShadowRange(300.0f);

	updateLightDirection(10.0f, light);

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	const f32 fps = 1.0f / 60.0f;
	f32 elapseTime = 0;


	/*
	while (device->run())
	{
		u32 ms = timer->tick();
		if (ms > 1000 / 60)
			ms = 1000 / 60;

		float dt = ms * 0.001f;
		InputHandler::update();
		switchCamera(smgr);
		car->update(dt);

		updatePhysics(dt);

		smgr->update(dt);

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);
		smgr->drawAll();
		driver->endScene();

		sprintf_s(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
	}
	*/
	
	while (device->run())
	{
		f32 dt = timer->tick();
		if (dt > 1.0f / 60)
			dt = 1.0f / 60;
		/*
		u32 ms = timer->tick();
		if (ms > 1000 / 60)
			ms = 1000 / 60;
		float dt = ms * 0.001f;
		*/
		elapseTime += dt;

		if (elapseTime > fps)
		{
			InputHandler::update();
			switchCamera(smgr);

			//updateCamera(camera, dt);
			car->update(fps);

			updatePhysics(fps);

			smgr->update(fps);

			const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			driver->beginScene(true, true, clearColor);
			smgr->drawAll();
			driver->endScene();

			sprintf_s(caption, "FPS:%f", getFps(dt));
			device->setWindowCaption(caption);

			elapseTime -= fps;
		}
	}
	

	//sphereMaterial.drop();
	smgr->destroy();
	device->drop();

	PhysicsEngine::deleteInstance();
	return 0;
}

void updateCamera(ICameraNode* camera, f32 delta)
{
	if (InputHandler::keyPressed(E_MOVE_FORWARD))
	{
		camera->walk(CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_MOVE_BACK))
	{
		camera->walk(-CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_TURE_LEFT))
	{
		camera->strafe(-CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_TURE_RIGHT))
	{
		camera->strafe(CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_MOVE_UP))
	{
		camera->fly(CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_MOVE_DOWN))
	{
		camera->fly(-CAMERA_MOVE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_UP))
	{
		camera->pitch(-CAMERA_ROTATE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_DOWN))
	{
		camera->pitch(CAMERA_ROTATE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_LEFT))
	{
		camera->yaw(-CAMERA_ROTATE_UNIT * delta);
	}

	if (InputHandler::keyPressed(E_LOOK_RIGHT))
	{
		camera->yaw(CAMERA_ROTATE_UNIT * delta);
	}

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

void setupPhysics(ISceneManager* smgr)
{
	
	buildLandscape(smgr);

	hkpRigidBodyCinfo wallInfo;
	hkVector4 wallSize(100.0f, 3.0f, 3.0f);
	hkpBoxShape* wallShape = new hkpBoxShape(wallSize, 0);

	hkConvertCSImpl<hkFloat32> converter(hkConvertCSImpl<hkFloat32>::CT_FLIP_Z);


	wallInfo.m_shape = wallShape;
	wallInfo.m_motionType = hkpMotion::MOTION_FIXED;
	wallInfo.m_position.set(0, 3.0f, 100);
	wallInfo.m_friction = 0.5f;

	XMFLOAT3 rotation = XMFLOAT3(XM_PIDIV4 * 0.4f, XM_PIDIV4, XM_PIDIV4 * 0.8f);
	XMVECTOR quat_v = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX M1 = XMMatrixRotationQuaternion(quat_v);
	XMFLOAT4 quat;
	XMStoreFloat4(&quat, quat_v);

	//hkVector4 up(XM_PIDIV4 * 0.4f, XM_PIDIV4, XM_PIDIV4 * 1.8f);
	//up.normalize3();
	hkVector4 up(0, 1.0f, 0);
	//wallInfo.m_rotation.setFromEulerAngles(0, 0, XM_PIDIV4);
	//wallInfo.m_rotation.setAxisAngle(up, XM_PIDIV4);
	//converter.convertQuaternion(wallInfo.m_rotation);
	wallInfo.m_rotation.set(quat.x, quat.y, quat.z, quat.w);

	//converter.convertQuaternion(wallInfo.m_rotation);
	
	//XMFLOAT3 rotation = XMFLOAT3(XM_PIDIV4 * 0.4f, XM_PIDIV4, XM_PIDIV4 * 0.8f);

	hkpRigidBody* wallBody = new hkpRigidBody(wallInfo);
	wallShape->removeReference();
	PhysicsEngine::getInstance()->addRigidBody(wallBody);
	wallBody->removeReference();

	ISimpleMesh* wallMesh = IMeshManager::getInstance()->createCubeMesh("wall", 100.0f * 2, 3.0f * 2, 3.0f * 2);

	//IMeshNode* wallNode = smgr->addMeshNode(wallMesh, nullptr, nullptr, false, XMFLOAT3(0, 3.0f, 100), XMFLOAT3(0, XM_PIDIV4, 0));
	IMeshNode* wallNode = smgr->addMeshNode(wallMesh, nullptr, nullptr, false);
	
	wallNode->transform(M1);
	wallNode->translate(0, 3.0f, 100);
	

	wallNode->setMaterialName("ground_material");

}

void updatePhysics(f32 dt)
{
	PhysicsEngine* engine = PhysicsEngine::getInstance();
	engine->update(dt);
}

void buildLandscape(ISceneManager* smgr)
{
	IMeshManager* meshManager = IMeshManager::getInstance();
	PhysicsEngine* engine = PhysicsEngine::getInstance();

#ifndef TERRAIN_ON

	const f32 size = 1000.0f;
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", size, size, 50, 50, 50.0f, 50.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");

	hkpRigidBodyCinfo groundInfo;
	hkVector4 groundSize(size * 0.5f, 0.5, size * 0.5f);
	hkpBoxShape* groundShape = new hkpBoxShape(groundSize, 0);
	groundInfo.m_shape = groundShape;
	groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
	groundInfo.m_position.set(0.0f, -0.5, 0.0f);
	//groundInfo.m_restitution = 0.9f;
	groundInfo.m_friction = 0.9f;

	hkpRigidBody* floorRigidBody = new hkpRigidBody(groundInfo);
	groundShape->removeReference();
	PhysicsEngine::getInstance()->addRigidBody(floorRigidBody);
	floorRigidBody->removeReference();
#endif

	std::string rawFileName("terrain-heightmap4.raw");
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", rawFileName,
		5.0f, 0.3f, false, true, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh);
	//terrainNode->setMaterialName("terrain/tessellation_material");
	terrainNode->setMaterialName("terrain/terrain_material");
	//terrainNode->addShadow(1);

	hkpRigidBodyCinfo groundInfo;

	hkpSampledHeightFieldBaseCinfo ci;
	ci.m_xRes = terrainMesh->getRowCellNum();
	ci.m_zRes = terrainMesh->getRowCellNum();

	f32 heightScale = terrainMesh->getHeightScale();
	f32 widthScale = terrainMesh->getVertexSpace();
	ci.m_scale.set(widthScale, 1.0f, widthScale);

	TerrainHeightFieldShape* heightFieldShape = new TerrainHeightFieldShape(ci, terrainMesh);

	// Wrap the heightfield in a hkpTriSampledHeightFieldCollection:
	hkpTriSampledHeightFieldCollection* collection = new hkpTriSampledHeightFieldCollection(heightFieldShape);

	// Now wrap the hkpTriSampledHeightFieldCollection in a hkpTriSampledHeightFieldBvTreeShape
	hkpTriSampledHeightFieldBvTreeShape* bvTree = new hkpTriSampledHeightFieldBvTreeShape(collection);

	groundInfo.m_shape = bvTree;
	groundInfo.m_position.set(-0.5f * ci.m_xRes * ci.m_scale(0), 0, -0.5f * ci.m_zRes * ci.m_scale(2));

	groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
	groundInfo.m_friction = 0.9f;
	groundInfo.m_restitution = 0.9f;
	hkpRigidBody* groundbody = new hkpRigidBody(groundInfo);

	heightFieldShape->removeReference();
	bvTree->removeReference();
	collection->removeReference();

	engine->addRigidBody(groundbody);

	groundbody->removeReference();
}
