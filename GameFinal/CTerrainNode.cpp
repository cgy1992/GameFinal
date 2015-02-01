#include "stdafx.h"
#include "CTerrainNode.h"
#include "CShaderVariableInjection.h"

namespace gf
{
	void CTerrainNode::render(E_PIPELINE_USAGE usage)
	{
		if (!mMaterial)
			return;

		mMesh->bind();
		
		IPipeline* pipeline = mMaterial->getPipeline(usage);

		CShaderVariableInjection::inject(this, pipeline, 0);


		pipeline->apply(usage);

		mMesh->draw();
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

	bool CTerrainNode::setMaterialName(const std::string& name)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		return setMaterial(material);
	}

	bool CTerrainNode::setMaterialName(u32 subset, const std::string& name)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		return setMaterial(subset, material);
	}

	void CTerrainNode::calcSortCode()
	{
		/* customed order - 8 bit, mesh - 8 bit, pipeline - 48 bit */

		IVideoDriver* driver = mSceneManager->getVideoDriver();
		E_PIPELINE_USAGE usage = driver->getPipelineUsage();

		u32 meshCode = (mMesh) ? mMesh->getSortCode() : 0;
		u64 pipeCode = (mMaterial && mMaterial->getPipeline(usage)) ? mMaterial->getPipeline(usage)->getSortCode() : 0;

		mSortCode = ((u64)mRenderOrder << 56) | ((u64)meshCode << 48) | pipeCode;
	}

	f32 CTerrainNode::getHeight(f32 x, f32 z, bool localPivot /*= false*/) const
	{
		f32 height;
		if (!localPivot)
		{
			x -= mPosition.x;
			z -= mPosition.z;
			height = mMesh->getHeight(x, z);
			height += mPosition.y;
		}
		else
		{
			height = mMesh->getHeight(x, z);
		}

		return height;
	}
}
