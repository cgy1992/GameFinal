#ifndef __REFLECTION_CLASS_H__
#define __REFLECTION_CLASS_H__

#include "IReflectionPlane.h"

namespace gf
{
	class CReflectionPlane : public IReflectionPlane
	{
	public:
		CReflectionPlane(XMFLOAT4 plane, f32 sizeX, f32 sizeZ);
		
		~CReflectionPlane();
		
		void render(ICameraNode* camera);



	private:
		XMFLOAT4		mPlaneEquation;
		f32				mSizeX;
		f32				mSizeZ;

	};
}

#endif

