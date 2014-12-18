#include "stdafx.h"
#include "PhysicsEngine.h"


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

PhysicsEngine* PhysicsEngine::Instance = nullptr;

PhysicsEngine::PhysicsEngine()
:mPhysicsWorld(nullptr)
, mVisualDebugger(nullptr)
, mPhysicsContext(nullptr)
{
	PlatformInit();

	hkMemorySystem::FrameInfo finfo(5000 * 1024);	// Allocate 500KB of Physics solver buffer
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, finfo);
	hkBaseSystem::init(memoryRouter, errorReport);

	// Create the physics world
	hkpWorldCinfo worldInfo;
	worldInfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM);
	//worldInfo.m_collisionTolerance = 0.03f;
	worldInfo.m_gravity = hkVector4(0.0f, -9.8f, 0.0f);
	worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_FIX_ENTITY; // just fix the entity if the object falls off too far

	// You must specify the size of the broad phase - objects should not be simulated outside this region
	worldInfo.setBroadPhaseWorldSize(10000.0f);
	mPhysicsWorld = new hkpWorld(worldInfo);

	hkpAgentRegisterUtil::registerAllAgents(mPhysicsWorld->getCollisionDispatcher());

	mPhysicsContext = new hkpPhysicsContext;
	hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
	mPhysicsContext->addWorld(mPhysicsWorld); // add the physics world so the viewers can see it
	mVisualDebugger = setupVisualDebugger();
}

PhysicsEngine::~PhysicsEngine()
{
	if (mPhysicsWorld)
		mPhysicsWorld->removeReference();

	if (mVisualDebugger)
		mVisualDebugger->removeReference();

	// Contexts are not reference counted at the base class level by the VDB as
	// they are just interfaces really. So only delete the context after you have
	// finished using the VDB.
	if (mPhysicsContext)
		mPhysicsContext->removeReference();
}

void PhysicsEngine::addRigidBody(hkpRigidBody* rigidBody)
{
	mPhysicsWorld->addEntity(rigidBody);
}

hkVisualDebugger* PhysicsEngine::setupVisualDebugger()
{
	// Setup the visual debugger
	hkArray<hkProcessContext*> contexts;
	contexts.pushBack(mPhysicsContext);

	hkVisualDebugger* vdb = new hkVisualDebugger(contexts);
	vdb->serve();

	// Allocate memory for internal profiling information
	// You can discard this if you do not want Havok profiling information
	hkMonitorStream& stream = hkMonitorStream::getInstance();
	stream.resize(5000 * 1024);	// 500K for timer info
	stream.reset();

	return vdb;
}

void PhysicsEngine::stepVisualDebugger()
{
	// Step the debugger
	mVisualDebugger->step();

	// Reset internal profiling info for next frame
	hkMonitorStream::getInstance().reset();
}

void PhysicsEngine::update(f32 deltaTime)
{
	if (deltaTime > 0)
	{
		mPhysicsWorld->stepDeltaTime(deltaTime);

		stepVisualDebugger();
	}
}

PhysicsEngine* PhysicsEngine::getInstance()
{
	if (!Instance)
	{
		Instance = new PhysicsEngine;
	}
	return Instance;
}

void PhysicsEngine::deleteInstance()
{
	if (Instance)
		delete Instance;
}

void PhysicsEngine::initInstance()
{
	if (!Instance)
	{
		Instance = new PhysicsEngine;
	}
}

