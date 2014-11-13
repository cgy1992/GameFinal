#ifndef __CAMERA_NODE_INTERFACE_H__
#define __CAMERA_NODE_INTERFACE_H__

#include "ISceneNode.h"
#include "gfMath.h"
#include "gfStructs.h"

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
			mShadowRange = mFarZ;
			splitShadowCascadeSegments();
		}

		virtual void lookAt(const XMFLOAT3& lookat) = 0;
		virtual void look(const XMFLOAT3& dir) = 0;
		virtual void lookAt(const XMFLOAT3& lookat, const XMFLOAT3& up) = 0;
		virtual void look(const XMFLOAT3& dir, const XMFLOAT3& up) = 0;
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

		XMFLOAT4X4 getProjMatrix(f32 nearZ, f32 farZ) const
		{
			XMFLOAT4X4 M;
			XMMATRIX proj;
			if (mPerspectiveProj)
				proj = XMMatrixPerspectiveFovLH(mFovAngleY, mAspectRatio, nearZ, farZ);
			else
				proj = XMMatrixOrthographicLH(mViewWidth, mViewHeight, nearZ, farZ);

			XMStoreFloat4x4(&M, proj);
			return M;
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
			splitShadowCascadeSegments();
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

		void setShadowRange(f32 range)
		{
			mShadowRange = range;
			splitShadowCascadeSegments();
		}

		f32 getViewWidth() const
		{
			return mViewWidth;
		}

		f32 getViewHeight() const
		{
			return mViewHeight;
		}

		f32 getShadowRange() const
		{
			return mShadowRange;
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

		void setPerspectiveProjection(bool perspectiveProj)
		{
			mPerspectiveProj = perspectiveProj;
		}

		bool isPerspectiveProjection() const
		{
			return mPerspectiveProj;
		}

		const math::SFrustum& getFrustum() const
		{
			return mFrustum;
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_CAMERA;
		}

		XMFLOAT4X4 getRotationMatrix()
		{
			XMFLOAT4X4 M;
			M._11 = mRight.x;
			M._21 = mRight.y;
			M._31 = mRight.z;
			M._41 = 0;

			M._12 = mUp.x;
			M._22 = mUp.y;
			M._32 = mUp.z;
			M._42 = 0;

			M._13 = mLook.x;
			M._23 = mLook.y;
			M._33 = mLook.z;
			M._43 = 0;

			M._14 = 0;
			M._24 = 0;
			M._34 = 0;
			M._44 = 1.0f;

			return M;
		}

		void getFrustumCorners(XMFLOAT3 corners[])
		{
			getFrustumCorners(corners, mNearZ, mFarZ);
		}

		void getFrustumCorners(XMFLOAT3 corners[], f32 n, f32 f)
		{
			f32 tanHalfFov = tanf(mFovAngleY * 0.5f);
			f32 near_y = n * tanHalfFov;
			f32 far_y = f * tanHalfFov;
			f32 near_x = mAspectRatio * near_y;
			f32 far_x = mAspectRatio * far_y;

			XMMATRIX view = XMLoadFloat4x4(&mViewMatrix);
			XMVECTOR det = XMMatrixDeterminant(view);
			XMMATRIX invView = XMMatrixInverse(&det, view);

			corners[0] = XMFLOAT3(near_x, near_y, n);
			corners[1] = XMFLOAT3(near_x, -near_y, n);
			corners[2] = XMFLOAT3(-near_x, -near_y, n);
			corners[3] = XMFLOAT3(-near_x, near_y, n);
			corners[4] = XMFLOAT3(far_x, far_y, f);
			corners[5] = XMFLOAT3(far_x, -far_y, f);
			corners[6] = XMFLOAT3(-far_x, -far_y, f);
			corners[7] = XMFLOAT3(-far_x, far_y, f);

			for (u32 i = 0; i < 8; i++)
			{
				//XMVECTOR v = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
				XMVECTOR v = XMLoadFloat3(&corners[i]);
				v = XMVector3Transform(v, invView);
				XMStoreFloat3(&corners[i], v);
			}
		}




		const static u32 ACTIVE_CAMERA_ID = 128;
		const static u32 CASCADE_SHADOW_LEVEL = 4; 

	protected:
		
		void splitShadowCascadeSegments()
		{
			f32 f = mShadowRange;
			f32 n = mNearZ;
			f32 m = CASCADE_SHADOW_LEVEL;
			f32 f_div_n = f / n;
			f32 lamda = 0.5f;

			for (u32 i = 0; i <= CASCADE_SHADOW_LEVEL; i++)
			{
				f32 i_div_m = (f32)i / m;
				f32 c1 = n * powf(f_div_n, i_div_m);
				f32 c2 = n + (f - n)*i_div_m;
				mShadowSegments[i] = lamda * c1 + (1.0f - lamda) * c2;
			}
		}




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

		XMFLOAT3		mRight;		/* 摄像机的右向量 */
		XMFLOAT3		mUp;		/* 摄像机的上向量 */
		XMFLOAT3		mLook;		/* 摄像机的前向量 */

		f32				mShadowRange;	// shadow的最远距离
		f32				mShadowSegments[CASCADE_SHADOW_LEVEL + 1];
	};

}

#endif
