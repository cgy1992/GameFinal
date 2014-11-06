#include "stdafx.h"
#include "COctreeNode.h"
#include "IOctreeManager.h"
#include "ISceneManager.h"
#include "ICameraNode.h"
#include "xnacollision/xnacollision.h"

namespace gf
{
	COctreeNode::COctreeNode(IOctreeManager* octreeManager, 
		COctreeNode* parent, 
		XMFLOAT3 minCorner, XMFLOAT3 maxCorner, u32 treeHeight)
		:IOctreeNode(octreeManager, minCorner, maxCorner, treeHeight)
		, mParentNode(parent)
	{
		mAabb.Center = XMFLOAT3((mMaxCorner.x + mMinCorner.x) * 0.5f,
			(mMaxCorner.y + mMinCorner.y) * 0.5f,
			(mMaxCorner.z + mMinCorner.z) * 0.5f);

		mAabb.Extents = XMFLOAT3((mMaxCorner.x - mMinCorner.x) * 0.5f,
			(mMaxCorner.y - mMinCorner.y) * 0.5f,
			(mMaxCorner.z - mMinCorner.z) * 0.5f);

		memset(mChildrenNodes, 0, sizeof(mChildrenNodes));
	}

	void COctreeNode::addSceneNode(ISceneNode* node)
	{
		/* 已经到了树的最底层节点 */
		if (mTreeHeight == mOctreeManager->getMaxTreeHeight())
		{
			addSceneNodeToCurrent(node);
			return;
		}

		const math::SAxisAlignedBox& aabb = node->getAabb();

		//f32 mid_x = (mMinCorner.x + mMaxCorner.x) * 0.5f;
		//f32 mid_y = (mMinCorner.y + mMaxCorner.y) * 0.5f;
		//f32 mid_z = (mMinCorner.z + mMaxCorner.z) * 0.5f;

		E_OCTREE_QUADRANT quadrant = getQuadrant(node);
		
		//	if the aabb intersects the boundaries of any sub-octree
		//  suspend the node directly onto the current octree node
		if (quadrant == EOQ_CURRENT)
			addSceneNodeToCurrent(node);
		else
			addSceneNodeToChild(quadrant, node);
	}


