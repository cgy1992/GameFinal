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

	void CDirectionalLightNode::generateShadowMap()
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
			}

			ICameraNode* camera = mSceneManager->getActiveCameraNode();
			camera->setPosition(mPosition);
			camera->setViewWidth(mShadowCameraViewWidth);
			camera->setViewHeight(mShadowCameraViewHeight);

			XMFLOAT3 lookdir(mLightData.Direction.x, mLightData.Direction.y, mLightData.Direction.z);
			camera->look(lookdir);

			camera->updateAbsoluteTransformation();
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
			driver->setViewport(0, 0, static_cast<f32>(mShadowMapWidth), static_cast<f32>(mShadowMapHeight));
			driver->setDepthStencilSurface(mShadowMap);
			driver->clearDepthStencil(1.0f, 0);
			mSceneManager->drawShadowMap(this);
		}
	}


}

