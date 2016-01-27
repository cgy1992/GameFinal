#ifndef __INSTANCE_COLLECTION_NODE_CLASS_H__
#define __INSTANCE_COLLECTION_NODE_CLASS_H__

#include "IInstanceCollectionNode.h"

namespace gf
{
	class IInstanceNode;

	class CInstanceCollectionNode : public IInstanceCollectionNode
	{
	public:
		CInstanceCollectionNode(ISceneNode* parent,
			ISceneManager* smgr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:IInstanceCollectionNode(parent, smgr, position, rotation, scale)
			, mMeshNode(nullptr)
			, mInstanceBuffer(nullptr)
			, mInstancesData(nullptr)
			, mInstanceNum(0)
		{
			mNeedCulling = false;
		}

		/* 这个函数非常重要，如果aabb太小，InstanceCollectionNode会被八叉树裁剪，所有的instance都无法绘制 */
		virtual math::SAxisAlignedBox getAabb() const;

		virtual void updateAbsoluteTransformation()
		{
			ISceneNode::updateAbsoluteTransformation();

			const math::SAxisAlignedBox& aabb = mMesh->getAabb();
			mOBB = computeOrientedBox(aabb);
		}

		bool init(IMesh* mesh, u32 maxInstanceNum, u32 eachInstanceDataSize);

		virtual IInstanceNode* addInstance(bool bStatic, 
			const XMFLOAT3& position, 
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0, 1.0));

		virtual bool setMaterial(u32 subset, IMaterial* material);

		virtual bool setMaterial(IMaterial* material);

		virtual bool setMaterialName(const std::string& name);

		virtual bool setMaterialName(u32 subset, const std::string& name);

		virtual IMaterial* getMaterial(u32 subset = 0);

		virtual IMesh* getMesh();

		virtual u32 getSubsetCount() const;

		virtual void registerInstanceForRendering(IInstanceNode* instanceNode);

		virtual void render(E_PIPELINE_USAGE usage);

		virtual void renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer){}

		virtual void OnRegisterSceneNode(bool bRecursion = true);

		virtual void OnRegisterSceneNode(u32 tag);

		virtual void addChild(ISceneNode* child);

		virtual IMesh* getMesh() const { return mMesh; }

		virtual void setReflectionPlane(IReflectionPlane* plane);

		virtual IReflectionPlane* getReflectionPlane();

		virtual ~CInstanceCollectionNode();

	protected:
		// decorator design pattern.
		IMesh*					mMesh;
		IMeshNode*				mMeshNode;
		
		// instanced buffer. 
		IMeshBuffer*			mInstanceBuffer;
		
		u8*						mInstancesData;

		// record actual intance count for rendering.
		u32						mInstanceNum;
	};
}



#endif

