#include "stdafx.h"
#include "ISceneManager.h"
#include "CFpsCameraNode.h"

namespace gf
{
	CFpsCameraNode::CFpsCameraNode(ISceneNode* parent, ISceneManager* smgr,
		const XMFLOAT3& position,
		const XMFLOAT3& lookat,
		const XMFLOAT3& up,
		f32 aspectRatio,
		f32 fov,
		f32 nearZ,
		f32 farZ,
		f32 maxUpAngle,
		f32 maxDownAngle,
		bool bPerspectiveProj)
		:IFpsCameraNode(parent, smgr, position, aspectRatio, 
		fov, nearZ, farZ, maxUpAngle, maxDownAngle, bPerspectiveProj)
	{
		mPosition.y = mStandHeight;
		mHeight = mStandHeight;

		XMVECTOR worldup = XMLoadFloat3(&up);
		worldup = XMVector3Normalize(worldup);
		XMStoreFloat3(&mWorldUp, worldup);

		lookAt(lookat);

		mInputDriver = smgr->getDevice()->getInputDriver();
		mMouse = mInputDriver->getMouse();
		mKeyboard = mInputDriver->getKeyboard();

		// set the default values 
		mWalkSpeed = 4.0f;
		mStrafeSpeed = 2.0f;
		mRotateSpeed = 0.5f;
		mCrouchSpeed = 4.0f;
		mVerticalVelocity = 0.0f;
		mJumpSpeed = 4.0f;

		// set default action mappings.
		mActionMappings[EFCA_MOVE_FORWARD] = GVK_W;
		mActionMappings[EFCA_MOVE_BACK] = GVK_S;
		mActionMappings[EFCA_MOVE_LEFT] = GVK_A;
		mActionMappings[EFCA_MOVE_RIGHT] = GVK_D;
		mActionMappings[EFCA_JUMP] = GVK_SPACE;
		mActionMappings[EFCA_CREEP] = GVK_LCTRL;
	}


	void CFpsCameraNode::updateAbsoluteTransformation()
	{
		XMMATRIX absoluteMatrix;

		XMVECTOR worldup = XMLoadFloat3(&mWorldUp);
		XMVECTOR dir = XMLoadFloat3(&mWalkDir);
		//dir = XMVector3Normalize(dir);

		XMVECTOR right = XMVector3Cross(worldup, dir);
		right = XMVector3Normalize(right);

		/* rotate along the right vector, to get the look vector. */
		XMMATRIX rotMat = XMMatrixRotationNormal(right, mPitchAngle);
		XMVECTOR look = XMVector3TransformNormal(dir, rotMat);
		look = XMVector3Normalize(look);

		XMVECTOR up = XMVector3Cross(look, right);
		up = XMVector3Normalize(up);

		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mLook, look);
		XMStoreFloat3(&mRight, right);

		XMMATRIX R = XMMatrixSet(
			mRight.x, mRight.y, mRight.z, 0.0f,
			mUp.x, mUp.y, mUp.z, 0.0f,
			mLook.x, mLook.y, mLook.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		XMMATRIX T = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		XMMATRIX relativeMatrix = R * T;

		XMStoreFloat4x4(&mRelativeTransform, relativeMatrix);

		if (mParent)
		{
			absoluteMatrix = XMMatrixMultiply(mParent->getAbsoluteTransformation(), relativeMatrix);
		}
		else {
			absoluteMatrix = relativeMatrix;
		}

		XMStoreFloat4x4(&mAbsoluteTransformation, absoluteMatrix);

		// adjust the dir, to prevent the accumulated error.
		dir = XMVector3Normalize(XMVector3Cross(right, worldup));
		XMStoreFloat3(&mWalkDir, dir);

		/* update the view-matrix, proj-matrix, view-proj-matrix */
		XMMATRIX view;
		XMVECTOR pos = XMLoadFloat3(&mPosition);

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
		XMMATRIX viewProj = view * proj;
		XMStoreFloat4x4(&mViewProjMatrix, viewProj);

		XMVECTOR det = XMMatrixDeterminant(viewProj);
		XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);
		XMStoreFloat4x4(&mInvViewProjMatrix, invViewProj);

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

