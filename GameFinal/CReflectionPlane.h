#ifndef __REFLECTION_CLASS_H__
#define __REFLECTION_CLASS_H__

#include "IReflectionPlane.h"
#include "ISceneManager.h"

namespace gf
{
	class CReflectionMediator;

	class CReflectionPlane : public IReflectionPlane
	{
	public:
		CReflectionPlane(ISceneManager* smgr, u32 id,
			XMFLOAT3 pos,
			XMFLOAT3 normal,
			XMFLOAT3 tangent,
			XMFLOAT2 size,
			u32 mapWidth, 
			u32 mapHeight);

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

		virtual XMFLOAT3 getPosition() const 
		{
			return mPosition;
		}

		virtual XMFLOAT4 getPlaneEquation() const
		{
			return mPlaneEquation;
		}

		virtual XMFLOAT2 getSize() const
		{
			return mPlaneSize;
		}

		virtual XMFLOAT3 getTangent() const
		{
			return mTangent;
		}

		virtual u32 getId() const
		{
			return mId;
		}

		virtual math::SOrientedBox getOBB() const;

		virtual ISceneManager* getSceneManager()
		{
			return mSceneManager;
		}

	private:
		u32				mId;
		XMFLOAT4		mPlaneEquation;
		XMFLOAT3		mPosition;
		XMFLOAT3		mTangent;
		XMFLOAT3		mNormal;
		XMFLOAT3		mBinormal;
		XMFLOAT2		mPlaneSize;
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

