#include "stdafx.h"
#include "CModelMeshNode.h"
#include "ISceneManager.h"
#include "CShaderVariableInjection.h"

namespace gf
{

	bool CModelMeshNode::setMaterial(IMaterial* material, u32 subset)
	{
		if (subset >= mMesh->getSubsetCount())
			return false;

		mMaterials[subset] = material;
		return true;
	}

	IMaterial* CModelMeshNode::getMaterial(u32 subset)
	{
		if (subset >= mMesh->getSubsetCount())
			return nullptr;

		return mMaterials[subset];
	}

	bool CModelMeshNode::setMaterialName(const std::string& name, u32 subset)
	{
		if (subset >= mMesh->getSubsetCount())
			return false;

		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		mMaterials[subset] = material;
		return true;
	}

	void CModelMeshNode::render(E_PIPELINE_USAGE usage)
	{
		mMesh->bind();

		IPipeline* prePipeline = nullptr; /* 记录前一个流水线 */
		IMaterial* preMaterial = nullptr; /* 记录前一个子集的材质 */

		u32 subsetCount = mMesh->getSubsetCount();

		for (u32 i = 0; i < subsetCount; i++)
		{
			IMaterial* material = mMaterials[i];
			if (!material)
				continue;
			
			IPipeline* pipeline = material->getPipeline(usage);
			if (!pipeline)
				continue;

			//如果当前的流水线和上一条相同，则除了材质外的其他变量不用注入
			if (pipeline == prePipeline)
			{
				//当且仅当材质也不同于上一个时才注入
				if (preMaterial != material)
				{
					CShaderVariableInjection::injectMaterial(material, pipeline);
					pipeline->apply(usage);
				}
			}
			else // 如果两条流水线不同，全部变量都需要更新
			{
				CShaderVariableInjection::inject(this, pipeline, i);
				pipeline->apply(usage);
				prePipeline = pipeline;
			}

			mMesh->drawSubset(i);

			preMaterial = material;
		}

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

}

