#pragma once

using namespace gf;

class PhysicsEngine
{
public:
	PhysicsEngine();
	
	void addRigidBody(hkpRigidBody* rigidBody);
	void update(f32 deltaTime);

	~PhysicsEngine();
	
	hkVisualDebugger* setupVisualDebugger();
	void stepVisualDebugger();

	static PhysicsEngine* getInstance();
	static void deleteInstance();

private:
	static PhysicsEngine*	Instance;

	hkpWorld*				mPhysicsWorld;
	hkVisualDebugger*		mVisualDebugger;
	hkpPhysicsContext*		mPhysicsContext;
};

