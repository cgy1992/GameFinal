#include "stdafx.h"
#include "Vehicle.h"
#include "InputHandler.h"
#include "VehicleApiUtils.h"
#include "VehicleSetup.h"
#include "PhysicsEngine.h"

#include <Physics2012\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesShape.h>

using namespace gf::math;

Vehicle::Vehicle(ISceneManager* sceneManager,
	const std::string& meshName)
	:mWheelRollRotation(0)
	, mWheelTurnRotation(0)
	, mAcceleration(10.0f)
	, mFrictionAcceleration(10.0f)
	, mVelocity(0)
	, mMaxVelocity(40.0f)
{
	const char* wheelnames[] = 
	{
		"_wheelfrontleft", "_wheelfrontright",
		"_wheelrearleft", "_wheelrearright"
	};

	std::string meshFileName = meshName + ".mesh";
	std::string meshMaterialName = meshName + "_material";
	std::string meshWheelMaterialName = meshName + "_wheel_material";
	std::string meshWheelNames[4];

	for (u32 i = 0; i < 4; i++)
	{
		meshWheelNames[i] = meshName + wheelnames[i] + ".mesh";
	}

	IMeshManager* meshManager = IMeshManager::getInstance();

	mCarMesh = meshManager->getModelMesh(meshFileName);
	mCarNode = sceneManager->addModelMeshNode(mCarMesh);
	mCarNode->setMaterialName(meshMaterialName);

	for (u32 i = 0; i < 4; i++)
	{
		mWheelMeshes[i] = meshManager->getModelMesh(meshWheelNames[i]);
		mWheelNodes[i] = sceneManager->addModelMeshNode(mWheelMeshes[i], mCarNode);
		mWheelNodes[i]->setMaterialName(meshWheelMaterialName);
	}

	math::SAxisAlignedBox wheelAabb = mWheelMeshes[0]->getAabb();
	
	mAboveGroundHeight = -wheelAabb.Center.y + wheelAabb.Extents.y;

	ISceneNode* emptyNode = sceneManager->addEmptyNode(nullptr, false, XMFLOAT3(0, 0, 0),
		XMFLOAT3(0, 0, 0), XMFLOAT3(0.5f, 0.5f, 0.5f));

	//mCarNode->translate(0, mAboveGroundHeight, 0);
	//emptyNode->addChild(mCarNode);

	mWheelRadius = wheelAabb.Extents.y;

	mCarNode->addShadow(1);
	for (u32 i = 0; i < 4; i++)
		mWheelNodes[i]->addShadow(1);

//	mCarNode->yaw(XM_PIDIV2);

	setupPhysics(mCarMesh->getAabb(), mAboveGroundHeight);
}

Vehicle::~Vehicle()
{

}

