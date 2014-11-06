#ifndef __CAMERA_NODE_CLASS_H__
#define __CAMERA_NODE_CLASS_H__

#include "ICameraNode.h"

namespace gf
{

	class CCameraNode : public ICameraNode
	{
	public:
		CCameraNode(ISceneNode* parent, ISceneManager* smgr,
			const XMFLOAT3& position,
			const XMFLOAT3& lookat,
			const XMFLOAT3& updir,
			f32 aspectRatio,
			f32 fov,
			f32 nearZ,
			f32 farZ,
			bool bPerspectiveProj)
			:ICameraNode(parent, smgr, position, aspectRatio, fov, nearZ, farZ, bPerspectiveProj)
		{
			XMVECTOR eye = XMLoadFloat3(&position);
			XMVECTOR target = XMLoadFloat3(&lookat);
			XMVECTOR look = target - eye;
			look = XMVector3Normalize(look);
			XMVECTOR up = XMLoadFloat3(&updir);

			XMVECTOR right = XMVector3Cross(up, look);
			right = XMVector3Normalize(right);

			up = XMVector3Cross(look, right);
			up = XMVector3Normalize(up);

			XMStoreFloat3(&mLook, look);
			XMStoreFloat3(&mUp, up);
			XMStoreFloat3(&mRight, right);
		}

		virtual void render(E_PIPELINE_USAGE usage)
		{
			
		}

		virtual void lookAt(const XMFLOAT3& lookat)
		{
			XMVECTOR eye = XMLoadFloat3(&mPosition);
			XMVECTOR target = XMLoadFloat3(&lookat);
			XMVECTOR look = target - eye;
			look = XMVector3Normalize(look);
			XMVECTOR up = XMLoadFloat3(&mUp);

			XMVECTOR right = XMVector3Cross(up, look);
			right = XMVector3Normalize(right);

			up = XMVector3Cross(look, right);
			up = XMVector3Normalize(up);

			XMStoreFloat3(&mLook, look);
			XMStoreFloat3(&mUp, up);
			XMStoreFloat3(&mRight, right);
		}

		virtual void look(const XMFLOAT3& dir)
		{
			XMVECTOR lookdir = XMLoadFloat3(&dir);
			lookdir = XMVector3Normalize(lookdir);
			XMVECTOR up = XMLoadFloat3(&mUp);

			XMVECTOR right = XMVector3Cross(up, lookdir);
			right = XMVector3Normalize(right);

			up = XMVector3Cross(lookdir, right);
			up = XMVector3Normalize(up);

			XMStoreFloat3(&mLook, lookdir);
			XMStoreFloat3(&mUp, up);
			XMStoreFloat3(&mRight, right);
		}

		/* move along camera's look vector */
		virtual void walk(f32 unit)
		{
			XMVECTOR look = XMLoadFloat3(&mLook);
			XMVECTOR position = XMLoadFloat3(&mPosition);
			position += look * unit;
			XMStoreFloat3(&mPosition, position);
		}

		/* move along camera's up vector */
		virtual void fly(f32 unit)
		{
			XMVECTOR up = XMLoadFloat3(&mUp);
			XMVECTOR position = XMLoadFloat3(&mPosition);
			position += up * unit;
			XMStoreFloat3(&mPosition, position);
		}

		/* move along camera's right vector */
		virtual void strafe(f32 unit)
		{
			XMVECTOR right = XMLoadFloat3(&mRight);
			XMVECTOR position = XMLoadFloat3(&mPosition);
			position += right * unit;
			XMStoreFloat3(&mPosition, position);
		}

		/* rotate along camera's right vector. */
		virtual void pitch(f32 angle)
		{
			XMVECTOR right = XMLoadFloat3(&mRight);
			XMVECTOR up = XMLoadFloat3(&mUp);
			XMVECTOR look = XMLoadFloat3(&mLook);

			/* construct the rotate matrix. */
			XMMATRIX matRot = XMMatrixRotationAxis(right, angle);

			up = XMVector3TransformNormal(up, matRot);
			look = XMVector3TransformNormal(look, matRot);

			XMStoreFloat3(&mUp, up);
			XMStoreFloat3(&mLook, look);
		}

		/* rotate along camera's up vector. */
		virtual void yaw(f32 angle)
		{
			XMVECTOR right = XMLoadFloat3(&mRight);
			XMVECTOR up = XMLoadFloat3(&mUp);
			XMVECTOR look = XMLoadFloat3(&mLook);

			/* construct the rotate matrix. */
			XMMATRIX matRot = XMMatrixRotationAxis(up, angle);

			right = XMVector3TransformNormal(right, matRot);
			look = XMVector3TransformNormal(look, matRot);

			XMStoreFloat3(&mRight, right);
			XMStoreFloat3(&mLook, look);
		}

		/* rotate along camera's look vector. */
		virtual void roll(f32 angle)
		{
			XMVECTOR right = XMLoadFloat3(&mRight);
			XMVECTOR up = XMLoadFloat3(&mUp);
			XMVECTOR look = XMLoadFloat3(&mLook);

			/* construct the rotate matrix. */
			XMMATRIX matRot = XMMatrixRotationAxis(look, angle);

			up = XMVector3TransformNormal(up, matRot);
			right = XMVector3TransformNormal(right, matRot);

			XMStoreFloat3(&mUp, up);
			XMStoreFloat3(&mRight, right);
		}

