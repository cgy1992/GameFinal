#include "stdafx.h"
#include "CTerrainNode.h"
#include "CShaderVariableInjection.h"

namespace gf
{
	void CTerrainNode::render()
	{
		if (!mMaterial)
			return;

		mMesh->bind();

		u32 pipelineCount = mMaterial->getPipelineCount();
		for (u32 i = 0; i < pipelineCount; i++)
		{
			if (mMaterial->isPipelineEnabled(i))
			{
				IPipeline* pipeline = mMaterial->getPipeline(i);

				CShaderVariableInjection::injectForTerrainNode(this, pipeline);

				pipeline->apply();

				mMesh->draw();
			}
		}
	}

	void CTerrainNode::OnRegisterSceneNode(bool bRecursion)
	{
		if (mVisible)
		{
			calcSortCode();
			mSceneManager->registerNodeForRendering(this);
			if (bRecursion)
				ISceneNode::OnRegisterSceneNode();
		}
	}

	bool CTerrainNode::setMaterialName(const std::string& name, u32 subset)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		ReleaseReferenceCounted(mMaterial);
		mMaterial = material;
		AddReferenceCounted(mMaterial);
		return true;
	}

	void CTerrainNode::calcSortCode()
	{
		/* mesh - 8 bit
		pipeline - 48 bit
		material - 8 bit
		*/
		mSortCode = ((u64)mMesh->getSortCode() << 56) | (mMaterial->getPipeline(0)->getSortCode() << 8);
		ITexture* pTexture = mMaterial->getTexture(0);
		if (pTexture != nullptr)
			mSortCode |= pTexture->getSortCode();
	}
}