	E_OCTREE_QUADRANT COctreeNode::getQuadrant(const ISceneNode* node) const
	{
		const math::SAxisAlignedBox& aabb = node->getAabb();

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
					return EOQ_LEFT_UP_FRONT;
				}
				//如果完全在后侧
				else if (aabb.Center.z - aabb.Extents.z > mid_z)
				{
					return EOQ_LEFT_UP_BACK;
				}

			}
			//如果完全在下侧
			else if (aabb.Center.y + aabb.Extents.y < mid_y)
			{
				//如果完全在前侧
				if (aabb.Center.z + aabb.Extents.z < mid_z)
				{
					return EOQ_LEFT_DOWN_FRONT;
				}
				//如果完全在后侧
				else if (aabb.Center.z - aabb.Extents.z > mid_z)
				{
					return EOQ_LEFT_DOWN_BACK;
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
					return EOQ_RIGHT_UP_FRONT;
				}
				//如果完全在后侧
				else if (aabb.Center.z - aabb.Extents.z > mid_z)
				{
					return EOQ_RIGHT_UP_BACK;
				}

			}
			//如果完全在下侧
			else if (aabb.Center.y + aabb.Extents.y < mid_y)
			{
				//如果完全在前侧
				if (aabb.Center.z + aabb.Extents.z < mid_z)
				{
					return EOQ_RIGHT_DOWN_FRONT;
				}
				//如果完全在后侧
				else if (aabb.Center.z - aabb.Extents.z > mid_z)
				{
					return EOQ_RIGHT_DOWN_BACK;
				}
			}
		}

		return EOQ_CURRENT;
	}



	void COctreeNode::addSceneNodeToChild(E_OCTREE_QUADRANT quadrant, ISceneNode* node)
	{
		if (!mChildrenNodes[quadrant])
		{
			mChildrenNodes[quadrant] = createChildNode(quadrant);
		}

		mChildrenNodes[quadrant]->addSceneNode(node);
	}

	COctreeNode* COctreeNode::createChildNode(E_OCTREE_QUADRANT quadrant)
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
			minCorner.y = mMinCorner.y;
			maxCorner.y = (mMinCorner.y + mMaxCorner.y) * 0.5f;
		}
		else
		{
			minCorner.y = (mMinCorner.y + mMaxCorner.y) * 0.5f;
			maxCorner.y = mMaxCorner.y;
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

		COctreeNode* octreeNode = new COctreeNode(mOctreeManager, this, minCorner, maxCorner, mTreeHeight + 1);
		return octreeNode;
	}

	void COctreeNode::removeSceneNodes()
	{
		mStaticMeshNodes.clear();
		mStaticLightNodes.clear();
		mDynamicMeshNodes.clear();
		mDynamicLightNodes.clear();

		for (u32 i = 0; i < 8; i++)
		{
			if (mChildrenNodes[i])
				mChildrenNodes[i]->removeSceneNodes();
		}
	}

	void COctreeNode::reset()
	{
		for (auto it = mDynamicMeshNodes.begin(); it != mDynamicMeshNodes.end(); it++)
			(*it)->setOctreeNode(nullptr);

		for (auto it = mDynamicLightNodes.begin(); it != mDynamicLightNodes.end(); it++)
			(*it)->setOctreeNode(nullptr);

		for (auto it = mStaticLightNodes.begin(); it != mStaticLightNodes.end(); it++)
			(*it)->setInsideFrustum(false);

		mDynamicMeshNodes.clear();
		mDynamicLightNodes.clear();

		for (u32 i = 0; i < 8; i++)
		{
			if (mChildrenNodes[i])
				mChildrenNodes[i]->reset();
		}
	}

	void COctreeNode::registerVisibleNodes(const math::SFrustum& frustum) const
	{
		if (math::IntersectAabbFrustum(mAabb, frustum) != math::EIS_OUTSIDE)
		{
			// register all the nodes in the containedNodes list
			// register mesh nodes.

			for (auto it = mStaticMeshNodes.begin(); it != mStaticMeshNodes.end(); it++)
			{
				(*it)->OnRegisterSceneNode(false);
			}

			for (auto it = mDynamicMeshNodes.begin(); it != mDynamicMeshNodes.end(); it++)
			{
				(*it)->OnRegisterSceneNode(false);
			}

			// register static lights for judging which light is inside frustum.
			for (auto it = mStaticLightNodes.begin(); it != mStaticLightNodes.end(); it++)
			{
				ILightNode* light = (*it);
				if (!light->isCulled(frustum))
					light->setInsideFrustum(true);
			}

			// recurse the eight sub-space
			for (u32 i = 0; i < 8; i++)
			{
				if (mChildrenNodes[i])
					mChildrenNodes[i]->registerVisibleNodes(frustum);
			}
		}
	}

	/*
		return a IOctreeNode pointer according to the position of the node
		but it doesn't mean this node is certainly contained in this octree node.
	*/
	IOctreeNode* COctreeNode::getBelongedOctreeNode(const ISceneNode* node)
	{
		E_OCTREE_QUADRANT quadrant = getQuadrant(node);
		if (quadrant == EOQ_CURRENT || !mChildrenNodes[quadrant])
			return this;

		return mChildrenNodes[quadrant]->getBelongedOctreeNode(node);
	}

	bool COctreeNode::removeSceneNode(ISceneNode* node)
	{
		if (!node->isStatic())
			return false;

		E_OCTREE_QUADRANT quadrant = getQuadrant(node);
		if (quadrant == EOQ_CURRENT || !mChildrenNodes[quadrant])
		{
			if (node->getNodeType() & ESNT_LIGHT)
			{
				for (auto it = mStaticLightNodes.begin(); it != mStaticLightNodes.end(); it++)
				{
					if (*it == node)
					{
						mStaticLightNodes.erase(it);
						return true;
					}
				}
			}
			else if (node->getNodeType() & ESNT_MESH)
			{
				for (auto it = mStaticMeshNodes.begin(); it != mStaticMeshNodes.end(); it++)
				{
					if (*it == node)
					{
						mStaticMeshNodes.erase(it);
						return true;
					}
				}
			}

			return false;
		}
		return mChildrenNodes[quadrant]->removeSceneNode(node);
	}

	void COctreeNode::addSceneNodeToCurrent(ISceneNode* node)
	{
		E_SCENE_NODE_TYPE nodeType = node->getNodeType();
		if (node->isStatic())
		{
			if (nodeType & ESNT_MESH)
			{
				IMeshNode* meshNode = (IMeshNode*)node;
				mStaticMeshNodes.push_back(meshNode);
				meshNode->setOctreeNode(this);
			}
			else if (nodeType & ESNT_LIGHT)
			{
				// directional light is not added
				ILightNode* light = (ILightNode*)node;
				if (light->getType() != ELT_DIRECTIONAL)
				{
					mStaticLightNodes.push_back(light);
					light->setOctreeNode(this);
				}
			}
		}
		else
		{
			if (nodeType & ESNT_MESH)
			{
				IMeshNode* meshNode = (IMeshNode*)node;
				mDynamicMeshNodes.push_back(meshNode);
				meshNode->setOctreeNode(this);
			}
			else if (nodeType & ESNT_LIGHT)
			{
				// directional light should not be added.
				ILightNode* light = (ILightNode*)node;
				if (light->getType() != ELT_DIRECTIONAL)
				{
					// when dynamic light is added, it should be checked whether culled by frustum.
					ISceneManager* smgr = node->getSceneManager();
					if (!smgr)
						return;

					ICameraNode* camera = smgr->getActiveCameraNode();
					if (!camera)
						return;

					const math::SFrustum& frustum = camera->getFrustum();
					if (!light->isCulled(frustum))
					{
						mDynamicLightNodes.push_back(light);
						light->setOctreeNode(this);
					}
				}
			}

		}
	}

	void COctreeNode::iterateChildren(ISceneNode* node, const XNA::OrientedBox& obb, const OctreeIterateCallback& callback)
	{
		XNA::AxisAlignedBox aabb = mAabb.getXnaAxisAlignedBox();
		if (XNA::IntersectAxisAlignedBoxOrientedBox(&aabb, &obb))
		{
			callback(node, this);
			for (u32 i = 0; i < 8; i++)
			{
				if (mChildrenNodes[i])
				{
					mChildrenNodes[i]->iterateChildren(node, obb, callback);
				}
			}
		}
	}

	void COctreeNode::iterateChildren(ISceneNode* node, const XNA::AxisAlignedBox& aabb, const OctreeIterateCallback& callback)
	{
		XNA::AxisAlignedBox octreeNodeAabb = mAabb.getXnaAxisAlignedBox();
		if (XNA::IntersectAxisAlignedBoxAxisAlignedBox(&octreeNodeAabb, &aabb))
		{
			callback(node, this);
			for (u32 i = 0; i < 8; i++)
			{
				if (mChildrenNodes[i])
				{
					mChildrenNodes[i]->iterateChildren(node, aabb, callback);
				}
			}
		}
	}

	void COctreeNode::iterateAncestors(ISceneNode* node, const OctreeIterateCallback& callback)
	{
		callback(node, this);
		if (mParentNode)
		{
			mParentNode->iterateAncestors(node, callback);
		}
	}

	bool COctreeNode::intersect(const math::SAxisAlignedBox& aabb)
	{
		XNA::AxisAlignedBox xna_aabb1 = mAabb.getXnaAxisAlignedBox();
		XNA::AxisAlignedBox xna_aabb2 = aabb.getXnaAxisAlignedBox();
		if (XNA::IntersectAxisAlignedBoxAxisAlignedBox(&xna_aabb1, &xna_aabb2))
			return true;
		return false;
	}



}
