#ifndef __OCTREE_MANAGER_INTERFACE_H__
#define __OCTREE_MANAGER_INTERFACE_H__

#include "ISceneNode.h"



class IOctreeManager : public ISceneNode
{
public:
	IOctreeManager(ISceneNode* parent,
		ISceneManager* smgr,
		f32 width,
		f32 height,
		f32 depth,
		XMFLOAT3 center,
		u32 maxTreeHeight)
		:ISceneNode(parent, smgr)
		, mWidth(width)
		, mHeight(height)
		, mDepth(depth)
		, mCenter(center)
		, mMaxTreeHeight(maxTreeHeight)
	{
		
	}

	u32 getMaxTreeHeight() const
	{
		return mMaxTreeHeight;
	}

	virtual void OnRegisterSceneNode()
	{
		if (mVisible)
		{
			auto it = mChildren.begin();
			for (; it != mChildren.end(); ++it)
				(*it)->OnRegisterSceneNode();
		}
	}

	

protected:

	f32				mWidth;
	f32				mHeight;
	f32				mDepth;
	XMFLOAT3		mCenter;
	u32				mMaxTreeHeight;
};

#endif