	void CFpsCameraNode::lookAt(const XMFLOAT3& lookat)
	{
		XMVECTOR eye = XMLoadFloat3(&mPosition);
		XMVECTOR target = XMLoadFloat3(&lookat);
		XMVECTOR look = XMVector3Normalize(target - eye);

		XMVECTOR worldup = XMLoadFloat3(&mWorldUp);
		XMVECTOR right = XMVector3Cross(worldup, look);
		right = XMVector3Normalize(right);

		XMVECTOR dir = XMVector3Cross(right, worldup);
		dir = XMVector3Normalize(dir);

		XMVECTOR angle = XMVector3AngleBetweenNormals(dir, look);

		/* 判断当前是抬头还是低头，如果是抬头，则angle角为负，否则为正
		这里根据look向量与worldup向量的点积判断，如果点积为正，则为抬头，
		如果点积为负，则为低头 */

		mPitchAngle = XMVectorGetX(angle);
		if (XMVectorGetX(XMVector3Dot(look, worldup)) > 0)
			mPitchAngle = -mPitchAngle;

		adjustPitchAngle();
		XMStoreFloat3(&mWalkDir, dir);
	}

	void CFpsCameraNode::lookAt(const XMFLOAT3& lookat, const XMFLOAT3& up)
	{
		lookAt(lookat, mUp);
	}

	void CFpsCameraNode::look(const XMFLOAT3& dir, const XMFLOAT3& up)
	{
		look(dir, mUp);
	}

	void CFpsCameraNode::look(const XMFLOAT3& d)
	{
		XMVECTOR lookdir = XMLoadFloat3(&d);
		lookdir = XMVector3Normalize(lookdir);
		XMVECTOR worldup = XMLoadFloat3(&mWorldUp);
		XMVECTOR right = XMVector3Cross(worldup, lookdir);
		right = XMVector3Normalize(right);

		XMVECTOR dir = XMVector3Cross(right, worldup);
		dir = XMVector3Normalize(dir);

		XMVECTOR angle = XMVector3AngleBetweenNormals(dir, lookdir);

		/* 判断当前是抬头还是低头，如果是抬头，则angle角为负，否则为正
		这里根据look向量与worldup向量的点积判断，如果点积为正，则为抬头，
		如果点积为负，则为低头 */

		mPitchAngle = XMVectorGetX(angle);
		if (XMVectorGetX(XMVector3Dot(lookdir, worldup)) > 0)
			mPitchAngle = -mPitchAngle;

		adjustPitchAngle();
		XMStoreFloat3(&mWalkDir, dir);
	}

	void CFpsCameraNode::yaw(f32 angle)
	{
		XMVECTOR up = XMLoadFloat3(&mWorldUp);
		XMVECTOR dir = XMLoadFloat3(&mWalkDir);

		/* construct the rotate matrix. */
		XMMATRIX matRot = XMMatrixRotationNormal(up, angle);

		dir = XMVector3TransformNormal(dir, matRot);
		dir = XMVector3Normalize(dir);

		XMStoreFloat3(&mWalkDir, dir);
	}

	void CFpsCameraNode::walk(f32 unit)
	{
		mPosition.x += mWalkDir.x * unit;
		mPosition.y += mWalkDir.y * unit;
		mPosition.z += mWalkDir.z * unit;
	}

	void CFpsCameraNode::fly(f32 unit)
	{
		mPosition.x += mWorldUp.x * unit;
		mPosition.y += mWorldUp.y * unit;
		mPosition.z += mWorldUp.z * unit;
	}

	void CFpsCameraNode::strafe(f32 unit)
	{
		XMVECTOR worldup = XMLoadFloat3(&mWorldUp);
		XMVECTOR dir = XMLoadFloat3(&mWalkDir);
		XMVECTOR right = XMVector3Cross(worldup, dir);

		XMVECTOR pos = XMLoadFloat3(&mPosition);
		pos += right * unit;

		XMStoreFloat3(&mPosition, pos);
	}

	void CFpsCameraNode::pitch(f32 angle)
	{
		mPitchAngle += angle;
		adjustPitchAngle();
	}

