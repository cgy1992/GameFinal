#include "Car.h"
#include "InputHandler.h"

using namespace gf::math;

Car::Car(ISceneManager* sceneManager,
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
	mCarNode->translate(0, mAboveGroundHeight, 0);

	mWheelRadius = wheelAabb.Extents.y;

	mCarNode->addShadow(1);
	for (u32 i = 0; i < 4; i++)
		mWheelNodes[i]->addShadow(1);

//	mCarNode->yaw(XM_PIDIV2);
}


Car::~Car()
{

}

void Car::update(f32 deltaTime)
{
	ISceneManager* sceneManager = mCarNode->getSceneManager();

	XMFLOAT3 axises[3];
	mCarNode->getLocalAxis(axises);
	XMFLOAT3 xAxis = axises[0];
	XMFLOAT3 yAxis = axises[1];
	XMFLOAT3 zAxis = axises[2];

	if (xAxis.x > 100000 || xAxis.x < -100000)
	{
		int a = 1;
	}
	

	bool bMoving = false;
	bool bTurning = false;

	if (InputHandler::keyPressed(E_MOVE_FORWARD))
	{
		mVelocity += deltaTime * mAcceleration;
		if (mVelocity > mMaxVelocity)
			mVelocity = mMaxVelocity;

		
		mWheelRollRotation += deltaTime * 5.0f;
		bMoving = true;
	}

	if (InputHandler::keyPressed(E_MOVE_BACK))
	{
		mVelocity -= deltaTime * mAcceleration;
		if (mVelocity < -mMaxVelocity)
			mVelocity = -mMaxVelocity;

		mWheelRollRotation -= deltaTime * 5.0f;

		bMoving = true;
	}


	static f32 maxTurnRotation = XM_PIDIV4 * 0.5f;

	

	if (InputHandler::keyPressed(E_TURE_LEFT))
	{
		mWheelTurnRotation -= deltaTime * 2.0f;
		if (mWheelTurnRotation < -maxTurnRotation)
		{
			mWheelTurnRotation = -maxTurnRotation;
		}

		turn(-deltaTime * getTurnVelocity(), yAxis);
		bMoving = true;
		bTurning = true;
	}

	if (InputHandler::keyPressed(E_TURE_RIGHT))
	{
		mWheelTurnRotation += deltaTime * 2.0f;
		if (mWheelTurnRotation > maxTurnRotation)
		{
			mWheelTurnRotation = maxTurnRotation;
		}

		turn(deltaTime * getTurnVelocity(), yAxis);
		bMoving = true;
		bTurning = true;
	}

	if (!bTurning)
	{
		if (mWheelTurnRotation > 0)
		{
			mWheelTurnRotation -= deltaTime * 4.0f;
			if (mWheelTurnRotation < 0)
				mWheelTurnRotation = 0;
		}
		else if (mWheelTurnRotation < 0)
		{
			mWheelTurnRotation += deltaTime * 4.0f;
			if (mWheelTurnRotation > 0)
				mWheelTurnRotation = 0;
		}
	}


	if (!bMoving)
	{
		if (mVelocity > 0)
		{
			mVelocity -= mFrictionAcceleration *deltaTime;
			if (mVelocity < 0)
				mVelocity = 0;
		}
		else if (mVelocity < 0)
		{
			mVelocity += mFrictionAcceleration * deltaTime;
			if (mVelocity > 0)
				mVelocity = 0;
		}
	}

	if (mVelocity != 0)
	{
		XMFLOAT3 movement = VectorMultiply(xAxis, deltaTime * mVelocity);
		mCarNode->translate(VectorOpposite(movement));
	}

	if (mVelocity != 0 || mWheelTurnRotation != 0)
	{
		rotateWheel(deltaTime);
	}


	/* ÉãÏñ»ú¸ú×Ù */
	ICameraNode* followCamera = sceneManager->getCameraNode(1);

	f32 distBetweenCameraAndCar = 10.0f;
	f32 angleBetweenCameraAndCar = XM_PI / 10;

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
	transVec = XMVectorScale(transVec, 0.1f);

	XMVECTOR lookTargetVec = pos;
	XMFLOAT3 lookTarget;
	XMStoreFloat3(&lookTarget, lookTargetVec);

	XMFLOAT3 trans;
	XMStoreFloat3(&trans, transVec);

	//followCamera->setPosition(targetPos);
	followCamera->lookAt(lookTarget);
	followCamera->translate(trans.x, trans.y, trans.z);


	/* ÅÔ²àÉãÏñ»ú */
	ICameraNode* sideCamera = sceneManager->getCameraNode(2);
	cameraPos = XMLoadFloat3(&sideCamera->getPosition());
	rotM = XMMatrixRotationAxis(forward, -angleBetweenCameraAndCar);
	v = XMVector3TransformNormal(forwardRight, rotM);
	cameraTargetPos = XMVectorMultiplyAdd(v, dist, pos);

	transVec = cameraTargetPos - cameraPos;
	transVec = XMVectorScale(transVec, 0.1f);

	XMStoreFloat3(&trans, transVec);

	//followCamera->setPosition(targetPos);
	sideCamera->lookAt(lookTarget);
	sideCamera->translate(trans.x, trans.y, trans.z);

	/*

	if (GetAsyncKeyState('W') & 0x8000)
	{
		car->translate(movement);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		car->translate(math::VectorMultiply(movement, -1.0f));
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		car->yaw(-CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		car->yaw(CAMERA_ROTATE_UNIT * delta);
	}

	f32 fCarCameraDist = 8.0f;

	XMFLOAT3 carPos = car->getPosition();


	XMVECTOR cameraPos = XMLoadFloat3(&followCamera->getPosition());
	XMVECTOR pos = XMLoadFloat3(&carPos);
	XMVECTOR invForword = XMVectorSet(-xAxis.x, -xAxis.y, -xAxis.z, 0.0f);
	XMVECTOR forwardRight = XMVectorSet(-zAxis.x, -zAxis.y, -zAxis.z, 0.0f);

	XMMATRIX rotM = XMMatrixRotationAxis(forwardRight, XM_PI / 10);
	XMVECTOR v = XMVector3TransformNormal(invForword, rotM);
	XMVECTOR dist = XMVectorSet(fCarCameraDist, fCarCameraDist, fCarCameraDist, 0.0f);
	XMVECTOR cameraTargetPos = XMVectorMultiplyAdd(v, dist, pos);

	XMVECTOR transVec = cameraTargetPos - cameraPos;
	transVec = XMVectorScale(transVec, 0.1f);

	XMVECTOR lookTargetVec = pos + XMVectorScale(invForword, -fCarCameraDist);
	XMFLOAT3 lookTarget;
	XMStoreFloat3(&lookTarget, lookTargetVec);

	XMFLOAT3 trans;
	XMStoreFloat3(&trans, transVec);

	followCamera->lookAt(lookTarget);
	followCamera->translate(trans.x, trans.y, trans.z);
	*/
}

