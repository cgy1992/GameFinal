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
		return nullptr;
	}

}
