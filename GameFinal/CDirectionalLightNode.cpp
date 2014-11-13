#include "stdafx.h"
#include "CDirectionalLightNode.h"
#include "ISceneManager.h"

namespace gf
{
	void CDirectionalLightNode::OnRegisterSceneNode(bool bRecursion)
	{
		if (mVisible)
		{
			XMVECTOR lightdir = XMLoadFloat4(&mLightData.Direction);

			XMMATRIX M = getAbsoluteTransformation();
			lightdir = XMVector3TransformNormal(lightdir, M);
			lightdir = XMVector3Normalize(lightdir);

			XMStoreFloat4(&mLightData.Direction, lightdir);

			mSceneManager->registerNodeForRendering(this);
			if (bRecursion)
				ISceneNode::OnRegisterSceneNode();
		}
	}

	void CDirectionalLightNode::getLightData(void* data) const
	{
		memcpy(data, &mLightData, sizeof(mLightData));
	}

	math::SAxisAlignedBox CDirectionalLightNode::getAabb() const
	{
		ISceneManager* sceneManager = this->getSceneManager();
		if (sceneManager)
		{
			return sceneManager->getAabb();
		}

		return ISceneNode::getAabb();
	}

	void CDirectionalLightNode::generateShadowMap(ICameraNode* viewCamera)
	{
		if (mCastShadow)
		{
			// if the shadow is empty
			if (!mShadowMap)
			{
				ITextureManager* tmgr = ITextureManager::getInstance();
				std::string name = "_shadow_map_surface_";
				name += getId();
				//mShadowMap = tmgr->createDepthStencilSurface(name, mShadowMapWidth, mShadowMapHeight, 32, 0, false, 0, 0, true, true);
				mShadowMap = tmgr->createDepthStencilSurface(name, mShadowMapWidth, mShadowMapHeight, 32, 0);

				ITexture2DArray* texture = tmgr->createTexture2DArray(name + std::string("d"), mShadowMapWidth, mShadowMapHeight,
					4, ETBT_SHADER | ETBT_DEPTH_STENCIL, nullptr, 1, EGF_D32, 0);

			}

			ICameraNode* camera = mSceneManager->getActiveCameraNode();
			setCameraAttributes(camera, viewCamera);

			/*
			camera->setPerspectiveProjection(false);
			camera->setPosition(mPosition);
			camera->setViewWidth(mShadowCameraViewWidth);
			camera->setViewHeight(mShadowCameraViewHeight);
			camera->setNearZ(0);
			camera->setFarZ(1000.0f);

			XMFLOAT3 lookdir(mLightData.Direction.x, mLightData.Direction.y, mLightData.Direction.z);
			camera->look(lookdir);

			camera->updateAbsoluteTransformation();
			*/
			XMFLOAT4X4 viewProj = camera->getViewProjMatrix();
			
			XMMATRIX viewProjMatrix = XMLoadFloat4x4(&viewProj);
			XMMATRIX T(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);

			XMMATRIX viewProjTexMatrix = viewProjMatrix * T;
			XMStoreFloat4x4(&mShadowMapTransform, viewProjTexMatrix);


			IVideoDriver* driver = mSceneManager->getVideoDriver();

			// set view port
			driver->setPipelineUsage(EPU_DIR_SHADOW_MAP);

			SViewport preViewport = driver->getViewport();

			SViewport viewport;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = static_cast<f32>(mShadowMapWidth);
			viewport.Height = static_cast<f32>(mShadowMapHeight);
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1.0f;
			driver->setViewport(viewport);
			//driver->setViewport(0, 0, static_cast<f32>(mShadowMapWidth), static_cast<f32>(mShadowMapHeight));
			//driver->setDepthStencilSurface(mShadowMap);
			driver->setRenderTargetAndDepthStencil(nullptr, mShadowMap);
			driver->clearDepthStencil(1.0f, 0);
			mSceneManager->drawShadowMap(this);

			driver->setViewport(preViewport);
		}
	}

