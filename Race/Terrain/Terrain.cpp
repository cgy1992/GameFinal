#include "stdafx.h"
#include "InputHandler.h"
#include "PhysicsEngine.h"
#include "TerrainHeightFieldShape.h"
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
#pragma comment(lib, "hkpVehicle.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 30.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;
const f32 OBJECT_NUM_PER_ROW = 10;
const f32 OBJECT_SIZE = 1.0f;
const f32 OBJECT_INTERVAL = 2.0f;

const f32 groundSize = 1000.0f;
const f32 boxSize = 2.0f;

void updateCamera(ICameraNode* camera, f32 delta);
void setupPhysics(ISceneManager* smgr, ITerrainMesh* terrainMesh);
void createBoxes(ISceneManager* smgr);
void updatePhysics(f32 dt);

std::vector<PhysicsCube*> g_boxes;

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

	srand(time(0));

	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IApplication* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->createSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	//	resourceGroupManager->loadResourceGroup("General");

	PhysicsEngine::initInstance();
	

	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(3.0f, -2.0f, 1.5f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));
	light->enableShadow(true);
	light->setShadowCameraOrthographicSize(100.0f, 100.0f);

	ILightNode* light2 = smgr->addDirectionalLight(2, nullptr, XMFLOAT3(-2.0f, -3.0f, -2.0f));
	light2->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light2->setDiffuse(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 100.0f, -50.0f), XMFLOAT3(0, 10.0f, 10.0f), XMFLOAT3(0, 1.0f, 0), true);



	char caption[200];

	std::string rawFileName("heightmap.raw");
	ITerrainMesh* mesh = meshManager->createTerrainMesh("terrain", rawFileName,
		3.0f, 0.4f, true, true, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(mesh);
	terrainNode->setMaterialName("terrain/tessellation_material");
	//terrainNode->setMaterialName("terrain/terrain_material");

	ITextureCube* skyTexture = textureManager->loadCubeTexture("Snow.dds");
	smgr->setSkyDome(skyTexture);


	PhysicsEngine::getInstance()->getWorld()->lock();

	setupPhysics(smgr, mesh);

	createBoxes(smgr);

	PhysicsEngine::getInstance()->getWorld()->unlock();

	ITimer* timer = device->getTimer();
	timer->reset();

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		u32 ms = timer->tick();
		float dt = ms * 0.001f;
		updateCamera(camera, dt);
		InputHandler::update();
		updatePhysics(dt);

		smgr->update(ms);

		driver->beginScene(true, true, clearColor);
		smgr->drawAll();
		driver->endScene();

		u32 num = smgr->getRenderedMeshNum();

		sprintf(caption, "FPS:%f num:%d", getFps(dt), num);
		device->setWindowCaption(caption);
	}

	smgr->destroy();
	device->drop();

	PhysicsEngine::getInstance()->deleteInstance();
	
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

void setupPhysics(ISceneManager* smgr, ITerrainMesh* terrainMesh)
{
	
	PhysicsEngine* engine = PhysicsEngine::getInstance();

	
	hkpRigidBodyCinfo groundInfo;

	hkpSampledHeightFieldBaseCinfo ci;
	ci.m_xRes = terrainMesh->getRowCellNum();
	ci.m_zRes = terrainMesh->getRowCellNum();

	f32 heightScale = terrainMesh->getHeightScale();
	f32 widthScale = terrainMesh->getVertexSpace();

	ci.m_scale.set(widthScale, 1.0f, widthScale);
//	ci.m_minHeight = 0;
//	ci.m_maxHeight = 1.0f;

	TerrainHeightFieldShape* heightFieldShape = new TerrainHeightFieldShape(ci, terrainMesh);

	// Wrap the heightfield in a hkpTriSampledHeightFieldCollection:
	hkpTriSampledHeightFieldCollection* collection = new hkpTriSampledHeightFieldCollection(heightFieldShape);

	// Now wrap the hkpTriSampledHeightFieldCollection in a hkpTriSampledHeightFieldBvTreeShape
	hkpTriSampledHeightFieldBvTreeShape* bvTree = new hkpTriSampledHeightFieldBvTreeShape(collection);

	groundInfo.m_shape = bvTree;
	groundInfo.m_position.set(-0.5f * ci.m_xRes * ci.m_scale(0), 0, -0.5f * ci.m_zRes * ci.m_scale(2));
	//groundInfo.m_position.setMul4(-0.5f, heightFieldShape->m_extents);

	//groundInfo.m_position.set(0, 0, 0);

	groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
	groundInfo.m_friction = 0.5f;
	hkpRigidBody* groundbody = new hkpRigidBody(groundInfo);

	heightFieldShape->removeReference();
	bvTree->removeReference();
	collection->removeReference();

	engine->addRigidBody(groundbody);
	
	groundbody->removeReference();
	
	/*
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
	*/
}

void addBoxInstance(ISceneManager* smgr, ISimpleMesh* mesh, IInstanceCollectionNode* collection)
{
	f32 x = math::RandomFloat(-50.0f, 50.0f);
	f32 z = math::RandomFloat(-50.0f, 50.0f);
	f32 y = math::RandomFloat(20.0f, 40.0f);
	//f32 y = 0;

	IInstanceNode* boxNode = collection->addInstance(false, XMFLOAT3(x, y, z), XMFLOAT3(0, 0, 0));
	boxNode->addShadow(1);

	f32 r = math::RandomFloat(0, 1.0f);
	f32 g = math::RandomFloat(0, 1.0f);
	f32 b = math::RandomFloat(0, 1.0f);

	XMFLOAT4 color(r, g, b, 1.0f);
	boxNode->setData(&color);
	//boxNode->setNeedCulling(false);

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