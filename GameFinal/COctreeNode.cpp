#include "stdafx.h"
#include "COctreeNode.h"
#include "IOctreeManager.h"
#include "xnacollision/xnacollision.h"

void COctreeNode::addSceneNode(ISceneNode* node)
{
	/* 已经到了树的最底层节点 */
	if (mTreeHeight == mOctreeManager->getMaxTreeHeight())
	{
		mContainedNodes.push_back(node);
		return;
	}

	const XNA::AxisAlignedBox& aabb = node->getAabb();

	f32 mid_x = (mMinCorner.x + mMaxCorner.x) * 0.5f;
	f32 mid_y = (mMinCorner.y + mMaxCorner.y) * 0.5f;
	f32 mid_z = (mMinCorner.z + mMaxCorner.z) * 0.5f;

	//如果完全在左侧
	if (aabb.Center.x + aabb.Extents.x < mid_x)
	{
		//如果完全在上侧
		if (aabb.Center.y - aabb.Extents.y > mid_y)
		{
			//如果完全在前侧
			if (aabb.Center.z + aabb.Extents.z < mid_z)
			{
				addSceneNodeToChild(EOQ_LEFT_UP_FRONT, node);
			}
			//如果完全在后侧
			else if (aabb.Center.z - aabb.Extents.z > mid_z)
			{
				addSceneNodeToChild(EOQ_LEFT_UP_BACK, node);
			}

		}
		//如果完全在下侧
		else if (aabb.Center.y + aabb.Extents.y < mid_y)
		{
			//如果完全在前侧
			if (aabb.Center.z + aabb.Extents.z < mid_z)
			{
				addSceneNodeToChild(EOQ_LEFT_DOWN_FRONT, node);
			}
			//如果完全在后侧
			else if (aabb.Center.z - aabb.Extents.z > mid_z)
			{
				addSceneNodeToChild(EOQ_LEFT_DOWN_BACK, node);
			}
		}
	}
	//如果完全在右侧
	else if (aabb.Center.x - aabb.Extents.x > mid_x)
	{
		//如果完全在上侧
		if (aabb.Center.y - aabb.Extents.y > mid_y)
		{
			//如果完全在前侧
			if (aabb.Center.z + aabb.Extents.z < mid_z)
			{
				addSceneNodeToChild(EOQ_RIGHT_UP_FRONT, node);
			}
			//如果完全在后侧
			else if (aabb.Center.z - aabb.Extents.z > mid_z)
			{
				addSceneNodeToChild(EOQ_RIGHT_UP_BACK, node);
			}

		}
		//如果完全在下侧
		else if (aabb.Center.y + aabb.Extents.y < mid_y)
		{
			//如果完全在前侧
			if (aabb.Center.z + aabb.Extents.z < mid_z)
			{
				addSceneNodeToChild(EOQ_RIGHT_DOWN_FRONT, node);
			}
			//如果完全在后侧
			else if (aabb.Center.z - aabb.Extents.z > mid_z)
			{
				addSceneNodeToChild(EOQ_RIGHT_DOWN_BACK, node);
			}
		}
	}
}



void COctreeNode::addSceneNodeToChild(E_OCTREE_QUADRANT quadrant, ISceneNode* node)
{
	if (!mChildrenNodes[quadrant])
	{
		mChildrenNodes[quadrant] = createChildNode(quadrant);
	}

	mChildrenNodes[quadrant]->addSceneNode(node);
}

IOctreeNode* COctreeNode::createChildNode(E_OCTREE_QUADRANT quadrant) const
{
	XMFLOAT3 minCorner, maxCorner;
	// 如果是在右侧的象限
	if (quadrant & 0x04)
	{
		minCorner.x = (mMinCorner.x + mMaxCorner.x) * 0.5f;
		maxCorner.x = mMaxCorner.x;
	}
	else
	{
		minCorner.x = mMinCorner.x;
		maxCorner.x = (mMinCorner.x + mMaxCorner.x) * 0.5f;
	}

	// 如果是在下侧的象限
	if (quadrant & 0x02)
	{
		minCorner.y = (mMinCorner.y + mMaxCorner.y) * 0.5f;
		maxCorner.y = mMaxCorner.y;
	}
	else
	{
		minCorner.y = mMinCorner.y;
		maxCorner.y = (mMinCorner.y + mMaxCorner.y) * 0.5f;
	}

	// 如果是背侧的象限
	if (quadrant & 0x01)
	{
		maxCorner.z = mMaxCorner.z;
		minCorner.z = (mMaxCorner.z + mMinCorner.z) * 0.5f;
	}
	else
	{
		maxCorner.z = (mMaxCorner.z + mMinCorner.z) * 0.5f;
		minCorner.z = mMinCorner.z;
	}

	IOctreeNode* octreeNode = new COctreeNode(mOctreeManager, minCorner, maxCorner, mTreeHeight + 1);
	return octreeNode;
}


