# Havok #

Although GameFinal does not provide physics support, you can combine it with other well-known physics engines very convenietly. This tutorial will show you how to combine GameFinal engine with the famous physics engine, Havok, and begin adding some physics into your program.

Each engine has its own structure to store and represent an object. In GameFinal, we use a `IMeshNode` to represent an object in the scene, while Havok uses `hkpRigidBody` to store a rigid object. Every program needs to maintain both of these two structures. In each frame, we need first update the state of those "hkpRigidBodys", after that, we need to use the state and position of these rigid bodies to update those corresponding mesh nodes, which would be rendered afterward by GameFinal. So our program has to maintain which rigid body corresponds to which mesh node. In order to do that, we'd better put the two structures into one structure.

In this tutorial, we create a class called `PhysicsCube`, which is an abstraction of a cube. This class contains two references, one is pointing to a `IMeshNode`, one is pointing to a `hkpRigidBody`:

	class PhysicsCube
	{
	public:
		PhysicsCube(IMeshNode* node, XMFLOAT3 halfExtends, XMFLOAT3 pos, 
			bool bStatic, f32 restitution, f32 friction);
		void update(f32 dt);
	
	private:
		IMeshNode*				mMeshNode;
		hkpRigidBody*			mRigidBody;
	}; 

The constructor of this class receives a `IMeshNode` interface and other physics parameters. In the constructor, we create a physics body based on these parameters:

	PhysicsCube::PhysicsCube(IMeshNode* node, XMFLOAT3 halfExtends, XMFLOAT3 pos, 
		bool bStatic, f32 restitution, f32 friction)
	:mMeshNode(node)
	{
		hkpRigidBodyCinfo boxInfo;
		hkVector4 halfBoxSize(halfExtends.x, halfExtends.y, halfExtends.z);
		hkpBoxShape* boxShape = new hkpBoxShape(halfBoxSize, 0);
	
		boxInfo.m_shape = boxShape;
		boxInfo.m_position.set(pos.x, pos.y, pos.z);
		boxInfo.m_restitution = restitution;
		boxInfo.m_friction = friction;
		boxInfo.m_motionType = (bStatic) ? hkpMotion::MOTION_FIXED : hkpMotion::MOTION_DYNAMIC;
	
		hkReal boxMass =  10.0f;
		hkMassProperties boxMassProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfBoxSize, boxMass, boxMassProperties);
		boxInfo.m_inertiaTensor = boxMassProperties.m_inertiaTensor;
		boxInfo.m_centerOfMass = boxMassProperties.m_centerOfMass;
		boxInfo.m_mass = boxMassProperties.m_mass;
	
		mRigidBody = new hkpRigidBody(boxInfo);
		boxShape->removeReference();
	
		PhysicsEngine* engine = PhysicsEngine::getInstance();
		engine->addRigidBody(mRigidBody);
	}

The code is clear if you're familiar with Havok engine. Firstly, we create a `hkpRigidBodyCinfo` structure and fill that structure with a cube's physics information, including its shape, position, friction, mass and so on. After that, we create the rigid body using this structure. Then we call `PhysicsEngine::add` method to add this rigid body to our physics system. The `PhysicsEngine` class is written by ourselves. Its function is quiet simple: it just initialize, update, and destory all the physics objects in the program. We will not show the implementations of this class here.

The `update` method in the PhysicsCube is just get the state of rigid body, and then pass rigid body's position and oriention to the real mesh node:

	void PhysicsCube::update(f32 dt)
	{
		XMMATRIX M;
		mRigidBody->getTransform().get4x4ColumnMajor(&M._11);
		mMeshNode->setTransform(M);
	}

We declare a vector of PhysicsCubes globally:

	std::vector<PhysicsCube*> g_boxes;

Then we begin to create these PhysicsCube. As we will add so many cubes in the same scene, it's better to use instancing method supported by GameFinal. So firstly, we create a instance collection:


	ISimpleMesh* boxMesh = IMeshManager::getInstance()->createCubeMesh("box1", boxSize, boxSize, boxSize);
	u32 boxNum = 300;
	IInstanceCollectionNode* collection = smgr->addInstanceCollectionNode(boxMesh, nullptr, boxNum, sizeof(XMFLOAT4));
	for (u32 i = 0; i < boxNum; i++)
	{
		addBoxInstance(smgr, boxMesh, collection);
	}
	collection->setMaterialName("multi_box_material");
	collection->addShadow(1);
	

Then we begin to implement `addBoxInstance` function. In this function, instead of simply creating a `IMeshNode`, we create a PhysicsCube object in which a rigid body is initialized and stored:

	void addBoxInstance(ISceneManager* smgr, ISimpleMesh* mesh, IInstanceCollectionNode* collection)
	{
		f32 x = math::RandomFloat(-20.0f, 20.0f);
		f32 z = math::RandomFloat(-20.0f, 20.0f);
		f32 y = math::RandomFloat(20.0f, 200.0f);
	
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

For each instance node, we firstly create a `IInstanceNode` interface at a random position. Notice that we also give each cube a random color by calling `IInstanceNode::setData` method. After that, we create the PhysicsCube object and put it into the `g_boxes` vector. The program would only maintains this vector and uses it to update the states of these cubes.
  
In order to update the physics system every frame, we create a function called `updatePhysics`:

	void updatePhysics(f32 dt)
	{
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

This function firstly updates the whole physics system. Remember that we have already put those rigid bodies into the PhysicsEngine, so they would be updated in this calling. Then we step through all the elements in the g_boxes and call the `update` method in the PhysicsCube class.

In the game loop, before we call the update method on `ISceneManager`, we need to firstly call this `updatePhysics` function:

		updatePhysics(dt);
		smgr->update(dt);

It guarantees that the physics system has been updated before the update for rendering.

Run this demo, you will see many boxes falling down, bouncing on the ground and colliding with each other:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/09-01.png)