	void CFpsCameraNode::roll(f32 angle)
	{
		/* the roll of fps camera is meaningless. */
	}

	bool CFpsCameraNode::setActionMapping(E_FPS_CAMERA_ACTION action, u32 key)
	{
		if (key == GVK_UNDEFINED) {
			mActionMappings[action] = key;
			return true;
		}

		// check if this key has already been used.
		for (u32 i = 0; i < EFCA_COUNT; i++) {
			if (i != action && mActionMappings[i] == key)
				return false;
		}

		mActionMappings[action] = key;
		return true;
	}

	void CFpsCameraNode::update(f32 delta /*= 0*/)
	{
		// if this is active camera
		bool bActive = (mSceneManager->getActiveCameraNode() == this);

		if (bActive && mInputDriver) {

			u32 key;

			// move front
			key = mActionMappings[EFCA_MOVE_FORWARD];
			if (key != GVK_UNDEFINED && mKeyboard->isPressed(key))
				walk(mWalkSpeed * delta);

			// move back
			key = mActionMappings[EFCA_MOVE_BACK];
			if (key != GVK_UNDEFINED && mKeyboard->isPressed(key))
				walk(-mWalkSpeed * delta);

			// move left
			key = mActionMappings[EFCA_MOVE_LEFT];
			if (key != GVK_UNDEFINED && mKeyboard->isPressed(key))
				strafe(-mStrafeSpeed * delta);

			// move right
			key = mActionMappings[EFCA_MOVE_RIGHT];
			if (key != GVK_UNDEFINED && mKeyboard->isPressed(key))
				strafe(mStrafeSpeed * delta);

			// jump
			key = mActionMappings[EFCA_JUMP];
			if (key != GVK_UNDEFINED && mKeyboard->keyDown(key)) {
				GF_PRINT_CONSOLE_INFO("jump!!!!!!!!!!!!!!!!!!!");
				jump();
			}

			// creep
			key = mActionMappings[EFCA_CREEP];
			if (key != GVK_UNDEFINED && mKeyboard->keyDown(key)) {
				if (mCreeping)
					stand();
				else
					creep();
			}

			int x, y, z;
			mMouse->getMovement(x, y, z);

			pitch(mRotateSpeed * y * delta);
			yaw(mRotateSpeed * x * delta);

			if (mVerticalVelocity != 0) 
			{
				mVerticalVelocity -= 9.8f * delta;
				mHeight += mVerticalVelocity * delta;

				if (mHeight < mStandHeight)
				{
					mHeight = mStandHeight;
					mVerticalVelocity = 0;
				}
			}
			else
			{
				if (mCreeping) {
					// decrease the height
					if (mHeight - mCreepHeight > 0.001f) {
						mHeight = mHeight - mCrouchSpeed * delta;
					}
					else
					{
						mHeight = mCreepHeight;
					}
				}
				else
				{
					if (mHeight < mStandHeight - 0.001f) {
						mHeight = mHeight + mCrouchSpeed * delta;
					}
					else
					{
						mHeight = mStandHeight;
					}
				}
			}
		}

		mPosition.y = mHeight;

		f32 terrainHeight = 0;
		if (mSceneManager->getHeightOnTerrain(mPosition.x, mPosition.z, terrainHeight)) {
			mPosition.y += terrainHeight;
		}

		ISceneNode::update(delta);

		if (bActive) {
			// the sky dome should be moved with camera.
			IMeshNode* skydome = mSceneManager->getSkyNode();
			if (skydome) {
				XMFLOAT3 pos = getPosition();
				skydome->setPosition(pos.x, pos.y, pos.z);
			}
		}
	}

	void CFpsCameraNode::creep()
	{
		if (mVerticalVelocity == 0) {
			mCreeping = true;
		}
	}

	void CFpsCameraNode::stand()
	{
		mCreeping = false;
	}

	void CFpsCameraNode::jump()
	{
		// if creeping now, standing up 'suddenly'
		if (mCreeping) {
			mCreeping = false;
			mHeight = mStandHeight;
		}

		mVerticalVelocity = mJumpSpeed;
	}

}

