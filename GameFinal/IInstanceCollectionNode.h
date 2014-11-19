#ifndef __INSTANCE_SET_NODE_INTERFACE_H__
#define __INSTANCE_SET_NODE_INTERFACE_H__

#include "IMeshNode.h"

namespace gf
{
	class IInstanceNode;

	class IInstanceCollectionNode : public IMeshNode
	{
	public:
		IInstanceCollectionNode(ISceneNode* parent,
			ISceneManager* smgr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:IMeshNode(parent, smgr, false, position, rotation, scale)
			, mMaxInstanceNum(0)
			, mEachInstanceDataSize(0)
		{
			
		}

		/*
		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_INSTANCE_COLLECTION_MESH;
		}
		*/

		virtual u32 getInstanceDataSize() const 
		{
			return mEachInstanceDataSize;
		}

		virtual u32 getMaxInstanceNum() const
		{
			return mMaxInstanceNum;
		}

		virtual IInstanceNode* addInstance(bool bStatic, const XMFLOAT3& position,
			const XMFLOAT3& rotation, const XMFLOAT3& scale) = 0;

		virtual IMesh* getMesh() = 0;

		virtual void registerInstanceForRendering(IInstanceNode* instanceNode) = 0;

	protected:
		u32						mMaxInstanceNum;
		u32						mEachInstanceDataSize;
	};

}


#endif

