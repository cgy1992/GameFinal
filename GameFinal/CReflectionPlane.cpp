#include "stdafx.h"
#include "CReflectionPlane.h"

namespace gf
{
	CReflectionPlane::CReflectionPlane(
		ISceneManager* smgr,
		u32 id,
		XMFLOAT3 pos,
		XMFLOAT3 normal,
		XMFLOAT3 tangent,
		XMFLOAT2 size,
		u32 mapWidth, u32 mapHeight)
		: mSceneManager(smgr)
		, mId(id)
		, mPlaneSize(size)
		, mPosition(pos)
		, mMapWidth(mapWidth)
		, mMapHeight(mapHeight)
		, mReflectMapTexture(nullptr)
	{
		if (mMapWidth == 0) {
			mMapWidth = IDevice::getInstance()->getClientWidth();
		}

		if (mMapHeight == 0) {
			mMapHeight = IDevice::getInstance()->getClientHeight();
		}

		mNormal = math::VectorNormalize(normal);
		mPlaneEquation.x = mNormal.x;
		mPlaneEquation.y = mNormal.y;
		mPlaneEquation.z = mNormal.z;
		mPlaneEquation.w = -math::VectorDot(pos, mNormal);

		XMVECTOR n = XMLoadFloat3(&mNormal); // local y
		XMVECTOR t = XMLoadFloat3(&tangent); // local x
		XMVECTOR b = XMVector3Cross(t, n);   // local z
		b = XMVector3Normalize(b);
		t = XMVector3Cross(n, b);

		XMStoreFloat3(&mTangent, t);
		XMStoreFloat3(&mBinormal, b);
	}


	CReflectionPlane::~CReflectionPlane()
	{
		ITextureManager::getInstance()->destroy(mReflectMapTexture);
		mReflectMapTexture = NULL;
	}

	void CReflectionPlane::render(ICameraNode* viewCamera)
	{
		// visibility test
		math::SFrustum frustum = viewCamera->getFrustum();
		math::SOrientedBox obb = getOBB();

		if (math::IntersectOrientedBoxFrustum(obb, frustum) == math::EIS_OUTSIDE)
			return;

		XMFLOAT3 pos = viewCamera->getPosition();
		XMFLOAT3 look = viewCamera->getLookVector();
		XMFLOAT3 right = viewCamera->getRightVector();

		XMVECTOR pos_v = XMLoadFloat3(&pos);
		XMVECTOR look_v = XMLoadFloat3(&look); 
		//XMVECTOR right_v = XMLoadFloat3(&right);

		XMVECTOR reflectPlane = XMLoadFloat4(&mPlaneEquation);
		XMMATRIX M = XMMatrixReflect(reflectPlane);

		pos_v = XMVector3TransformCoord(pos_v, M);
		look_v = XMVector3TransformNormal(look_v, M);
		//right_v = XMVector3TransformNormal(right_v, M);

		XMFLOAT3 rpos, rlook;
		XMStoreFloat3(&rpos, pos_v);
		XMStoreFloat3(&rlook, look_v);

		ICameraNode* reflectCamera = mSceneManager->getActiveCameraNode();

		reflectCamera->setPosition(rpos);
		reflectCamera->look(rlook, XMFLOAT3(0, 1, 0));
		reflectCamera->updateAbsoluteTransformation();

		// store the view and proj matrix
		mViewMatrix = reflectCamera->getViewMatrix();
		mProjMatrix = reflectCamera->getProjMatrix();
		mViewProjMatrix = reflectCamera->getViewProjMatrix();

		ITextureManager* tmgr = ITextureManager::getInstance();

		// if the texture is empty
		if (!mReflectMapTexture) {

			std::string name = "_reflect_plane_map_";
			name += mId;

			IRenderTarget* pRenderTarget = tmgr->createRenderTarget(name, mMapWidth, mMapHeight);
			mReflectMapTexture = pRenderTarget->getTexture();
		}

		IRenderTarget* target = mReflectMapTexture->getRenderTarget();
		IVideoDriver* driver = IVideoDriver::getInstance();
		target->clear(driver->getClearColor());
		driver->clearDepthStencil(1.0f, 0);
		driver->setRenderTarget(mReflectMapTexture->getRenderTarget());

		mSceneManager->drawPass();
	}

	math::SOrientedBox CReflectionPlane::getOBB() const
	{
		math::SOrientedBox obb;
		obb.Axis[0] = mTangent;
		obb.Axis[1] = mNormal;
		obb.Axis[2] = mBinormal;

		obb.Center = mPosition;

		obb.Extents.x = mPlaneSize.x * 0.5f;
		obb.Extents.y = mPlaneSize.x * 0.001f < 0.01f ? mPlaneSize.x * 0.001f : 0.01f;
		obb.Extents.z = mPlaneSize.y * 0.5f;

		return obb;
	}

	

}

