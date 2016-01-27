#ifndef __IMESHNODE_H__
#define __IMESHNODE_H__

#include "IMesh.h"
#include "ISceneNode.h"
#include "IPipeline.h"
#include "IMaterial.h"
#include "ILightNode.h"
#include "IOctreeNode.h"
#include "IReflectionPlane.h"

namespace gf
{
	class IOctreeNode;

	class IMeshNode : public ISceneNode
	{
	public:
		IMeshNode(ISceneNode* parent,
			ISceneManager* smgr,
			bool bStatic,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:ISceneNode(parent, smgr, bStatic, position, rotation, scale)
			, mOctreeNode(nullptr)
			, mShadowFlag(0)
			, mMultiInstanced(false)
			, mFarCullingDist(0)
		{
			setRenderOrder(ERO_MESH);
		}

		virtual bool setMaterial(u32 subset, IMaterial* material) = 0;

		virtual bool setMaterial(IMaterial* material) = 0;

		virtual IMaterial* getMaterial(u32 subset = 0) = 0;

		virtual bool setMaterialName(const std::string& name) = 0;

		virtual bool setMaterialName(u32 subset, const std::string& name) = 0;

		virtual void render(E_PIPELINE_USAGE usage) = 0;

		virtual void renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer) = 0;

		virtual u32 getSubsetCount() const = 0;

		math::SOrientedBox computeOrientedBox(const math::SAxisAlignedBox& aabb) const
		{
			math::SOrientedBox obb;
			getLocalAxis(obb.Axis);
			obb.Center = aabb.Center;

			XMVECTOR center = XMVectorSet(obb.Center.x, obb.Center.y, obb.Center.z, 1.0f);
			XMMATRIX worldMatrix = getAbsoluteTransformation();
			center = XMVector3Transform(center, worldMatrix);
			XMStoreFloat3(&obb.Center, center);

			XMVECTOR axis, recipLength;

			axis = XMLoadFloat3(&obb.Axis[0]);
			recipLength = XMVector3ReciprocalLength(axis);
			axis = XMVectorMultiply(axis, recipLength);
			XMStoreFloat3(&obb.Axis[0], axis);
			f32 scaleX = 1.0f / XMVectorGetX(recipLength);
			obb.Extents.x = aabb.Extents.x * scaleX;


			axis = XMLoadFloat3(&obb.Axis[1]);
			recipLength = XMVector3ReciprocalLength(axis);
			axis = XMVectorMultiply(axis, recipLength);
			XMStoreFloat3(&obb.Axis[1], axis);
			f32 scaleY = 1.0f / XMVectorGetX(recipLength);
			obb.Extents.y = aabb.Extents.y * scaleY;

			axis = XMLoadFloat3(&obb.Axis[2]);
			recipLength = XMVector3ReciprocalLength(axis);
			axis = XMVectorMultiply(axis, recipLength);
			XMStoreFloat3(&obb.Axis[2], axis);
			f32 scaleZ = 1.0f / XMVectorGetX(recipLength);
			obb.Extents.z = aabb.Extents.z * scaleZ;

			//obb.Center.x += aabb.Center.x * scaleX;
			//obb.Center.y += aabb.Center.y * scaleY;
			//obb.Center.z += aabb.Center.z * scaleZ;

			return obb;
		}

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

		virtual IOctreeNode*	getOctreeNode()
		{
			return mOctreeNode;
		}

		virtual void setOctreeNode(IOctreeNode* octreeNode)
		{
			mOctreeNode = octreeNode;
		}

		void addShadow(u32 lightID)
		{
			mShadowFlag |= (1 << lightID);
		}

		void removeShadow(u32 lightID)
		{
			mShadowFlag &= (~(1 << lightID));
		}

		bool isShadowCaster(u32 lightID)
		{
			return (mShadowFlag & (1 << lightID)) != 0;
		}

		void setFarCullingDistance(f32 dist)
		{
			mFarCullingDist = dist;
		}

		f32 getFarCullingDistance() const
		{
			return mFarCullingDist;
		}

		virtual IMesh* getMesh() const = 0;

		virtual void setReflectionPlane(IReflectionPlane* plane) = 0;

		virtual IReflectionPlane* getReflectionPlane() = 0;

	protected:
		math::SOrientedBox				mOBB;
		IOctreeNode*					mOctreeNode; // belong to which node in an octree.
		f32								mFarCullingDist; // 最远裁剪距离, 如果为0, 则使用摄像机的farZ进行裁剪
		
		/* 一个32bit的flag, 要投影几号光源的阴影，就把哪一位设置为1 */
		u32								mShadowFlag;

		/* 该node 是否是多实例的. */
		bool							mMultiInstanced;
	};


}

#endif