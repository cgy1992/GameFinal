#include "stdafx.h"
#include "CReflectionPlane.h"

namespace gf
{
	CReflectionPlane::CReflectionPlane(
		ISceneManager* smgr,
		u32 id,
		XMFLOAT4 plane, 
		f32 planeSizeX, f32 planeSizeZ,
		f32 mapWidth, f32 mapHeight)
		: mSceneManager(smgr)
		, mId(id)
		, mPlaneEquation(plane)
		, mPlaneSizeX(planeSizeX)
		, mPlaneSizeZ(planeSizeZ)
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
	}


	CReflectionPlane::~CReflectionPlane()
	{
		if (mReflectMapTexture) {
			ReleaseReferenceCounted(mReflectMapTexture);
		}
	}

	void CReflectionPlane::render(ICameraNode* viewCamera)
	{
		// visibility test
		math::SFrustum frustum = viewCamera->getFrustum();
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

	

}

