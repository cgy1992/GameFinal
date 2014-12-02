#include "stdafx.h"
#include "PhysicsCube.h"
#include "PhysicsEngine.h"

PhysicsCube::PhysicsCube(IMeshNode* node, XMFLOAT3 halfExtends, XMFLOAT3 pos, 
	bool bStatic, f32 restitution, f32 friction)
:mMeshNode(node)
{
	hkpRigidBodyCinfo boxInfo;
	hkVector4 halfBoxSize(halfExtends.x, halfExtends.y, halfExtends.z);
	hkpBoxShape* boxShape = new hkpBoxShape(halfBoxSize, 0);

	boxInfo.m_shape = boxShape;
	boxInfo.m_position.set(pos.x, pos.y, pos.z);
	//boxInfo.m_motionType = hkpMotion::MOTION_DYNAMIC;
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

void PhysicsCube::update(f32 dt)
{
	XMMATRIX M;
	mRigidBody->getTransform().get4x4ColumnMajor(&M._11);
	mMeshNode->setTransform(M);
}
