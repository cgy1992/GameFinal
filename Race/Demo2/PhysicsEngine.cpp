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
, mMemoryRouter(nullptr)
, mThreadPool(nullptr)
, mJobQueue(nullptr)
{
	PlatformInit();

	hkMemorySystem::FrameInfo finfo(5000 * 1024);	// Allocate 500KB of Physics solver buffer
	mMemoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, finfo);
	hkBaseSystem::init(mMemoryRouter, errorReport);


	int totalNumThreadsUsed = hkHardwareInfo::getNumHardwareThreads();

	// We use one less than this for our thread pool, because we must also use this thread for our simulation
	hkCpuThreadPoolCinfo threadPoolCinfo;
	threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

	// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
	// timer collection will not be enabled.
	threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
	mThreadPool = new hkCpuThreadPool(threadPoolCinfo);

	// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
	// Here we only use it for physics.
	hkJobQueueCinfo info;
	info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
	mJobQueue = new hkJobQueue(info);

	// Monitors have been enabled for thread pool threads already (see above comment).
	hkMonitorStream::getInstance().resize(200000);


	// Create the physics world
	hkpWorldCinfo worldInfo;
	worldInfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM);
	worldInfo.m_gravity = hkVector4(0.0f, -9.8f, 0.0f);
	worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_FIX_ENTITY; // just fix the entity if the object falls off too far
	worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
	// You must specify the size of the broad phase - objects should not be simulated outside this region
	worldInfo.setBroadPhaseWorldSize(5000.0f);
	mPhysicsWorld = new hkpWorld(worldInfo);
	// Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
	mPhysicsWorld->m_wantDeactivation = false;

	mPhysicsWorld->markForWrite();
	hkpAgentRegisterUtil::registerAllAgents(mPhysicsWorld->getCollisionDispatcher());
	// We need to register all modules we will be running multi-threaded with the job queue
	mPhysicsWorld->registerWithJobQueue(mJobQueue);

	mPhysicsContext = new hkpPhysicsContext;
	hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
	mPhysicsContext->addWorld(mPhysicsWorld); // add the physics world so the viewers can see it
	mPhysicsWorld->unmarkForWrite();
	mVisualDebugger = setupVisualDebugger();
}

PhysicsEngine::~PhysicsEngine()
{
	if (mPhysicsWorld)
	{
		mPhysicsWorld->markForWrite();
		mPhysicsWorld->removeReference();
	}
		
	if (mVisualDebugger)
		mVisualDebugger->removeReference();

	// Contexts are not reference counted at the base class level by the VDB as
	// they are just interfaces really. So only delete the context after you have
	// finished using the VDB.
	if (mPhysicsContext)
		mPhysicsContext->removeReference();

	delete mJobQueue;

	mThreadPool->removeReference();

	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
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
	//stream.reset();

	return vdb;
}

void PhysicsEngine::stepVisualDebugger()
{
	mPhysicsContext->syncTimers(mThreadPool);

	mVisualDebugger->step();
	// Clear accumulated timer data in this thread and all slave threads
	hkMonitorStream::getInstance().reset();
}

void PhysicsEngine::update(f32 deltaTime)
{
	if (deltaTime > 0)
	{
		mPhysicsWorld->stepMultithreaded(mJobQueue, mThreadPool, deltaTime);
		stepVisualDebugger();
		mThreadPool->clearTimerData();
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

