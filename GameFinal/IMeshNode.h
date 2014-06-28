#ifndef __IMESHNODE_H__
#define __IMESHNODE_H__

#include "IMesh.h"
#include "ISceneNode.h"
#include "IPipeline.h"
#include "IMaterial.h"

namespace gf
{

	class IMeshNode : public ISceneNode
	{
	public:
		IMeshNode(ISceneNode* parent,
			ISceneManager* smgr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:ISceneNode(parent, smgr, position, rotation, scale)
		{

		}

		virtual bool setMaterial(IMaterial* material, u32 subset = 0) = 0;

		virtual IMaterial* getMaterial(u32 subset = 0) = 0;

		virtual bool setMaterialName(const std::string& name, u32 subset = 0) = 0;

		virtual void render() = 0;

		virtual u32 getSubsetCount() const = 0;

		virtual math::SOrientedBox getOrientedBox() const
		{
			return mOBB;
		}

		virtual math::SAxisAlignedBox getAabb() const
		{
			// get all eight corners of obb
			const math::SOrientedBox& obb = getOrientedBox();
			math::SAxisAlignedBox aabb;
			math::ComputeAabbFromOrientedBox(&aabb, obb);
			return aabb;
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_MESH;
		}

	protected:
		math::SOrientedBox				mOBB;
	};


}

#endif