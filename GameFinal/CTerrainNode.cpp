#include "stdafx.h"
#include "CTerrainNode.h"
#include "CShaderVariableInjection.h"
#include "CSceneManager.h"
#include "CGrassLand.h"

namespace gf
{
	void CTerrainNode::render(E_PIPELINE_USAGE usage)
	{
		if (!mMaterial)
			return;

		mMesh->bind();
		
		IPipeline* pipeline = mMaterial->getPipeline(usage);

		if (pipeline)
		{
			CShaderVariableInjection::inject(this, pipeline, 0);

			pipeline->apply(usage);

			mMesh->draw();
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

	CTerrainNode::~CTerrainNode()
	{
		CSceneManager* smgr = dynamic_cast<CSceneManager*>(mSceneManager);
		std::list<ITerrainNode*>& terrainList = smgr->mTerrainNodes;
		for (auto it = terrainList.begin(); it != terrainList.end(); it++) {
			ITerrainNode* node = *it;
			if (node == this) {
				terrainList.erase(it);
				break;
			}
		}

		ReleaseReferenceCounted(mMesh);
		ReleaseReferenceCounted(mMaterial);
	}

	bool CTerrainNode::isInsideTerrainScope(f32 x, f32 z, bool localPivot /*= false*/) const
	{
		if (!localPivot)
		{
			x -= mPosition.x;
			z -= mPosition.z;
		}
		
		return mMesh->isInsideTerrainScope(x, z);
	}

	bool CTerrainNode::addGrassLand(u32 grassNum, f32 grassWidth, f32 grassHeight,
		const std::string& materialName,
		u32 pitchPerRow,
		f32 amplitude,
		XMFLOAT3 windDirection)
	{
		removeGrassLand();
		mGrassLand = new CGrassLand(this, XMFLOAT2(grassWidth, grassHeight));
		u32 grassCountPerPitch = grassNum / (pitchPerRow * pitchPerRow);
		if (!mGrassLand->init(grassCountPerPitch, pitchPerRow, materialName,
			amplitude, windDirection))
		{
			delete mGrassLand;
			return false;
		}
		return true;
	}

	void CTerrainNode::removeGrassLand()
	{
		if (mGrassLand)
		{
			delete mGrassLand;
			mGrassLand = nullptr;
		}
	}

	void CTerrainNode::update(f32 dt)
	{
		if (mGrassLand)
			mGrassLand->update(dt);
		ITerrainNode::update(dt);
	}

}
