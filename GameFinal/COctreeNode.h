#ifndef __OCTREE_TREE_INTERFACE_H__
#define __OCTREE_TREE_INTERFACE_H__

#include "IOctreeNode.h"

namespace gf
{

	class COctreeNode : public IOctreeNode
	{
	public:
		COctreeNode(IOctreeManager* octreeManager,
			XMFLOAT3 minCorner,
			XMFLOAT3 maxCorner,
			u32 treeHeight)
			:IOctreeNode(octreeManager, minCorner, maxCorner, treeHeight)
		{

		}

		virtual void addSceneNode(ISceneNode* node);

		void addSceneNodeToChild(E_OCTREE_QUADRANT, ISceneNode* node);

		IOctreeNode* createChildNode(E_OCTREE_QUADRANT) const;

		virtual void removeSceneNodes();

		virtual void registerVisibleNodes(const math::SFrustum& frustum) const;

		E_OCTREE_QUADRANT getQuadrant(const ISceneNode* node) const;

		IOctreeNode* getBelongedOctreeNode(const ISceneNode* node);

		virtual bool removeSceneNode(ISceneNode* node);

	protected:
	
		
	};

}

#endif
