#ifndef __CAR_CLASS_H__
#define __CAR_CLASS_H__

#include<Windows.h>
#include "GameFinal.h"
#include <math.h>
#include <Physics2012\Vehicle\hkpVehicleInstance.h>

using namespace gf;

class Car
{
public:
	enum E_CAR_WHEEL
	{
		FRONT_RIGHT_WHEEL, 
		FRONT_LEFT_WHEEL,
		REAR_RIGHT_WHEEL,
		REAR_LEFT_WHEEL,
	};

public:
	Car(ISceneManager* sceneManager, 
		const std::string& meshName);
	~Car();

	void update(f32 deltaTime);

	void setupPhysics(const math::SAxisAlignedBox& aabb, f32 heightAboveGround);

	f32 getTurnVelocity()
	{
		return sqrtf(fabs(mVelocity)) * 0.3f;
	}

protected:
	void rotateWheel(f32 deltaTime);

	void turn(f32 angle, XMFLOAT3 yAxis);

private:
	IModelMesh*			mCarMesh;
	IModelMesh*			mWheelMeshes[4];

	IMeshNode*			mCarNode;
	IMeshNode*			mWheelNodes[4];

	//IModelMesh
	//IMeshNode*			mCarNode;

	f32					mAboveGroundHeight;
	f32					mWheelRollRotation;
	f32					mWheelTurnRotation;

	f32					mAcceleration;
	f32					mFrictionAcceleration;
	f32					mVelocity;

	f32					mMaxVelocity;
	f32					mWheelRadius;

	hkpVehicleInstance* m_vehicle;

	hkReal				m_inputXPosition;
	hkReal				m_inputYPosition;
};

#endif
