#ifndef __OCTREE_MANAGER_CLASS_H__
#define __OCTREE_MANAGER_CLASS_H__

#include "IOctreeManager.h"

class COctreeManager : public IOctreeManager
{
public:
	COctreeManager(ISceneNode* parent,
		ISceneManager* smgr,
		f32 width,
		f32 height,
		f32 depth,
		XMFLOAT3 center = XMFLOAT3(0, 0, 0),
		u32 maxTreeHeight = 8)
		:IOctreeManager(parent, smgr, width, height, 
		depth, center, maxTreeHeight)
	{
		
	}

	virtual void render()
	{

	}
};

#endif
