#ifndef __CAMERA_NODE_INTERFACE_H__
#define __CAMERA_NODE_INTERFACE_H__

#include "ISceneNode.h"
#include "gfMath.h"

namespace gf
{

	class ICameraNode : public ISceneNode
	{
	public:
		ICameraNode(ISceneNode* parent, ISceneManager* smgr,
			const XMFLOAT3& position,
			f32 aspectRatio,
			f32 fov,
			f32 nearZ,
			f32 farZ, 
			bool bPerspectiveProj)
			:ISceneNode(parent, smgr, false, position)
			, mFovAngleY(fov)
			, mNearZ(nearZ)
			, mFarZ(farZ)
			, mAspectRatio(aspectRatio)
			, mPerspectiveProj(bPerspectiveProj)
		{
			mViewWidth = 300.0f;
			mViewHeight = 200.0f;
		}

		virtual void lookAt(const XMFLOAT3& lookat) = 0;
		virtual void look(const XMFLOAT3& dir) = 0;
		virtual void walk(f32 unit) = 0;
		virtual void fly(f32 unit) = 0;
		virtual void strafe(f32 unit) = 0;

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

		void setFovAngle(f32 fov)
		{
			mFovAngleY = fov;
		}

		f32 getFovAngle() const
		{
			return mFovAngleY;
		}

		void setNearZ(f32 nearZ)
		{
			mNearZ = nearZ;
		}

		f32 getNearZ() const
		{
			return mNearZ;
		}

		void setFarZ(f32 farZ)
		{
			mFarZ = farZ;
		}

		f32 getFarZ() const
		{
			return mFarZ;
		}

		void setAspectRatio(f32 aspectRatio)
		{
			mAspectRatio = aspectRatio;
		}

		f32 getAspectRatio() const
		{
			return mAspectRatio;
		}

		void setViewWidth(f32 viewWidth)
		{
			mViewWidth = viewWidth;
		}

		void setViewHeight(f32 viewHeight)
		{
			mViewHeight = viewHeight;
		}

		f32 getViewWidth() const
		{
			return mViewWidth;
		}

		f32 getViewHeight() const
		{
			return mViewHeight;
		}

		XMMATRIX calcProjMatrix() const
		{
			XMMATRIX proj;
			if (mPerspectiveProj)
				proj = XMMatrixPerspectiveFovLH(mFovAngleY, mAspectRatio, mNearZ, mFarZ);
			else
				proj = XMMatrixOrthographicLH(mViewWidth, mViewHeight, mNearZ, mFarZ);

			return proj;
		}

		const math::SFrustum& getFrustum() const
		{
			return mFrustum;
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_CAMERA;
		}

		const static u32 ACTIVE_CAMERA_ID = 128;

	protected:
		XMFLOAT4X4		mViewMatrix;
		XMFLOAT4X4		mProjMatrix;
		XMFLOAT4X4		mViewProjMatrix;
		math::SFrustum	mFrustum;

		f32				mFovAngleY;
		f32				mAspectRatio;
		f32				mNearZ;
		f32				mFarZ;
		f32				mViewWidth;
		f32				mViewHeight;

		bool			mPerspectiveProj;
	};

}

#endif
