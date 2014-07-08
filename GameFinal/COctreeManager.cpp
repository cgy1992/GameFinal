#include "stdafx.h"
#include "COctreeManager.h"
#include "ISceneManager.h"

namespace gf
{

	void COctreeManager::addChild(ISceneNode* child)
	{
		ISceneNode::addChild(child);
		child->update();

		// if this manager is a static octree manager,
		// child should be classified into concrete sub-space of octree according to its position IMMEDIATELY!
		if (mStaticOctree)
		{
			addSceneNodeToOctree(child);
		}
	}

	void COctreeManager::addSceneNodeToOctree(ISceneNode* node)
	{
		if (node && node->isVisible())
		{
			if (node->getNodeType() & ESNT_MESH)
				mRootNode.addSceneNode(node);

			ISceneNode::CSceneNodeIterator& it = node->getChildrenIterator();
			while (it.hasNext())
			{
				ISceneNode* child = it.next();
				addSceneNodeToOctree(child);
			}
		}
	}


	void COctreeManager::OnRegisterSceneNode(bool bRecursion)
	{
		// if this is dynamic octree, every frame it should be update
		// first remove all scene nodes from octree,
		// and then re-add them to the octree
		if (mVisible && !mStaticOctree)
		{
			mRootNode.removeSceneNodes();

			// add all children to octree
			for (auto it = mChildren.begin(); it != mChildren.end(); it++)
			{
				addSceneNodeToOctree(*it);
			}
		}

		ICameraNode* camera = mSceneManager->getActiveCameraNode();

		if (!camera)
			return;

		const math::SFrustum& frustum = camera->getFrustum();

		mRootNode.registerVisibleNodes(frustum);
	}

	void COctreeManager::update(u32 delta)
	{
		if (mVisible && !mStaticOctree)
		{
			updateAbsoluteTransformation();

			auto it = mChildren.begin();
			for (; it != mChildren.end(); ++it)
				(*it)->update();
		}
	}

	bool COctreeManager::removeSceneNode(ISceneNode* node)
	{
		// check if the node is really in the octree manager 
		ISceneNode* ancestor = node->getParent();
		while (ancestor != this && ancestor != nullptr)
			ancestor = ancestor->getParent();

		// if the node is not in the octree manager
		if (ancestor == nullptr)
			return false;

		recurseRemoveSceneNode(node);
		node->remove();
		return true;
	}

	void COctreeManager::recurseRemoveSceneNode(ISceneNode* node)
	{
		mRootNode.removeSceneNode(node);
		ISceneNode::CSceneNodeIterator& it = node->getChildrenIterator();
		while (it.hasNext())
		{
			ISceneNode* child = it.next();
			recurseRemoveSceneNode(child);
		}
	}

}