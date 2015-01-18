#ifndef __OCTREE_TREE_INTERFACE_H__
#define __OCTREE_TREE_INTERFACE_H__

#include "IOctreeNode.h"

namespace gf
{
	class COctreeNode : public IOctreeNode
	{
		friend class COctreeManager;

		typedef std::function<void(ISceneNode*, COctreeNode*)> OctreeIterateCallback;

	public:
		COctreeNode(IOctreeManager* octreeManager,
			COctreeNode* parent,
			XMFLOAT3 minCorner,
			XMFLOAT3 maxCorner,
			u32 treeHeight);

		virtual void addSceneNode(ISceneNode* node);

		void addSceneNodeToCurrent(ISceneNode* node);

		void addSceneNodeToChild(E_OCTREE_QUADRANT, ISceneNode* node);

		COctreeNode* createChildNode(E_OCTREE_QUADRANT);

		virtual void removeSceneNodes();

		virtual void reset();

		virtual void registerVisibleNodes(const math::SFrustum& frustum) const;

		E_OCTREE_QUADRANT getQuadrant(const ISceneNode* node) const;

		IOctreeNode* getBelongedOctreeNode(const ISceneNode* node);

		virtual bool removeSceneNode(ISceneNode* node);

		// whether a scene-node is in current octree-node.
		virtual bool intersect(const math::SAxisAlignedBox& aabb);

	protected:
	
		virtual void iterateChildren(ISceneNode* node, const XNA::OrientedBox& obb, const OctreeIterateCallback& callback);
		virtual void iterateChildren(ISceneNode* node, const XNA::AxisAlignedBox& aabb, const OctreeIterateCallback& callback);
		virtual void iterateChildren(ISceneNode* node, const math::SFrustum& frustum, const OctreeIterateCallback& callback);

		virtual void iterateAncestors(ISceneNode* node, const OctreeIterateCallback& callback);
		

	public:
		std::list<IMeshNode*>			mStaticMeshNodes;
		std::list<ILightNode*>			mStaticLightNodes;
		std::list<IMeshNode*>			mDynamicMeshNodes;
		std::list<ILightNode*>			mDynamicLightNodes;

		COctreeNode*					mChildrenNodes[8];
		COctreeNode*					mParentNode;

	};

}

#endif
