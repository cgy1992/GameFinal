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
			XMFLOAT3 center = XMFLOAT3(0, 0, 0),
			u32 maxTreeHeight = 8)
			:IOctreeManager(parent, smgr, width, height,
			depth, center, maxTreeHeight)
			, mRootNode(this, nullptr,
			XMFLOAT3(center.x - width * 0.5f, center.y - height * 0.5f, center.z - depth * 0.5f),
			XMFLOAT3(center.x + width * 0.5f, center.y + height * 0.5f, center.z + depth * 0.5f), 0)
		{

		}

		virtual void render(E_PIPELINE_USAGE)
		{

		}

		virtual void addChild(ISceneNode* child);

		virtual void addSceneNodeToOctree(ISceneNode* node);

		virtual void OnRegisterSceneNode(bool bRecursion = true);

		virtual void update(f32 delta = 0);
		
		virtual bool removeSceneNode(ISceneNode* node);

		virtual bool getNearLights(IMeshNode* node, E_LIGHT_TYPE lightType, std::vector<ILightNode*>& lights);

		virtual void getLightsInFrustum(const math::SFrustum& frustum, std::vector<ILightNode*>& pointLights);

		virtual ISceneNode* intersectRay(const math::SRay& ray, f32* pDist,
			u32 nodeType = ENT_SOLID_NODE | ENT_LIGHT_NODE) const;

		virtual ISceneNode* intersectRayWithTag(const math::SRay& ray, f32 *pDist, u32 tag,
			u32 nodeType = ENT_SOLID_NODE | ENT_LIGHT_NODE) const;

	private:

		virtual void buildStaticMeshAndLightsMapping(IMeshNode* mesh);

		virtual void buildStaticMeshAndLightsMapping(ILightNode* light);

		static void addLightToListIfVisible(ILightNode* light, const math::SFrustum& frustum, std::vector<ILightNode*>& pointLights);

		void recurseRemoveSceneNode(ISceneNode* node);

	protected:

		COctreeNode											mRootNode;
		// static mesh and static lights mapping.
		std::map<IMeshNode*, std::vector<ILightNode*>>		mStaticMeshLightMapping;
	};

}

#endif
