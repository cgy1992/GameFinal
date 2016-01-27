#include "stdafx.h"
#include "CInstanceCollectionNode.h"
#include "ISimpleMesh.h"
#include "ISceneManager.h"
#include "CInstanceNode.h"

namespace gf
{
	bool CInstanceCollectionNode::init(IMesh* mesh, u32 maxInstanceNum, u32 eachInstanceDataSize)
	{
		mMaxInstanceNum = maxInstanceNum;
		mEachInstanceDataSize = eachInstanceDataSize;

		// step1. create mesh node
		if (mesh->getType() == EMT_SIMPLE_MESH)
		{
			ISimpleMesh* pSimpleMesh = dynamic_cast<ISimpleMesh*>(mesh);
			mMeshNode = mSceneManager->addMeshNode(pSimpleMesh, nullptr, nullptr, mStatic);
		}
		else if (mesh->getType() == EMT_MODEL_MESH)
		{
			IModelMesh* pModelMesh = dynamic_cast<IModelMesh*>(mesh);
			mMeshNode = mSceneManager->addModelMeshNode(pModelMesh, nullptr, mStatic);
		}
		else if (mesh->getType() == EMT_ANIMATE_MODEL_MESH)
		{
			IAnimatedMesh* pAnimateMesh = dynamic_cast<IAnimatedMesh*>(mesh);
			mMeshNode = mSceneManager->addAnimatedMeshNode(pAnimateMesh, nullptr, mStatic);
		}

		if (!mMeshNode)
		{
			GF_PRINT_CONSOLE_INFO("MeshNode create fail!.\n");
			return false;
		}

		// detach mesh node from its parent.
		mMeshNode->remove();
		mMeshNode->setNeedCulling(false);
		
		// step2. create instance buffer.
		IResourceFactory* resourceFactory = IResourceFactory::getInstance();
		mInstanceBuffer = resourceFactory->createMeshBuffer(EMU_DYNAMIC, nullptr, nullptr, 
			mMaxInstanceNum, 0, sizeof(XMFLOAT4X4) + mEachInstanceDataSize, false);

		if (!mInstanceBuffer)
		{
			GF_PRINT_CONSOLE_INFO("Create Instance buffer failed.\n");
			mMeshNode->destroy();
			return false;
		}

		// step3. allocate data memory
		u32 dataSize = (sizeof(XMFLOAT4X4)+mEachInstanceDataSize) * mMaxInstanceNum;
		mInstancesData = (u8*)malloc(dataSize);

		mMesh = mesh;

		return true;
	}

	CInstanceCollectionNode::~CInstanceCollectionNode()
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->removeMapping(this);

		free(mInstancesData);
		ReleaseReferenceCounted(mInstanceBuffer);
		ReleaseReferenceCounted(mMeshNode);
	}

	bool CInstanceCollectionNode::setMaterial(u32 subset, IMaterial* material)
	{
		return mMeshNode->setMaterial(subset, material);
	}

	bool CInstanceCollectionNode::setMaterial(IMaterial* material)
	{
		return mMeshNode->setMaterial(material);
	}

	bool CInstanceCollectionNode::setMaterialName(const std::string& name)
	{
		return mMeshNode->setMaterialName(name);
	}

	bool CInstanceCollectionNode::setMaterialName(u32 subset, const std::string& name)
	{
		return mMeshNode->setMaterialName(subset, name);
	}

	IMaterial* CInstanceCollectionNode::getMaterial(u32 subset /*= 0*/)
	{
		return mMeshNode->getMaterial(subset);
	}

	IMesh* CInstanceCollectionNode::getMesh()
	{
		return mMesh;
	}

	IInstanceNode* CInstanceCollectionNode::addInstance(
		bool bStatic, 
		const XMFLOAT3& position, 
		const XMFLOAT3& rotation, 
		const XMFLOAT3& scale)
	{
		ISceneNode* parent = this;
		if (bStatic)
			parent = mSceneManager->getDefaultOctreeManager();

		IInstanceNode* node = new CInstanceNode(nullptr, mSceneManager, this, bStatic, position, rotation, scale);
		parent->addChild(node);
		return node;
	}

	void CInstanceCollectionNode::registerInstanceForRendering(IInstanceNode* instanceNode)
	{
		if (mInstanceNum < mMaxInstanceNum)
		{
			u32 stride = sizeof(XMFLOAT4X4)+mEachInstanceDataSize;
			u8* pCurrentInstancesData = mInstancesData + stride * mInstanceNum;

			XMFLOAT4X4 world = instanceNode->getWorldTransformation();
			memcpy(pCurrentInstancesData, &world, sizeof(XMFLOAT4X4));

			const void* data = instanceNode->getData();
			if (data)
			{	
				pCurrentInstancesData += sizeof(XMFLOAT4X4);
				memcpy(pCurrentInstancesData, data, mEachInstanceDataSize);
			}
			

			mInstanceNum++;
		}
	}

	void CInstanceCollectionNode::render(E_PIPELINE_USAGE usage)
	{
		/*
		if (usage == EPU_FORWARD)
		{
			int a = 1;
			std::cout << mInstanceNum << std::endl;
		}
		*/


		// update instance buffer.
		if (mInstanceNum > 0)
		{
			u32 stride = sizeof(XMFLOAT4X4)+mEachInstanceDataSize;
			
			mInstanceBuffer->setVertexData(mInstancesData, mInstanceNum);

			mMeshNode->renderInstanced(usage, mInstanceNum, mInstanceBuffer);

			mInstanceNum = 0;
		}
	}

	u32 CInstanceCollectionNode::getSubsetCount() const
	{
		return mMeshNode->getSubsetCount();
	}

	void CInstanceCollectionNode::OnRegisterSceneNode(bool bRecursion /*= true*/)
	{
		mSortCode = ((u64)mRenderOrder << 56);
		if (mVisible)
			mSceneManager->registerNodeForRendering(this);
	}

	void CInstanceCollectionNode::OnRegisterSceneNode(u32 tag)
	{
		mSortCode = ((u64)mRenderOrder << 56);
		if (mVisible && (mTag & tag))
			mSceneManager->registerNodeForRendering(this);
	}

	void CInstanceCollectionNode::addChild(ISceneNode* child)
	{
		if (child->getNodeType() == ESNT_INSTANCE_MESH)
		{
			ISceneNode::addChild(child);
		}
	}

	math::SAxisAlignedBox CInstanceCollectionNode::getAabb() const
	{
		return mSceneManager->getAabb();
	}

	void CInstanceCollectionNode::setReflectionPlane(IReflectionPlane* plane)
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->setMapping(this, plane);
	}

	IReflectionPlane* CInstanceCollectionNode::getReflectionPlane()
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		return mediator->getReflectionPlane(this);
	}
}



