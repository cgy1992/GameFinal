#ifndef __OCTREE_TREE_INTERFACE_H__
#define __OCTREE_TREE_INTERFACE_H__

#include "IOctreeNode.h"

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
};


#endif