	void CDirectionalLightNode::setCameraAttributes(ICameraNode* shadowCamera, ICameraNode* viewCamera)
	{
		// get three axis of shadow camera.
		XMFLOAT3 lookdir(mLightData.Direction.x, mLightData.Direction.y, mLightData.Direction.z);
		shadowCamera->look(lookdir);

		//shadowCamera->look(XMFLOAT3(0, 0, 1.0f), XMFLOAT3(0, 1.0f, 0));
		
		XMFLOAT4X4 rot = shadowCamera->getRotationMatrix();
		XMMATRIX rotMatrix = XMLoadFloat4x4(&rot);
		//XMMATRIX invRotMatrix = XMMatrixTranspose(rotMatrix);
		XMVECTOR det = XMMatrixDeterminant(rotMatrix);
		XMMATRIX invRotMatrix = XMMatrixInverse(&det, rotMatrix);

		// get 8 corners of viewCamera' frustum
		static XMFLOAT3 projConners[8] = {
			XMFLOAT3(-1.0f, 1.0f, 0),
			XMFLOAT3(1.0f, 1.0f, 0),
			XMFLOAT3(1.0f, -1.0f, 0),
			XMFLOAT3(-1.0f, -1.0f, 0),
			XMFLOAT3(-1.0f, 1.0f, 1.0f),
			XMFLOAT3(1.0f, 1.0f, 1.0f),
			XMFLOAT3(1.0f, -1.0f, 1.0f),
			XMFLOAT3(-1.0f, -1.0f, 1.0f)
		};

		//XMFLOAT4X4 view = viewCamera->getViewMatrix();
		// 这里不用摄像机的proj矩阵，因为farZ太大的话，导致阴影范围太大
		// 这里farZ用shadowRange代替
		//XMFLOAT4X4 proj = viewCamera->getProjMatrix(viewCamera->getNearZ(), viewCamera->getShadowRange());
		//XMFLOAT4X4 proj = viewCamera->getProjMatrix();
		//XMMATRIX viewMatrix = XMLoadFloat4x4(&view);
		//XMMATRIX projMatrix = XMLoadFloat4x4(&proj);
		//XMMATRIX viewProjMatrix = viewMatrix*projMatrix;
		//det = XMMatrixDeterminant(viewProjMatrix);
		//XMMATRIX invViewProjMatrix = XMMatrixInverse(&det, viewProjMatrix);

		// first from proj space to world space, then to shadow camera space.
		//XMMATRIX combineMatrix = invViewProjMatrix * rotMatrix;
		XMFLOAT3 corners[8];
		viewCamera->getFrustumCorners(corners, viewCamera->getNearZ(), viewCamera->getShadowRange());
		for (u32 i = 0; i < 8; i++)
		{
			XMVECTOR v = XMLoadFloat3(&corners[i]);
			v = XMVector4Transform(v, rotMatrix);
			XMStoreFloat3(&corners[i], v);
		}


		/*
		XMFLOAT3 corners2[8];
		for (u32 i = 0; i < 8; i++)
		{
			XMVECTOR p = XMVectorSet(projConners[i].x, projConners[i].y, projConners[i].z, 1.0f);
			p = XMVector4Transform(p, combineMatrix);
			f32 inv_w = 1.0f / XMVectorGetW(p);
			XMVECTOR inv_w_vec = XMVectorSet(inv_w, inv_w, inv_w, inv_w);
			p = XMVectorMultiply(p, inv_w_vec);
			XMStoreFloat3(&corners2[i], p);
		}
		*/

		// compute the AABB of the 8 corners.
		f32 min_x, min_y, min_z;
		f32 max_x, max_y, max_z;
		min_x = min_y = min_z = FLT_MAX;
		max_x = max_y = max_z = -FLT_MAX;

		for (u32 i = 0; i < 8; i++)
		{
			XMFLOAT3 p = corners[i];
			if (p.x < min_x)
				min_x = p.x;
			if (p.x > max_x)
				max_x = p.x;
			if (p.y < min_y)
				min_y = p.y;
			if (p.y > max_y)
				max_y = p.y;
			if (p.z < min_z)
				min_z = p.z;
			if (p.z > max_z)
				max_z = p.z;
		}

		f32 farZ = max_z - min_z;
		f32 viewWidth = max_x - min_x;
		f32 viewHeight = max_y - min_y;

		XMFLOAT3 camPos;
		camPos.x = (min_x + max_x) * 0.5f;
		camPos.y = (min_y + max_y) * 0.5f;
		camPos.z = min_z;

		XMVECTOR camPos_v = XMVectorSet(camPos.x, camPos.y, camPos.z, 1.0f);
		camPos_v = XMVector4Transform(camPos_v, invRotMatrix);
		XMStoreFloat3(&camPos, camPos_v);

		shadowCamera->setPerspectiveProjection(false);
		shadowCamera->setNearZ(0);
		shadowCamera->setFarZ(farZ);
		shadowCamera->setViewWidth(viewWidth);
		shadowCamera->setViewHeight(viewHeight);
		shadowCamera->setPosition(camPos);
		shadowCamera->updateAbsoluteTransformation();

		

		/*
		XMFLOAT4X4 view = viewCamera->getViewMatrix();
		XMFLOAT4X4 proj = viewCamera->getProjMatrix();
		XMMATRIX viewMatrix = XMLoadFloat4x4(&view);
		XMMATRIX projMatrix = XMLoadFloat4x4(&proj);
		XMVECTOR det = XMMatrixDeterminant(viewMatrix);
		XMMATRIX invViewMatrix = XMMatrixInverse(&det, viewMatrix);
		det = XMMatrixDeterminant(projMatrix);
		XMMATRIX invProjMatrix = XMMatrixInverse(&det, projMatrix);

		// first from proj space to world space, then to shadow camera space.
		XMMATRIX combineMatrix = invViewMatrix * rotMatrix;
		XMFLOAT3 corners[8];
		for (u32 i = 0; i < 8; i++)
		{
			XMVECTOR p = XMVectorSet(projConners[i].x, projConners[i].y, projConners[i].z, 1.0f);
			p = XMVector4Transform(p, invProjMatrix);
			f32 inv_w = 1.0f / XMVectorGetW(p);
			XMVECTOR inv_w_vec = XMVectorSet(inv_w, inv_w, inv_w, inv_w);
			p = XMVectorMultiply(p, inv_w_vec);
			p = XMVector4Transform(p, combineMatrix);

			XMStoreFloat3(&corners[i], p);
		}
		*/
	}
}

