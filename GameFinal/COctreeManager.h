#ifndef __OCTREE_MANAGER_CLASS_H__
#define __OCTREE_MANAGER_CLASS_H__

#include "IOctreeManager.h"
#include "COctreeNode.h"

namespace gf
{

	class COctreeManager : public IOctreeManager
	{
	public:
		COctreeManager(ISceneNode* parent,
			ISceneManager* smgr,
			f32 width,
			f32 height,
			f32 depth,
			bool staticOctree = true,
			XMFLOAT3 center = XMFLOAT3(0, 0, 0),
			u32 maxTreeHeight = 8)
			:IOctreeManager(parent, smgr, width, height,
			depth, staticOctree, center, maxTreeHeight)
			, mRootNode(this,
			XMFLOAT3(center.x - width * 0.5f, center.y - height * 0.5f, center.z - depth * 0.5f),
			XMFLOAT3(center.x + width * 0.5f, center.y + height * 0.5f, center.z + depth * 0.5f), 0)
		{

		}

		virtual void render()
		{

		}

		virtual void addChild(ISceneNode* child);

		void addSceneNodeToOctree(ISceneNode* node);

		virtual void OnRegisterSceneNode(bool bRecursion = true);

		virtual void OnAnimate(u32 timeMs);
		
		virtual bool removeSceneNode(ISceneNode* node);

	private:

		void recurseRemoveSceneNode(ISceneNode* node);

	protected:

		COctreeNode			mRootNode;
	};

}

#endif
