#include "stdafx.h"
#include "CLightNode.h"

/*
namespace gf
{

	void CLightNode::render()
	{

	}

	void CLightNode::OnRegisterSceneNode(bool bRecursion)
	{
		if (mVisible)
		{
			m_lightData.Position = getAbsolutePosition();
			XMVECTOR lightdir = XMVectorSet(m_direction.x, m_direction.y, m_direction.z, 0.0f);

			if (m_type != ELT_POINT)
			{
				XMMATRIX M = getAbsoluteTransformation();
				lightdir = XMVector3TransformNormal(lightdir, M);
				lightdir = XMVector3Normalize(lightdir);

				XMStoreFloat3(&m_lightData.Direction, lightdir);
			}

			mSceneManager->registerNodeForRendering(this);
			if (bRecursion)
				ISceneNode::OnRegisterSceneNode();
		}
	}

	bool CLightNode::isCulled(const math::SFrustum& frustum) const
	{
		if (m_type == ELT_DIRECTIONAL)
			return false;

		if (m_type == ELT_POINT)
		{
			math::SSphere sphere;
			sphere.Center = m_lightData.Position;
			sphere.Radius = m_lightData.Range;

			if (math::IntersectSphereFrustum(sphere, frustum) == math::EIS_OUTSIDE)
				return true;
			
			return false;
		}

		if (m_type == ELT_SPOT)
		{
			math::SOrientedBox obb;
			
			XMVECTOR dir = XMLoadFloat3(&m_lightData.Direction);
			//dir = XMVectorSetW(dir, 0);

			XMVECTOR yAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			XMVECTOR xAxis = XMVector3Cross(yAxis, dir);
			XMVECTOR xLength = XMVector3Length(xAxis);
			f32 length = XMVectorGetX(xLength);

			// yAxis may be parallel with dir
			if (length < 0.01f)
			{
				yAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
				xAxis = XMVector3Cross(yAxis, dir);
				//xLength = XMVector3Length(xAxis);
			}

			// normalize xAxis
			xAxis = XMVector3Normalize(xAxis);
			yAxis = XMVector3Cross(dir, xAxis);

			XMStoreFloat3(&obb.Axis[0], xAxis);
			XMStoreFloat3(&obb.Axis[1], yAxis);
			obb.Axis[2] = m_lightData.Direction;

			f32 radius = m_lightData.Range * tanf(m_lightData.Theta); 
			obb.Extents = XMFLOAT3(radius, radius, m_lightData.Range * 0.5f);

			if (math::IntersectOrientedBoxFrustum(obb, frustum) == math::EIS_OUTSIDE)
				return true;

			return false;
		}

		return false;
	}

}

*/