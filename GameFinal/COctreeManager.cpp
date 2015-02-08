#include "stdafx.h"
#include "COctreeManager.h"
#include "ISceneManager.h"
#include "COctreeNode.h"
#include "gfUtil.h"

namespace gf
{

	void COctreeManager::addChild(ISceneNode* child)
	{
		ISceneNode::addChild(child);
		child->update();

		// if this manager is a static octree manager,
		// child should be classified into concrete sub-space of octree according to its position IMMEDIATELY!

		addSceneNodeToOctree(child);

	}

	void COctreeManager::addSceneNodeToOctree(ISceneNode* node)
	{
		if (node && node->isVisible())
		{
			E_SCENE_NODE_TYPE nodeType = node->getNodeType();

			if (nodeType & ESNT_MESH || nodeType & ESNT_LIGHT)
				mRootNode.addSceneNode(node);

			if ((nodeType & ESNT_MESH) && node->isStatic())
			{
				buildStaticMeshAndLightsMapping((IMeshNode*)node);
			}
			else if ((nodeType & ESNT_LIGHT) && node->isStatic())
			{
				buildStaticMeshAndLightsMapping((ILightNode*)node);
			}

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
		ICameraNode* camera = mSceneManager->getActiveCameraNode();

		if (!camera)
			return;

		const math::SFrustum& frustum = camera->getFrustum();
		mRootNode.registerVisibleNodes(frustum);
	}

	void COctreeManager::update(f32 delta)
	{
		// don't need to update all nodes in it.
		// because all child nodes are static nodes.

		// delete all dynamic nodes in it
		mRootNode.reset();
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

	void COctreeManager::buildStaticMeshAndLightsMapping(IMeshNode* mesh)
	{
		// iterate all ancestor nodes in octree
		std::vector<ILightNode*> collectedlights;

		COctreeNode* octreeNode = (COctreeNode*)mesh->getOctreeNode();
		COctreeNode* parentOctreeNode = octreeNode->mParentNode;

		XNA::OrientedBox obb = mesh->getOrientedBox().getXnaOrientedBox();
		COctreeNode::OctreeIterateCallback callback = [&obb, &collectedlights](ISceneNode* node, COctreeNode* octreeNode)
		{
			for (auto it = octreeNode->mStaticLightNodes.begin(); it != octreeNode->mStaticLightNodes.end(); it++)
			{
				ILightNode* light = *it;
				if (light->intersect(obb))
				{
					collectedlights.push_back(light);
				}
			}
		};

		if (parentOctreeNode)
			parentOctreeNode->iterateAncestors(mesh, callback);

		octreeNode->iterateChildren(mesh, obb, callback);
		auto it = mStaticMeshLightMapping.find(mesh);
		if (it != mStaticMeshLightMapping.end())
		{
			mStaticMeshLightMapping.erase(it);
		}

		if (!collectedlights.empty())
		{
			mStaticMeshLightMapping.insert(std::make_pair(mesh, collectedlights));
		}
	}

	void COctreeManager::buildStaticMeshAndLightsMapping(ILightNode* light)
	{
		COctreeNode* octreeNode = (COctreeNode*)light->getOctreeNode();
		COctreeNode* parentOctreeNode = octreeNode->mParentNode;

		XNA::AxisAlignedBox aabb = light->getAabb().getXnaAxisAlignedBox();
		COctreeNode::OctreeIterateCallback callback = [this](ISceneNode* node, COctreeNode* octreeNode)
		{
			ILightNode* lightNode = (ILightNode*)node;
			for (auto it = octreeNode->mStaticMeshNodes.begin(); it != octreeNode->mStaticMeshNodes.end(); it++)
			{
				IMeshNode* meshNode = *it;
				XNA::OrientedBox obb = meshNode->getOrientedBox().getXnaOrientedBox();
				if (lightNode->intersect(obb))
				{
					mStaticMeshLightMapping[meshNode].push_back(lightNode);
				}
			}
		};

		if (parentOctreeNode)
			parentOctreeNode->iterateAncestors(light, callback);

		octreeNode->iterateChildren(light, aabb, callback);
	}

	bool COctreeManager::getNearLights(IMeshNode* node, E_LIGHT_TYPE lightType, std::vector<ILightNode*>& lights)
	{
		if (!node)
			return false;

		if (lightType == ELT_DIRECTIONAL)
			return false;

		COctreeNode* octreeNode = (COctreeNode*)node->getOctreeNode();
		COctreeNode* parentOctreeNode = octreeNode->mParentNode;
		XNA::OrientedBox obb = node->getOrientedBox().getXnaOrientedBox();

		// it node is static.
		if (node->isStatic())
		{
			// first collect all static lights.
			auto it = mStaticMeshLightMapping.find(node);
			if (it != mStaticMeshLightMapping.end())
			{
				const std::vector<ILightNode*>& nearStaticLights = it->second;
				for (u32 i = 0; i < nearStaticLights.size(); i++)
				{
					ILightNode* light = nearStaticLights[i];
					GF_ASSERT(light->isStatic());
					//if (light->insideFrustum())
					if (light->getType() == lightType &&  light->insideFrustum())
					{
						lights.push_back(light);
					}
				}
			}

			// second, collect all affected dynamic point lights.
			COctreeNode::OctreeIterateCallback callback = [&obb, &lights, lightType](ISceneNode* node, COctreeNode* octreeNode)
			{
				for (auto it = octreeNode->mDynamicLightNodes.begin(); it != octreeNode->mDynamicLightNodes.end(); it++)
				{
					ILightNode* light = *it;
					if (light->getType() == lightType && light->intersect(obb))
						lights.push_back(light);
				}
			};

			if (parentOctreeNode)
				parentOctreeNode->iterateAncestors(node, callback);

			octreeNode->iterateChildren(node, obb, callback);
		}
		else
		{
			// if it is a dynamic object, then just iterate all related lights in octree
			COctreeNode::OctreeIterateCallback callback = [&obb, &lights, lightType](ISceneNode* node, COctreeNode* octreeNode)
			{
				for (auto it = octreeNode->mStaticLightNodes.begin(); it != octreeNode->mStaticLightNodes.end(); it++)
				{
					ILightNode* light = *it;
					if (light->getType() == lightType && light->insideFrustum() && light->intersect(obb))
						lights.push_back(light);
				}

				for (auto it = octreeNode->mDynamicLightNodes.begin(); it != octreeNode->mDynamicLightNodes.end(); it++)
				{
					ILightNode* light = *it;
					if (light->getType() == lightType && light->intersect(obb))
						lights.push_back(light);
				}
			};

			if (parentOctreeNode)
				parentOctreeNode->iterateAncestors(node, callback);

			octreeNode->iterateChildren(node, obb, callback);
		}

		return true;
	}

	void COctreeManager::getLightsInFrustum(const math::SFrustum& frustum, std::vector<ILightNode*>& pointLights)
	{
		pointLights.clear();
		COctreeNode::OctreeIterateCallback callback = [&frustum, &pointLights](ISceneNode* node, COctreeNode* octreeNode)
		{
			for (auto it = octreeNode->mStaticLightNodes.begin(); it != octreeNode->mStaticLightNodes.end(); it++)
			{
				ILightNode* light = *it;
				addLightToListIfVisible(light, frustum, pointLights);
			}

			for (auto it = octreeNode->mDynamicLightNodes.begin(); it != octreeNode->mDynamicLightNodes.end(); it++)
			{
				ILightNode* light = *it;
				addLightToListIfVisible(light, frustum, pointLights);
			}
		};

		mRootNode.iterateChildren(nullptr, frustum, callback);
	}

	void COctreeManager::addLightToListIfVisible(ILightNode* light, 
		const math::SFrustum& frustum, 
		std::vector<ILightNode*>& pointLights)
	{
		switch (light->getType())
		{
		case ELT_POINT:
			if (light->isVisible() && !light->isCulled(frustum))
			{
				pointLights.push_back(light);
			}
			break;
		}
	}
}