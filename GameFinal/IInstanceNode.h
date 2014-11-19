#ifndef __INSTANCE_SUB_NODE_INTERFACE_H__
#define __INSTANCE_SUB_NODE_INTERFACE_H__

#include "IMeshNode.h"

namespace gf
{
	class IInstanceCollectionNode;

	class IInstanceNode : public IMeshNode
	{
	public:
		IInstanceNode(ISceneNode* parent,
			ISceneManager* smgr,
			IInstanceCollectionNode* owner,
			bool bStatic,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:IMeshNode(parent, smgr, bStatic, position, rotation, scale)
			, mOwnerCollection(owner)
		{
			///setRenderOrder(owner->getRenderOrder());
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_INSTANCE_MESH;
		}

		virtual void setData(void* data) = 0;

		virtual const void* getData() const = 0;

		virtual u32 getInstanceDataSize() const = 0;

		virtual void registerToCollectionForRendering() = 0;

		IInstanceCollectionNode* getCollectionNode() { return mOwnerCollection; }

	protected:
		
		IInstanceCollectionNode*		mOwnerCollection;
	};
}

#endif

