#ifndef __REFLECTION_CLASS_H__
#define __REFLECTION_CLASS_H__

#include "IReflectionPlane.h"
#include "ISceneManager.h"

namespace gf
{
	class CReflectionPlane : public IReflectionPlane
	{
	public:
		CReflectionPlane(ISceneManager* smgr, u32 id,
			XMFLOAT4 plane, f32 sizeX, f32 sizeZ,
			f32 mapWidth, f32 mapHeight);

		~CReflectionPlane();

		virtual void render(ICameraNode* camera);

		virtual ITexture* getReflectionMap()
		{
			return mReflectMapTexture;
		}

		virtual XMFLOAT4X4 getViewMatrix() const
		{
			return mViewMatrix;
		}

		virtual XMFLOAT4X4 getProjMatrix() const
		{
			return mProjMatrix;
		}

		virtual XMFLOAT4X4 getViewProjMatrix() const
		{
			return mViewProjMatrix;
		}

	private:
		u32				mId;
		XMFLOAT4		mPlaneEquation;
		f32				mPlaneSizeX;
		f32				mPlaneSizeZ;
		ISceneManager*	mSceneManager;

		f32				mMapWidth;
		f32				mMapHeight;

		ITexture*		mReflectMapTexture;

		XMFLOAT4X4		mViewMatrix;
		XMFLOAT4X4		mProjMatrix;
		XMFLOAT4X4		mViewProjMatrix;

	};
}

#endif

