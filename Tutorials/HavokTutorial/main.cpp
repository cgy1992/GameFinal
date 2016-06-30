#include "stdafx.h"
#include "PhysicsEngine.h"
#include "PhysicsCube.h"
#include <time.h>

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

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 10.0f;
const f32 CAMERA_ROTATE_UNIT = 0.5f;

void setupPhysics(ISceneManager* smgr);
void updatePhysics(f32 dt);
void createBoxes(ISceneManager* smgr);

PhysicsCube* g_ground = nullptr;
const f32 groundSize = 1000.0f;
const f32 boxSize = 2.0f;

std::vector<PhysicsCube*> g_boxes;

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(0));

	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IApplication* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 10.0f, 0);
	aabb.Extents = XMFLOAT3(500.0f, 500.0f, 500.0f);
	ISceneManager* smgr = device->createSceneManager(aabb);
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();
	IPipelineManager* pipelineManager = driver->getPipelineManager();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	PhysicsEngine::getInstance();

	setupPhysics(smgr);

	// create ground
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", groundSize, groundSize, 10, 10, 30.0f, 30.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");

	createBoxes(smgr);

	// add directional light
	XMFLOAT3 light_dir(5.0f, -5.0f, -2.0f);
	f32 lightDistance = -20;
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, light_dir);
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->enableShadow(true);

	ICameraNode* camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setPosition(XMFLOAT3(0, 10.0f, -40.0f));
	camera->lookAt(XMFLOAT3(0, 0, 0));
	camera->setNearZ(1.0f);
	camera->setFarZ(1000.0f);
	camera->setShadowRange(200.0f);
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		

		float dt = timer->tick();
		updatePhysics(dt);
		smgr->update(dt);

		driver->beginScene(true, true, clearColor);
		smgr->drawAll();
		driver->endScene();
	}

	smgr->destroy();
	device->drop();

	delete g_ground;
	PhysicsEngine::deleteInstance();

	return 0;
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
	IMeshManager* meshManager = IMeshManager::getInstance();
	PhysicsEngine* engine = PhysicsEngine::getInstance();

	hkpRigidBodyCinfo groundInfo;
	hkVector4 groundSize(groundSize * 0.5f, 0.5, groundSize * 0.5f);
	hkpBoxShape* groundShape = new hkpBoxShape(groundSize, 0);
	groundInfo.m_shape = groundShape;
	groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
	groundInfo.m_position.set(0.0f, -0.5, 0.0f);
	groundInfo.m_restitution = 0.9f;
	groundInfo.m_friction = 0.5f;

	hkpRigidBody* floorRigidBody = new hkpRigidBody(groundInfo);
	groundShape->removeReference();
	engine->addRigidBody(floorRigidBody);
	floorRigidBody->removeReference();
}


void updatePhysics(f32 dt)
{
	//	physicsWorld->stepDeltaTime(dt);

	if (dt > 0)
	{
		PhysicsEngine* engine = PhysicsEngine::getInstance();
		engine->update(dt);
		for (u32 i = 0; i < g_boxes.size(); i++)
		{
			g_boxes[i]->update(dt);
		}

	}
}

void addBoxInstance(ISceneManager* smgr, ISimpleMesh* mesh, IInstanceCollectionNode* collection)
{
	f32 x = math::RandomFloat(-20.0f, 20.0f);
	f32 z = math::RandomFloat(-20.0f, 20.0f);
	f32 y = math::RandomFloat(20.0f, 200.0f);
	//f32 y = 0;

	IInstanceNode* boxNode = collection->addInstance(false, XMFLOAT3(x, y, z), XMFLOAT3(0, 0, 0));
	boxNode->addShadow(1);

	f32 r = math::RandomFloat(0, 1.0f);
	f32 g = math::RandomFloat(0, 1.0f);
	f32 b = math::RandomFloat(0, 1.0f);

	XMFLOAT4 color(r, g, b, 1.0f);
	boxNode->setData(&color);

	XMFLOAT3 boxHalfSize(boxSize / 2, boxSize / 2, boxSize / 2);
	PhysicsCube* cube = new PhysicsCube(boxNode, boxHalfSize, XMFLOAT3(x, y, z), false, 0.5f, 0.5f);
	g_boxes.push_back(cube);
}

void createBoxes(ISceneManager* smgr)
{
	ISimpleMesh* boxMesh = IMeshManager::getInstance()->createCubeMesh("box1", boxSize, boxSize, boxSize);
	u32 boxNum = 1000;
#if defined(DEBUG) || defined(_DEBUG)
	boxNum = 100;
#endif
	IInstanceCollectionNode* collection = smgr->addInstanceCollectionNode(boxMesh, nullptr, boxNum, sizeof(XMFLOAT4));
	for (u32 i = 0; i < boxNum; i++)
	{
		addBoxInstance(smgr, boxMesh, collection);
	}
	collection->setMaterialName("multi_box_material");
	collection->addShadow(1);
}
