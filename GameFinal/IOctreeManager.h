#ifndef __OCTREE_MANAGER_INTERFACE_H__
#define __OCTREE_MANAGER_INTERFACE_H__

#include "ISceneNode.h"
#include "IOctreeNode.h"

namespace gf
{

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
			:ISceneNode(parent, smgr, true)
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

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_OCTREE_MANAGER;
		}

		virtual void addSceneNodeToOctree(ISceneNode* node) = 0;

		/*
			remove 'node' and all its decendants from the octree manager
			ALWAYS use method to remove scene node from octree manager
			'remove' or 'removeChild' method should NEVER be called by the user
		*/
		virtual bool removeSceneNode(ISceneNode* node) = 0;

		virtual bool getNearLights(IMeshNode* node, E_LIGHT_TYPE lightType, std::vector<ILightNode*>& lights) = 0;

		virtual void getLightsInFrustum(const math::SFrustum& frustum, std::vector<ILightNode*>& pointLights) = 0;
		
		virtual ISceneNode* intersectRay(const math::SRay& ray, f32* pDist, 
			u32 nodeType = ENT_SOLID_NODE | ENT_LIGHT_NODE) const = 0;

	protected:

		f32				mWidth;
		f32				mHeight;
		f32				mDepth;
		XMFLOAT3		mCenter;
		u32				mMaxTreeHeight;

		// Is it a static octree or a dynamic octree?
		// "static" means the nodes inside the octree won't change in different frames
		// bool			mStaticOctree;
	};

}

#endif