void Car::turn(f32 angle, XMFLOAT3 yAxis)
{
	XMFLOAT3 pos = mCarNode->getPosition();
	XMVECTOR pos_v = XMLoadFloat3(&pos);
	XMVECTOR y = XMLoadFloat3(&yAxis);

	XMMATRIX rot = XMMatrixRotationAxis(y, angle);

	mCarNode->translate(-pos.x, -pos.y, -pos.z);
	mCarNode->transform(rot);
	mCarNode->translate(pos);
}


void Car::rotateWheel(f32 deltaTime)
{
	f32 rad = mVelocity / mWheelRadius * deltaTime;
	mWheelRollRotation += rad;

	for (u32 i = 0; i < 4; i++)
	{
		//mWheelNodes[i]->yaw(deltaTime * speed);
		//mWheelNodes[i]->resetTransform();

		SAxisAlignedBox aabb = mWheelMeshes[i]->getAabb();
		XMFLOAT3 wheelPos = aabb.Center;

		XMMATRIX invT = XMMatrixTranslation(-wheelPos.x, -wheelPos.y, -wheelPos.z);
		XMMATRIX rollMatrix = XMMatrixRotationZ(mWheelRollRotation);
		XMMATRIX yawMatrix = XMMatrixRotationY(mWheelTurnRotation);
		XMMATRIX T = XMMatrixTranslation(wheelPos.x, wheelPos.y, wheelPos.z);

		XMMATRIX M = invT * rollMatrix * yawMatrix * T;
		
		mWheelNodes[i]->setTransform(M);

	}

}
