#include "stdafx.h"
#include "CPointLightNode.h"
#include "ISceneManager.h"

namespace gf
{
	void CPointLightNode::OnRegisterSceneNode(bool bRecursion)
	{
		if (mVisible)
		{
			mLightData.Position = getAbsolutePosition();

			computeBoundingVolumn();

			mSceneManager->registerNodeForRendering(this);
			if (bRecursion)
				ISceneNode::OnRegisterSceneNode();
		}
	}

	bool CPointLightNode::isCulled(const math::SFrustum& frustum) const
	{
		if (math::IntersectSphereFrustum(mSphere, frustum) == math::EIS_OUTSIDE)
			return true;

		return false;
	}

	void CPointLightNode::computeBoundingVolumn()
	{
		mSphere.Center = mLightData.Position;
		mSphere.Radius = mLightData.Range;

		mAabb.Center = mLightData.Position;
		mAabb.Extents = XMFLOAT3(mLightData.Range, mLightData.Range, mLightData.Range);
	}

	void CPointLightNode::getLightData(void* data) const
	{
		memcpy(data, &mLightData, sizeof(mLightData));
	}

	void CPointLightNode::update(u32 delta /* = 0 */)
	{
		ISceneNode::update(delta);
		computeBoundingVolumn();
	}

	bool CPointLightNode::intersect(const XNA::OrientedBox& obb) const
	{
		XNA::Sphere sphere = mSphere.getXnaSphere();
		if (XNA::IntersectSphereOrientedBox(&sphere, &obb))
			return true;

		return false;
	}

	ITexture* CPointLightNode::getShadowMap()
	{
		return mShadowMapTexture;
	}

	void CPointLightNode::generateShadowMap()
	{
		ITextureManager* tmgr = ITextureManager::getInstance();
		if (mCastShadow)
		{
			if (!mShadowMapTexture)
			{
				std::string name = "_point_shadow_map_";
				name += getId();

				mShadowMapTexture = tmgr->createTextureCube(name, mShadowMapWidth,
					ETBT_SHADER | ETBT_RENDER_TARGET, nullptr, 1, EGF_R32_FLOAT);
			}

			ICameraNode* camera = mSceneManager->getActiveCameraNode();
			camera->setPerspectiveProjection(true);

			XMFLOAT3 pos = getAbsolutePosition();
			camera->setPosition(pos);
			camera->setFovAngle(XM_PIDIV2);
			camera->setNearZ(0.1f);
			camera->setFarZ(mLightData.Range);
			camera->setAspectRatio(1.0f);

			//const 
			const static XMFLOAT3 looks[] = {
				XMFLOAT3(1.0f, 0, 0),					//+x
				XMFLOAT3(-1.0f, 0, 0),					//-x
				XMFLOAT3(0, 1.0f, 0),					//+y
				XMFLOAT3(0, -1.0f, 0),					//-y
				XMFLOAT3(0, 0, 1.0f),					//+z
				XMFLOAT3(0, 0, -1.0f)					//-z
			};

			const static XMFLOAT3 ups[] = {
				XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
				XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
				XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
				XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
				XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
				XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
			};

			IVideoDriver* driver = mSceneManager->getVideoDriver();
			IDepthStencilSurface* pDepthStencilSurface = tmgr->getTempDepthStencilSurface(
				mShadowMapWidth, mShadowMapWidth, 32, 0);
			
			driver->setPipelineUsage(EPU_POINT_SHADOW_MAP);

			for (u32 i = 0; i < 6; i++)
			{
				camera->look(looks[i], ups[i]);
				camera->updateAbsoluteTransformation();
				IRenderTarget* pRenderTarget = mShadowMapTexture->getRenderTarget(i);
				driver->setRenderTargetAndDepthStencil(pRenderTarget, pDepthStencilSurface);
				
				const f32 clearColor[] = { 1000.0f, 1000.0f, 1000.0f, 1000.0f };
				pRenderTarget->clear(clearColor);
				driver->clearDepthStencil(1.0f, 0);
				mSceneManager->drawShadowMap(this);
			}

			tmgr->releaseTempDepthStencilSurface(pDepthStencilSurface);
		}
	}

}
