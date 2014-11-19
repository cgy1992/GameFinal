#include "stdafx.h"
#include "CInstanceNode.h"
#include "IInstanceCollectionNode.h"
#include "ISceneManager.h"

namespace gf
{
	CInstanceNode::CInstanceNode(ISceneNode* parent,
		ISceneManager* smgr,
		IInstanceCollectionNode* owner,
		bool bStatic,
		const XMFLOAT3& position /*= XMFLOAT3(0, 0, 0)*/,
		const XMFLOAT3& rotation /*= XMFLOAT3(0, 0, 0)*/,
		const XMFLOAT3& scale /*= XMFLOAT3(1.0f, 1.0f, 1.0f)*/)
		:IInstanceNode(parent, smgr, owner, bStatic, position, rotation, scale)
		, mInstanceData(nullptr)
	{
		mNeedCulling = false;
		u32 instanceDataSize = mOwnerCollection->getInstanceDataSize();
		/* use a memory pool is better. */

		if (instanceDataSize > 0)
			mInstanceData = (u8*)malloc(instanceDataSize);
	}

	CInstanceNode::~CInstanceNode()
	{
		free(mInstanceData);
	}

	IMaterial* CInstanceNode::getMaterial(u32 subset /*= 0*/)
	{
		return mOwnerCollection->getMaterial(subset);
	}

	void CInstanceNode::setData(void* data)
	{
		if (data)
		{
			u32 dataSize = getInstanceDataSize();
			memcpy((void*)mInstanceData, data, dataSize);
		}
	}

	const void* CInstanceNode::getData() const
	{
		return (void*)mInstanceData;
	}

	u32 CInstanceNode::getInstanceDataSize() const
	{
		return mOwnerCollection->getInstanceDataSize();
	}

	void CInstanceNode::updateAbsoluteTransformation()
	{
		ISceneNode::updateAbsoluteTransformation();
		const math::SAxisAlignedBox& aabb = mOwnerCollection->getMesh()->getAabb();
		mOBB = computeOrientedBox(aabb);
	}

	u32 CInstanceNode::getSubsetCount() const
	{
		return mOwnerCollection->getSubsetCount();
	}

	void CInstanceNode::OnRegisterSceneNode(bool bRecursion /*= true*/)
	{
		if (mVisible)
		{
			mSceneManager->registerNodeForRendering(this, ENT_INSTANCE_NODE);
		}
	}

	void CInstanceNode::registerToCollectionForRendering()
	{
		mOwnerCollection->registerInstanceForRendering(this);
	}

}

