#include "stdafx.h"
#include "CDirectionalLightNode.h"
#include "CSceneManager.h"
#include "IDevice.h"

namespace gf
{
	CDirectionalLightNode::CDirectionalLightNode(ISceneNode* parent, ISceneManager* smgr,
		u32 id, const XMFLOAT3& direction) :ILightNode(parent, smgr, false, id)
		, mShadowMap(nullptr)
	{
		// Directional Light must be no-static.
		ZeroMemory(&mLightData, sizeof(SDirectionalLight));
		setDirection(direction);
		mInsideFrustum = true;

		mShadowMapWidth = IDevice::getInstance()->getClientWidth();
		mShadowMapHeight = IDevice::getInstance()->getClientHeight();
	}


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
				//mShadowMap = tmgr->createDepthStencilSurface(name, mShadowMapWidth, mShadowMapHeight, 32, 0);

				mShadowMap = tmgr->createTexture2DArray(name + std::string("d"), mShadowMapWidth, mShadowMapHeight,
					4, ETBT_SHADER_RESOURCE | ETBT_DEPTH_STENCIL, nullptr, 1, EGF_D32, 0);

			}


			IVideoDriver* driver = mSceneManager->getVideoDriver();
			driver->setPipelineUsage(EPU_DIR_SHADOW_MAP);
			driver->setViewport(0, 0, static_cast<f32>(mShadowMapWidth), static_cast<f32>(mShadowMapHeight));

			ICameraNode* shadowCamera = mSceneManager->getActiveCameraNode();
			shadowCamera->setPerspectiveProjection(false);

			//setCameraAttributes(shadowCamera, viewCamera);

			math::SAxisAlignedBox aabb;
			XMFLOAT3 corners[8];
			// get three axis of shadow camera.
			XMFLOAT3 lookdir(mLightData.Direction.x, mLightData.Direction.y, mLightData.Direction.z);
			shadowCamera->look(lookdir);
			XMFLOAT4X4 rot = shadowCamera->getRotationMatrix();
			XMMATRIX rotMatrix = XMLoadFloat4x4(&rot);
			//XMMATRIX invRotMatrix = XMMatrixTranspose(rotMatrix);
			XMVECTOR det = XMMatrixDeterminant(rotMatrix);
			XMMATRIX invRotMatrix = XMMatrixInverse(&det, rotMatrix);

			const u32 levels = ICameraNode::CASCADE_SHADOW_LEVEL;
			const f32* shadowSegments = viewCamera->getShadowSegments();

			for (u32 i = 0; i < levels; i++)
			{
				f32 n = shadowSegments[i];
				f32 f = shadowSegments[i + 1];
				viewCamera->getFrustumCorners(corners, n, f);

				for (u32 j = 0; j < 8; j++)
				{
					XMVECTOR v = XMLoadFloat3(&corners[j]);
					v = XMVector4Transform(v, rotMatrix);
					XMStoreFloat3(&corners[j], v);
				}

				// compute the AABB of the 8 corners.
				ComputeAabbFromPoints(&aabb, 8, corners);

				// 让摄像机位置在往后拉一点, 这样包含更多可能的shadow caster.
				f32 addedCameraDist = aabb.Extents.z * 0.2f;

				f32 farZ = aabb.Extents.z * 2.0f + addedCameraDist;
				f32 viewWidth = aabb.Extents.x * 2.0f;
				f32 viewHeight = aabb.Extents.y * 2.0f;

				XMFLOAT3 camPos = aabb.Center;
				camPos.z = aabb.Center.z - aabb.Extents.z - addedCameraDist;

				XMVECTOR camPos_v = XMVectorSet(camPos.x, camPos.y, camPos.z, 1.0f);
				camPos_v = XMVector4Transform(camPos_v, invRotMatrix);
				XMStoreFloat3(&camPos, camPos_v);

				shadowCamera->setNearZ(0);
				shadowCamera->setFarZ(farZ);
				shadowCamera->setViewWidth(viewWidth);
				shadowCamera->setViewHeight(viewHeight);
				shadowCamera->setPosition(camPos);
				shadowCamera->updateAbsoluteTransformation();

				// update shadow map transforms
				XMFLOAT4X4 viewProj = shadowCamera->getViewProjMatrix();
				XMMATRIX viewProjMatrix = XMLoadFloat4x4(&viewProj);
				XMMATRIX T(
					0.5f, 0.0f, 0.0f, 0.0f,
					0.0f, -0.5f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.5f, 0.5f, 0.0f, 1.0f);

				XMMATRIX viewProjTexMatrix = viewProjMatrix * T;
				XMStoreFloat4x4(&mShadowTransforms[i], viewProjTexMatrix);

				// draw shadow
				IDepthStencilSurface* pDepthStencilSurface = mShadowMap->getDepthStencilSurface(i);
				driver->setRenderTargetAndDepthStencil(nullptr, pDepthStencilSurface);
				driver->clearDepthStencil(1.0f, 0);
				mSceneManager->drawShadowMap(this);
			}
		}
	}

	void CDirectionalLightNode::setCameraAttributes(ICameraNode* shadowCamera, ICameraNode* viewCamera)
	{
		math::SAxisAlignedBox aabb;
		XMFLOAT3 corners[8];

		// get three axis of shadow camera.
		XMFLOAT3 lookdir(mLightData.Direction.x, mLightData.Direction.y, mLightData.Direction.z);
		shadowCamera->look(lookdir);

		//shadowCamera->look(XMFLOAT3(0, 0, 1.0f), XMFLOAT3(0, 1.0f, 0));
		
		XMFLOAT4X4 rot = shadowCamera->getRotationMatrix();
		XMMATRIX rotMatrix = XMLoadFloat4x4(&rot);
		//XMMATRIX invRotMatrix = XMMatrixTranspose(rotMatrix);
		XMVECTOR det = XMMatrixDeterminant(rotMatrix);
		XMMATRIX invRotMatrix = XMMatrixInverse(&det, rotMatrix);

		
		viewCamera->getFrustumCorners(corners, viewCamera->getNearZ(), viewCamera->getShadowRange());
		for (u32 i = 0; i < 8; i++)
		{
			XMVECTOR v = XMLoadFloat3(&corners[i]);
			v = XMVector4Transform(v, rotMatrix);
			XMStoreFloat3(&corners[i], v);
		}

		// compute the AABB of the 8 corners.
		ComputeAabbFromPoints(&aabb, 8, corners);

		f32 farZ = aabb.Extents.z * 2.0f;
		f32 viewWidth = aabb.Extents.x * 2.0f;
		f32 viewHeight = aabb.Extents.y * 2.0f;

		XMFLOAT3 camPos = aabb.Center;
		camPos.z = aabb.Center.z - aabb.Extents.z;

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

		XMFLOAT4X4 lightSpaceTransform = shadowCamera->getViewProjMatrix();
		XMMATRIX lightSpaceMatrix = XMLoadFloat4x4(&lightSpaceTransform);

		const u32 levels = ICameraNode::CASCADE_SHADOW_LEVEL;
		const f32* shadowSegments = viewCamera->getShadowSegments();

		for (u32 i = 0; i < levels; i++)
		{
			f32 n = shadowSegments[i];
			f32 f = shadowSegments[i + 1];
			viewCamera->getFrustumCorners(corners, n, f);

			// transform 8 corners to light proj space
			for (u32 j = 0; j < 8; j++)
			{
				XMVECTOR v = XMVectorSet(corners[j].x, corners[j].y, corners[j].z, 1.0f);
				v = XMVector4Transform(v, lightSpaceMatrix);
				XMVECTOR w = XMVectorSplatW(v);
				v = XMVectorDivide(v, w);
				XMStoreFloat3(&corners[j], v);
			}

			//find the AABB of 8 corners (in light proj space)
			ComputeAabbFromPoints(&aabb, 8, corners);
			
			f32 scaleX = 1.0f / aabb.Extents.x;
			f32 scaleY = 1.0f / aabb.Extents.y;
			f32 offsetX = -aabb.Center.x * scaleX;
			f32 offsetY = -aabb.Center.y * scaleY;
			f32 scaleZ = 1.0f / (aabb.Center.z + aabb.Extents.z);
			f32 offsetZ = 0;

			XMMATRIX cropMatrix(
				scaleX, 0, 0, 0,
				0, scaleY, 0, 0,
				0, 0, scaleZ, 0,
				offsetX, offsetY, offsetZ, 1.0f);
			
			XMMATRIX texMatrix(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);

			XMMATRIX combinedShadowMatrix = lightSpaceMatrix * cropMatrix * texMatrix;
			XMStoreFloat4x4(&mShadowCropTransforms[i], cropMatrix);
			XMStoreFloat4x4(&mShadowTransforms[i], combinedShadowMatrix);
		}
	}

	void CDirectionalLightNode::disableLightInScene(ILightNode* light)
	{
		CSceneManager* sceneManager = dynamic_cast<CSceneManager*>(mSceneManager);

		// remove from mDirectionalLights in sceneManager.
		std::list<ILightNode*>& dirLightsInScene = sceneManager->mDirectionalLights;
		for (auto it = dirLightsInScene.begin(); it != dirLightsInScene.end(); it++)
		{
			if (*it == this)
			{
				dirLightsInScene.erase(it);
				break;
			}
		}
	}

	void CDirectionalLightNode::enableLightInScene(ILightNode* light)
	{
		CSceneManager* sceneManager = dynamic_cast<CSceneManager*>(mSceneManager);

		// check if this is already enabled now.
		std::list<ILightNode*>& dirLightsInScene = sceneManager->mDirectionalLights;
		for (auto it = dirLightsInScene.begin(); it != dirLightsInScene.end(); it++)
		{
			if (*it == this)
				return;
		}

		dirLightsInScene.push_back(this);
	}

	CDirectionalLightNode::~CDirectionalLightNode()
	{
		CSceneManager* sceneManager = dynamic_cast<CSceneManager*>(mSceneManager);
		sceneManager->mLightNodes.erase(mId);
	}

	void CDirectionalLightNode::setVisible(bool visible)
	{
		mVisible = visible;
		if (mVisible)
		{
			enableLightInScene(this);
		}
		else
		{
			disableLightInScene(this);
		}
	}


}

