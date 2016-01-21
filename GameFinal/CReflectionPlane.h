#ifndef __REFLECTION_CLASS_H__
#define __REFLECTION_CLASS_H__

#include "IReflectionPlane.h"

namespace gf
{
	class CReflectionPlane : public IReflectionPlane
	{
	public:
		CReflectionPlane(ISceneManager* smgr, u32 id,
			XMFLOAT4 plane, f32 sizeX, f32 sizeZ,
			f32 mapWidth, f32 mapHeight);
		
		~CReflectionPlane();

		void render(ICameraNode* camera);

	private:
		u32				mId;
		XMFLOAT4		mPlaneEquation;
		f32				mPlaneSizeX;
		f32				mPlaneSizeZ;
		ISceneManager*	mSceneManager;

		f32				mMapWidth;
		f32				mMapHeight;

		ITexture*		mReflectMapTexture;

	};
}

#endif

