#ifndef __OCTREE_NODE_CLASS_H__
#define __OCTREE_NODE_CLASS_H__

#include "IReferenceCounted.h"
#include "ISceneNode.h"

class IOctreeManager;

enum E_OCTREE_QUADRANT
{
	EOQ_LEFT_UP_FRONT = 0,	// 000
	EOQ_LEFT_UP_BACK,		// 001
	EOQ_LEFT_DOWN_FRONT,	// 010
	EOQ_LEFT_DOWN_BACK,		// 011
	EOQ_RIGHT_UP_FRONT,		// 100
	EOQ_RIGHT_UP_BACK,		// 101
	EOQ_RIGHT_DOWN_FRONT,	// 110
	EOQ_RIGHT_DOWN_BACK,	// 111
};

class IOctreeNode : public IReferenceCounted
{
public:
	IOctreeNode(IOctreeManager* octreeManager, 
		XMFLOAT3 minCorner, 
		XMFLOAT3 maxCorner,
		u32 treeHeight)
		:mOctreeManager(octreeManager),
		mMinCorner(minCorner),
		mMaxCorner(maxCorner),
		mTreeHeight(treeHeight)
	{
		memset(mChildrenNodes, 0, sizeof(mChildrenNodes));
	}

	virtual void addSceneNode(ISceneNode* node) = 0;

	u32	getTreeHeight() const
	{
		return mTreeHeight;
	}



protected:
	IOctreeManager*					mOctreeManager;
	
	XMFLOAT3						mMinCorner; 
	XMFLOAT3						mMaxCorner;

	u32								mTreeHeight;

	std::list<ISceneNode*>			mContainedNodes;
	IOctreeNode*					mChildrenNodes[8]; 



};

#endif