void Vehicle::update(f32 deltaTime)
{
	ISceneManager* sceneManager = mCarNode->getSceneManager();
	hkpWorld* world = PhysicsEngine::getInstance()->getWorld();
	world->markForWrite();

	if (InputHandler::keyPressed(E_MOVE_FORWARD)
		|| InputHandler::keyPressed(E_MOVE_BACK)
		|| InputHandler::keyPressed(E_TURE_RIGHT)
		|| InputHandler::keyPressed(E_TURE_LEFT))
	{
		m_vehicle->getChassis()->activate();
	}

	VehicleApiUtils::steer(
		m_inputXPosition,
		m_inputYPosition,
		*m_vehicle,
		deltaTime);

	hkpRigidBody* chassisRigidBody = m_vehicle->getChassis();

	if (InputHandler::keyPressed(E_RESET_ROTATION))
	{
		hkQuaternion quat(0, 0, 0, 1.0f);
		chassisRigidBody->setRotation(quat);
	}

	world->unmarkForWrite();

	XMMATRIX M;
	chassisRigidBody->getTransform().get4x4ColumnMajor(&M._11);

	XMMATRIX T = XMMatrixTranslation(0, mAboveGroundHeight - mCarMesh->getAabb().Extents.y - 0.1f, 0);
	mCarNode->setTransform(M * T);

	XMFLOAT3 axises[3];
	mCarNode->getLocalAxis(axises);
	XMFLOAT3 xAxis = axises[0];
	XMFLOAT3 yAxis = axises[1];
	XMFLOAT3 zAxis = axises[2];

	rotateWheel(deltaTime);


	/* ÉãÏñ»ú¸ú×Ù */

	ICameraNode* followCamera = sceneManager->getCameraNode(2);

	f32 distBetweenCameraAndCar = 10.0f;
	f32 angleBetweenCameraAndCar = XM_PI / 18;

	XMFLOAT3 carPos = mCarNode->getPosition();
	XMVECTOR cameraPos = XMLoadFloat3(&followCamera->getPosition());
	XMVECTOR pos = XMLoadFloat3(&carPos);
	XMVECTOR forward = XMVectorSet(-xAxis.x, -xAxis.y, -xAxis.z, 0.0f);
	XMVECTOR forwardRight = XMVectorSet(-zAxis.x, -zAxis.y, -zAxis.z, 0.0f);

	XMMATRIX rotM = XMMatrixRotationAxis(forwardRight, -angleBetweenCameraAndCar);
	XMVECTOR v = XMVector3TransformNormal(forward, rotM);
	XMVECTOR dist = XMVectorReplicate(distBetweenCameraAndCar);
	XMVECTOR cameraTargetPos = XMVectorMultiplyAdd(-v, dist, pos);
	
	XMVECTOR transVec = cameraTargetPos - cameraPos;
	transVec = XMVectorScale(transVec, 0.4f);

	XMVECTOR lookTargetVec = pos;
	XMFLOAT3 lookTarget;
	XMStoreFloat3(&lookTarget, lookTargetVec);

	XMFLOAT3 trans;
	XMStoreFloat3(&trans, transVec);

	//followCamera->setPosition(targetPos);
	followCamera->lookAt(lookTarget);
	followCamera->translate(trans.x, trans.y, trans.z);

	ICameraNode* sideCamera1 = sceneManager->getCameraNode(3);
	cameraPos = XMLoadFloat3(&sideCamera1->getPosition());
	rotM = XMMatrixRotationAxis(forward, -angleBetweenCameraAndCar);
	v = XMVector3TransformNormal(forwardRight, rotM);
	cameraTargetPos = XMVectorMultiplyAdd(v, dist, pos);

	transVec = cameraTargetPos - cameraPos;
	transVec = XMVectorScale(transVec, 0.8f);

	XMStoreFloat3(&trans, transVec);

	//followCamera->setPosition(targetPos);
	sideCamera1->lookAt(lookTarget);
	sideCamera1->translate(trans.x, trans.y, trans.z);


}

void Vehicle::turn(f32 angle, XMFLOAT3 yAxis)
{
	XMFLOAT3 pos = mCarNode->getPosition();
	XMVECTOR pos_v = XMLoadFloat3(&pos);
	XMVECTOR y = XMLoadFloat3(&yAxis);

	XMMATRIX rot = XMMatrixRotationAxis(y, angle);

	mCarNode->translate(-pos.x, -pos.y, -pos.z);
	mCarNode->transform(rot);
	mCarNode->translate(pos);
}


void Vehicle::rotateWheel(f32 deltaTime)
{
	hkConvertCSImpl<hkReal> converter(hkConvertCSImpl<hkReal>::CT_FLIP_X);

	hkQuaternion wheelRotations[4];

	VehicleApiUtils::calcWheelsRotation(*m_vehicle, wheelRotations);

	hkQuaternion hkCarRotation = m_vehicle->getChassis()->getRotation();
	//converter.convertQuaternion(hkCarRotation);
	
	XMVECTOR carRotation = XMVectorSet(hkCarRotation(0), hkCarRotation(1), hkCarRotation(2), hkCarRotation(3));
	XMMATRIX carRotM = XMMatrixRotationQuaternion(carRotation);
	XMVECTOR det = XMMatrixDeterminant(carRotM);
	XMMATRIX invCarRot = XMMatrixInverse(&det, carRotM);
	

	for (u32 i = 0; i < 4; i++)
	{
		SAxisAlignedBox aabb = mWheelMeshes[i]->getAabb();
		XMFLOAT3 wheelPos = aabb.Center;

		XMMATRIX invT = XMMatrixTranslation(-wheelPos.x, -wheelPos.y, -wheelPos.z);

		hkQuaternion wheelRotation = wheelRotations[i];
		
		XMVECTOR rotQuaternion = XMVectorSet(wheelRotation(0), wheelRotation(1), wheelRotation(2), wheelRotation(3));

		XMMATRIX R = XMMatrixRotationQuaternion(rotQuaternion);
		R = R * invCarRot;
		XMVECTOR zAxis = R.r[2];
		zAxis = XMVectorSet(XMVectorGetX(zAxis), 0, XMVectorGetZ(zAxis), 0);
		zAxis = XMVector4Normalize(zAxis);
		XMVECTOR z_unit = XMVectorSet(0.0f, 0, -1.0f, 0);
		XMVECTOR theta = XMVector3AngleBetweenNormals(zAxis, z_unit);

		f32 angle = XMVectorGetX(theta);
		if (XMVectorGetX(zAxis) < 0)
		{
			angle = -angle;
		}		
		
		//std::cout << angle << std::endl;

		XMMATRIX R1 = XMMatrixRotationY(angle);

		XMVECTOR xAxis = R.r[0];
		xAxis = XMVectorSet(XMVectorGetX(xAxis), XMVectorGetY(xAxis), 0, 0);
		xAxis = XMVector4Normalize(xAxis);
		XMVECTOR x_unit = XMVectorSet(1.0f, 0, 0, 0);
		theta = XMVector3AngleBetweenNormals(xAxis, x_unit);
		angle = XMVectorGetX(theta);
		
		if (XMVectorGetY(xAxis) < 0)
		{
			angle = -angle;
		}

		XMMATRIX R2 = XMMatrixRotationZ(angle);

		XMMATRIX T = XMMatrixTranslation(wheelPos.x, wheelPos.y, wheelPos.z);

		XMMATRIX M = invT  * R2 * R1 * T;

		mWheelNodes[i]->setTransform(M);
	}
}