		virtual void setOrientation(CXMMATRIX M)
		{
			XMStoreFloat3(&mRight, M.r[0]);
			XMStoreFloat3(&mUp, M.r[1]);
			XMStoreFloat3(&mLook, M.r[2]);
		}

		virtual XMMATRIX getRelativeTransformation() const
		{
			XMMATRIX R = XMMatrixSet(
				mRight.x, mRight.y, mRight.z, 0.0f,
				mUp.x, mUp.y, mUp.z, 0.0f,
				mLook.x, mLook.y, mLook.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			XMMATRIX T = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

			return R * T;
		}

		virtual XMMATRIX getAbsoluteTransformation()
		{
			return XMLoadFloat4x4(&mAbsoluteTransformation);
		}

		virtual void updateAbsoluteTransformation()
		{
			XMMATRIX absoluteMatrix;
			if (mParent)
			{
				absoluteMatrix = XMMatrixMultiply(mParent->getAbsoluteTransformation(), getRelativeTransformation());
			}
			else {
				absoluteMatrix = getRelativeTransformation();
			}

			XMStoreFloat4x4(&mAbsoluteTransformation, absoluteMatrix);


			/* adjust right, up, look vectors, to prevent accumulated error. */
			XMVECTOR right = XMLoadFloat3(&mRight);
			XMVECTOR up = XMLoadFloat3(&mUp);
			XMVECTOR look = XMLoadFloat3(&mLook);
			XMVECTOR pos = XMLoadFloat3(&mPosition);

			look = XMVector3Normalize(look);
			up = XMVector3Cross(look, right);
			up = XMVector3Normalize(up);

			right = XMVector3Cross(up, look);
			right = XMVector3Normalize(right);

			XMStoreFloat3(&mLook, look);
			XMStoreFloat3(&mUp, up);
			XMStoreFloat3(&mRight, right);


			/* update the view-matrix, proj-matrix, view-proj-matrix */
			XMMATRIX view;

			view._11 = mRight.x;
			view._21 = mRight.y;
			view._31 = mRight.z;
			view._41 = -XMVectorGetX(XMVector3Dot(right, pos));

			view._12 = mUp.x;
			view._22 = mUp.y;
			view._32 = mUp.z;
			view._42 = -XMVectorGetX(XMVector3Dot(up, pos));

			view._13 = mLook.x;
			view._23 = mLook.y;
			view._33 = mLook.z;
			view._43 = -XMVectorGetX(XMVector3Dot(look, pos));

			view._14 = 0;
			view._24 = 0;
			view._34 = 0;
			view._44 = 1.0f;

			XMMATRIX proj = calcProjMatrix();

			XMStoreFloat4x4(&mViewMatrix, view);
			XMStoreFloat4x4(&mProjMatrix, proj);
			XMStoreFloat4x4(&mViewProjMatrix, view * proj);

			// update frustum
			XMFLOAT4X4 M = mViewProjMatrix;
			mFrustum.LeftPlane.x = M._14 + M._11;
			mFrustum.LeftPlane.y = M._24 + M._21;
			mFrustum.LeftPlane.z = M._34 + M._31;
			mFrustum.LeftPlane.w = M._44 + M._41;

			mFrustum.RightPlane.x = M._14 - M._11;
			mFrustum.RightPlane.y = M._24 - M._21;
			mFrustum.RightPlane.z = M._34 - M._31;
			mFrustum.RightPlane.w = M._44 - M._41;

			mFrustum.BottomPlane.x = M._14 + M._12;
			mFrustum.BottomPlane.y = M._24 + M._22;
			mFrustum.BottomPlane.z = M._34 + M._32;
			mFrustum.BottomPlane.w = M._44 + M._42;

			mFrustum.TopPlane.x = M._14 - M._12;
			mFrustum.TopPlane.y = M._24 - M._22;
			mFrustum.TopPlane.z = M._34 - M._32;
			mFrustum.TopPlane.w = M._44 - M._42;

			mFrustum.NearPlane.x = M._13;
			mFrustum.NearPlane.y = M._23;
			mFrustum.NearPlane.z = M._33;
			mFrustum.NearPlane.w = M._43;

			mFrustum.FarPlane.x = M._14 - M._13;
			mFrustum.FarPlane.y = M._24 - M._23;
			mFrustum.FarPlane.z = M._34 - M._33;
			mFrustum.FarPlane.w = M._44 - M._43;

			// normalize the normal of each plane
			for (u32 i = 0; i < 6; i++)
			{
				XMVECTOR planeVec = XMLoadFloat4(&mFrustum.Planes[i]);
				XMVECTOR recipLength = XMVector3ReciprocalLength(planeVec);
				XMVECTOR normPlaneVec = XMVectorMultiply(planeVec, recipLength);
				XMStoreFloat4(&mFrustum.Planes[i], normPlaneVec);
			}
		}

	protected:

		XMFLOAT3		mRight;		/* 摄像机的右向量 */
		XMFLOAT3		mUp;		/* 摄像机的上向量 */
		XMFLOAT3		mLook;		/* 摄像机的前向量 */

	};

}

#endif
