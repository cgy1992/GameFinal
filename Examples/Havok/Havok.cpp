// Havok.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

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

void updateCamera(ICameraNode* camera, f32 delta);
void updateLightDirection(f32 delta, ILightNode* light);
bool initHavokEngine();
void unloadHavodEngine();
void setupPhysics();
hkVisualDebugger* setupVisualDebugger(hkpPhysicsContext* physicsWorlds);
void stepVisualDebugger(hkVisualDebugger* vdb);
void updatePhysics(f32 dt);

hkpWorld* physicsWorld;
hkVisualDebugger* visualDebugger;
hkpRigidBody* floorRigidBody;
hkpRigidBody* boxRigidBody;
hkpPhysicsContext* physicsContext;

const f32 groundSize = 100.0f;
const f32 boxHalfSize = 2.0f;
const f32 boxInitHeight = 2.0f;
IMeshNode* boxNode;


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
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
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

	initHavokEngine();
	setupPhysics();

	// create ground
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", groundSize, groundSize, 10, 10, 30.0f, 30.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");
	// create sphere

	ISimpleMesh* boxMesh = meshManager->createCubeMesh("box", boxHalfSize*2.0f,
		boxHalfSize * 2.0f, boxHalfSize*2.0f);
	boxNode = smgr->addMeshNode(boxMesh, nullptr, nullptr, false, XMFLOAT3(0, 
		boxHalfSize, 0));
	boxNode->setMaterialName("box_material");
	boxNode->addShadow(1);

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
	camera->setPosition(XMFLOAT3(0, 20.0f, -40.0f));
	camera->lookAt(XMFLOAT3(0, 0, 0));
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

		u32 ms = timer->tick();
		float dt = ms * 0.001f;

		

		updatePhysics(dt);
		updateCamera(camera, dt);
		updateLightDirection(dt, light);
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

	unloadHavodEngine();

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

static void HK_CALL errorReport(const char* msg, void*)
{
	printf("%s", msg);
#ifdef HK_PLATFORM_WIN32
#ifndef HK_PLATFORM_WINRT
	OutputDebugStringA(msg);
#else
	// Unicode only 
	int sLen = hkString::strLen(msg) + 1;
	wchar_t* wideStr = hkAllocateStack<wchar_t>(sLen);
	mbstowcs_s(HK_NULL, wideStr, sLen, msg, sLen - 1);
	OutputDebugString(wideStr);
	hkDeallocateStack<wchar_t>(wideStr, sLen);
#endif
#endif
}


bool initHavokEngine()
{
	PlatformInit();

	hkMemorySystem::FrameInfo finfo(500 * 1024);	// Allocate 500KB of Physics solver buffer
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, finfo);
	hkBaseSystem::init(memoryRouter, errorReport);

	// Create the physics world
	hkpWorldCinfo worldInfo;
	worldInfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM);
	worldInfo.m_gravity = hkVector4(0.0f, -9.8f, 0.0f);
	worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_FIX_ENTITY; // just fix the entity if the object falls off too far

	// You must specify the size of the broad phase - objects should not be simulated outside this region
	worldInfo.setBroadPhaseWorldSize(1000.0f);
	physicsWorld = new hkpWorld(worldInfo);

	hkpAgentRegisterUtil::registerAllAgents(physicsWorld->getCollisionDispatcher());

	setupPhysics();

	physicsContext = new hkpPhysicsContext;
	hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
	physicsContext->addWorld(physicsWorld); // add the physics world so the viewers can see it
	visualDebugger = setupVisualDebugger(physicsContext);

	return true;
}

void setupPhysics()
{
	hkpRigidBodyCinfo groundInfo;
	hkVector4 groundSize(groundSize * 0.5f, 0.5, groundSize * 0.5f);
	hkpBoxShape* groundShape = new hkpBoxShape(groundSize, 0);
	groundInfo.m_shape = groundShape;
	groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
	groundInfo.m_position.set(0.0f, -0.5, 0.0f);
	groundInfo.m_restitution = 0.9f;
	groundInfo.m_friction = 0.5f;

	floorRigidBody = new hkpRigidBody(groundInfo);
	groundShape->removeReference();
	physicsWorld->addEntity(floorRigidBody);
	floorRigidBody->removeReference();

	hkpRigidBodyCinfo boxInfo;
	hkVector4 halfBoxSize(boxHalfSize, boxHalfSize, boxHalfSize);
	hkpBoxShape* boxShape = new hkpBoxShape(halfBoxSize, 0);
	boxInfo.m_shape = boxShape;
	boxInfo.m_position.set(0, boxInitHeight, 0);
	boxInfo.m_motionType = hkpMotion::MOTION_DYNAMIC;
	boxInfo.m_restitution = 0.5f;
	boxInfo.m_friction = 0.5f;

	hkReal boxMass = 10.0f;
	hkMassProperties boxMassProperties;
	hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfBoxSize, boxMass, boxMassProperties);
	boxInfo.m_inertiaTensor = boxMassProperties.m_inertiaTensor;
	boxInfo.m_centerOfMass = boxMassProperties.m_centerOfMass;
	boxInfo.m_mass = boxMassProperties.m_mass;

	boxRigidBody = new hkpRigidBody(boxInfo);
	boxShape->removeReference();

	physicsWorld->addEntity(boxRigidBody);
	//boxRigidBody->removeReference();
}


hkVisualDebugger* setupVisualDebugger(hkpPhysicsContext* physicsWorlds)
{
	// Setup the visual debugger
	hkArray<hkProcessContext*> contexts;
	contexts.pushBack(physicsWorlds);

	hkVisualDebugger* vdb = new hkVisualDebugger(contexts);
	vdb->serve();

	// Allocate memory for internal profiling information
	// You can discard this if you do not want Havok profiling information
	hkMonitorStream& stream = hkMonitorStream::getInstance();
	stream.resize(500 * 1024);	// 500K for timer info
	stream.reset();

	return vdb;
}

void stepVisualDebugger(hkVisualDebugger* vdb)
{
	// Step the debugger
	vdb->step();

	// Reset internal profiling info for next frame
	hkMonitorStream::getInstance().reset();
}

void updatePhysics(f32 dt)
{
//	physicsWorld->stepDeltaTime(dt);

	if (dt > 0)
	{
		

		physicsWorld->stepDeltaTime(dt);
		
		XMMATRIX M;
		boxRigidBody->getTransform().get4x4ColumnMajor(&M._11);
		boxNode->setTransform(M);
		
		hkVector4 pivot(0, 0, 1.0f);
		
		if (GetAsyncKeyState('N') & 0x8000)
		{
			hkVector4 v(-100.0f, 0, 0);
			boxRigidBody->applyForce(dt, v, pivot);
		}
		else if (GetAsyncKeyState('M') & 0x8000)
		{
			hkVector4 v(100.0f, 0, 0);
			boxRigidBody->applyForce(dt, v, pivot);
		}


		if (GetAsyncKeyState('O') & 0x8000)
		{
			hkVector4 v(0, 0, 100);
			boxRigidBody->applyForce(dt, v);
		}
		else if (GetAsyncKeyState('L') & 0x8000)
		{
			hkVector4 v(0, 0, -100);
			boxRigidBody->applyForce(dt, v);
		}

		//visualDebugger->step(dt);

		
	}

}

void unloadHavodEngine()
{
	physicsWorld->removeReference();

	visualDebugger->removeReference();

	// Contexts are not reference counted at the base class level by the VDB as
	// they are just interfaces really. So only delete the context after you have
	// finished using the VDB.
	physicsContext->removeReference();
}
