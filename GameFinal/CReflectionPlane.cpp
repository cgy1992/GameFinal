#include "stdafx.h"
#include "CReflectionPlane.h"

namespace gf
{
	CReflectionPlane::CReflectionPlane(XMFLOAT4 plane, f32 sizeX, f32 sizeZ)
		:mPlaneEquation(plane), mSizeX(sizeX), mSizeZ(sizeZ)
	{

	}


	CReflectionPlane::~CReflectionPlane()
	{

	}

	void CReflectionPlane::render(ICameraNode* camera)
	{
		// visibility test
		math::SFrustum frustum = camera->getFrustum();
		
		

	}

	

}

