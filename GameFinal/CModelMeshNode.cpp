#include "stdafx.h"
#include "CModelMeshNode.h"
#include "ISceneManager.h"
#include "CShaderVariableInjection.h"

namespace gf
{
	bool CModelMeshNode::setMaterial(u32 subset, IMaterial* material)
	{
		if (subset >= mMesh->getSubsetCount())
			return false;

		if (material != mMaterials[subset])
		{
			ReleaseReferenceCounted(mMaterials[subset]);
			mMaterials[subset] = material;
			AddReferenceCounted(material);
		}
		return true;
	}

	bool CModelMeshNode::setMaterial(IMaterial* material)
	{
		for (u32 i = 0; i < mMesh->getSubsetCount(); i++)
		{
			setMaterial(i, material);
		}
		return true;
	}

	IMaterial* CModelMeshNode::getMaterial(u32 subset)
	{
		if (subset >= mMesh->getSubsetCount())
			return nullptr;

		return mMaterials[subset];
	}

	bool CModelMeshNode::setMaterialName(const std::string& name)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		return setMaterial(material);
	}

	bool CModelMeshNode::setMaterialName(u32 subset, const std::string& name)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		return setMaterial(subset, material);
	}


	void CModelMeshNode::renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer)
	{
		if (instanceCount == 0)
			mMesh->bind();
		else
			mMesh->bind(instanceBuffer);

		IPipeline* prePipeline = nullptr; /* ��¼ǰһ����ˮ�� */
		IMaterial* preMaterial = nullptr; /* ��¼ǰһ���Ӽ��Ĳ��� */

		u32 subsetCount = mMesh->getSubsetCount();

		for (u32 i = 0; i < subsetCount; i++)
		{
			IMaterial* material = mMaterials[i];
			if (!material)
				continue;

			IPipeline* pipeline = material->getPipeline(usage);
			if (!pipeline)
				continue;

			//�����ǰ����ˮ�ߺ���һ����ͬ������˲������������������ע��
			if (pipeline == prePipeline)
			{
				//���ҽ�������Ҳ��ͬ����һ��ʱ��ע��
				if (preMaterial != material)
				{
					CShaderVariableInjection::injectMaterial(material, pipeline);
					pipeline->apply(usage);
				}
			}
			else // ���������ˮ�߲�ͬ��ȫ����������Ҫ����
			{
				CShaderVariableInjection::inject(this, pipeline, i);
				pipeline->apply(usage);
				prePipeline = pipeline;
			}


			if (instanceCount == 0)
				mMesh->drawSubset(i);
			else
				mMesh->drawSubsetInstanced(i, instanceCount);

			preMaterial = material;
		}
	}


	void CModelMeshNode::render(E_PIPELINE_USAGE usage)
	{
		renderInstanced(usage, 0, nullptr);
	}

	void CModelMeshNode::OnRegisterSceneNode(bool bRecursion)
	{
		if (mVisible)
		{
			calcSortCode();
			mSceneManager->registerNodeForRendering(this);
			if (bRecursion)
				ISceneNode::OnRegisterSceneNode();
		}
	}

	void CModelMeshNode::calcSortCode()
	{
		/* customed order - 8 bit, mesh - 8 bit, pipeline - 48 bit */
		IVideoDriver* driver = mSceneManager->getVideoDriver();
		E_PIPELINE_USAGE usage = driver->getPipelineUsage();


		u32 meshCode = (mMesh) ? mMesh->getSortCode() : 0;
		u64 pipeCode = 0;
		if (mMaterials[0])
		{
			if (mMaterials[0]->getPipeline(usage))
				pipeCode = mMaterials[0]->getPipeline(usage)->getSortCode();
		}
		mSortCode = ((u64)mRenderOrder << 56) | ((u64)meshCode << 48) | pipeCode;
	}

	void CModelMeshNode::setReflectionPlane(IReflectionPlane* plane)
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->setMapping(this, plane);
	}

	IReflectionPlane* CModelMeshNode::getReflectionPlane()
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		return mediator->getReflectionPlane(this);
	}

	CModelMeshNode::~CModelMeshNode()
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->removeMapping(this);

		ReleaseReferenceCounted(mMesh);
		ReleaseListElementCounted(mMaterials);
	}

}