void Vehicle::setupPhysics(const math::SAxisAlignedBox& aabb, f32 heightAboveGround)
{
	hkpConvexVerticesShape* chassisShape = VehicleApiUtils::createCarChassisShape();
	f32 y = aabb.Extents.y;
	hkVector4 halfSizes(aabb.Extents.x, y, aabb.Extents.z);
	//hkpBoxShape* chassisShape = new hkpBoxShape(halfSizes, 0.01f);

	hkpRigidBody* chassisRigidBody;
	hkpRigidBodyCinfo chassisInfo;

	// NB: The inertia value is reset by the vehicle SDK.  However we give it a
	// reasonable value so that the hkpRigidBody does not assert on construction. See
	// VehicleSetup for the yaw, pitch and roll values used by hkVehicle.
	chassisInfo.m_mass = 750.0f * 2;
	chassisInfo.m_shape = chassisShape;
	chassisInfo.m_friction = 0.2f;

	// The chassis MUST have m_motionType hkpMotion::MOTION_BOX_INERTIA to correctly simulate
	// vehicle roll, pitch and yaw.
	chassisInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
	//chassisInfo.m_position.set(-560.0f, 20.0f, 1320.0f);
	chassisInfo.m_position.set(0.0f, 20.0f, 0.0f);
	
	/*
	hkpInertiaTensorComputer::setShapeVolumeMassProperties(chassisInfo.m_shape,
		chassisInfo.m_mass,
		chassisInfo);
		*/

	//hkpInertiaTensorComputer::setShapeVolumeMassProperties()

	chassisRigidBody = new hkpRigidBody(chassisInfo);

	// No longer need reference to shape as the hkpRigidBody holds one.
	chassisShape->removeReference();

	//m_world->addEntity(chassisRigidBody);
	// Create the basic vehicle.
	m_vehicle = new hkpVehicleInstance(chassisRigidBody);
	VehicleSetup setup;

	hkpWorld* world = PhysicsEngine::getInstance()->getWorld();
	mPhysicsWorld = world;
	
	world->markForWrite();

	setup.buildVehicle(world, *m_vehicle);
	m_vehicle->addToWorld(world);

	///[integrationWithSDK]
	/// Actions are the interface between user controllable behavior of the physical simulation and the Havok core. 
	/// You can easily integrate the Vehicle Kit with the Havok physical simulation using the hkpVehicleInstance action, 
	/// which establishes the connection between the two SDKs. 
	///
	///	To simulate a vehicle, you first need to create a hkpVehicleInstance instance in your game. 
	/// You then add it to the actions of your core physical simulation, just like any other user action:
	///<

	world->addAction(m_vehicle);

	world->unmarkForWrite();
	///>
	/// Once you have added the action to the simulation, no extra work is required to simulate the vehicle. 
	/// On each call to step the core physical simulation, the vehicle action will be updated automatically.

	m_inputXPosition = 0;
	m_inputYPosition = 0;

	//m_vehicle->m_wheelsInfo;

}

void Vehicle::setRotation(const XMFLOAT4& rot)
{
	mPhysicsWorld->markForWrite();

	hkpRigidBody* chassisRigidBody = m_vehicle->getChassis();
	hkQuaternion rotation(rot.x, rot.y, rot.z, rot.w);
	chassisRigidBody->setRotation(rotation);

	mPhysicsWorld->unmarkForWrite();
}

void Vehicle::setPosition(const XMFLOAT3& pos)
{
	mPhysicsWorld->markForWrite();

	hkpRigidBody* chassisRigidBody = m_vehicle->getChassis();
	hkVector4 hkPos(pos.x, pos.y, pos.z, 1.0f);
	chassisRigidBody->setPosition(hkPos);

	mPhysicsWorld->unmarkForWrite();
}
