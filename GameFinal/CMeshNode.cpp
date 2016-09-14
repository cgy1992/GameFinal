#include "stdafx.h"
#include "CMeshNode.h"
#include "ISceneManager.h"
#include "gfMath.h"
#include "ILightNode.h"
#include "CShaderVariableInjection.h"
#include "IInputLayoutManager.h"

namespace gf
{
	CMeshNode::CMeshNode(ISceneNode* parent,
		ISceneManager* smgr,
		bool bStatic,
		ISimpleMesh* mesh,
		IMaterial* material /*= nullptr*/,
		const XMFLOAT3& position /*= XMFLOAT3(0, 0, 0)*/,
		const XMFLOAT3& rotation /*= XMFLOAT3(0, 0, 0)*/,
		const XMFLOAT3& scale /*= XMFLOAT3(1.0f, 1.0f, 1.0f)*/)
		:IMeshNode(parent, smgr, bStatic, position, rotation, scale)
		, mMesh(mesh)
		, mMaterial(material)
	{
		AddReferenceCounted(mMesh);
		AddReferenceCounted(mMaterial);

		if (mMesh) {
			IInputLayoutManager* inputLayoutMgr = IInputLayoutManager::getInstance();
			u32 vertexFormat = mMesh->getVertexFormat();
			mInputLayout = inputLayoutMgr->get(vertexFormat);
			if (!mInputLayout) {
				IShader* vs = mMaterial->getPipeline(0)->getShader(EST_VERTEX_SHADER);
				mInputLayout = inputLayoutMgr->create(vertexFormat, vs);
			}

			mMaterial->getPipeline(0)->setInputLayout(mInputLayout);
			AddReferenceCounted(mInputLayout);
		}
	}

	void CMeshNode::renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer)
	{
		if (!mMaterial)
			return;

		IPipeline* pipeline = mMaterial->getPipeline(usage);
		
		if (pipeline)
		{
			mMesh->bind(instanceBuffer);

			CShaderVariableInjection::inject(this, pipeline, 0);

			pipeline->apply(usage);

			mMesh->drawInstanced(instanceCount);
		}
	}

	void CMeshNode::render(E_PIPELINE_USAGE usage)
	{
		if (!mMaterial)
			return;

		IPipeline* pipeline = mMaterial->getPipeline(usage);
		
		if (pipeline)
		{
			mMesh->bind();

			CShaderVariableInjection::inject(this, pipeline, 0);

			pipeline->apply(usage);

			mMesh->draw();
		}
	}

	void CMeshNode::OnRegisterSceneNode(bool bRecursion)
	{
		if (mVisible)
		{
			calcSortCode();
			mSceneManager->registerNodeForRendering(this);
			if (bRecursion)
				ISceneNode::OnRegisterSceneNode();
		}
	}

	bool CMeshNode::setMaterialName(const std::string& name)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		return setMaterial(material);
	}

	bool CMeshNode::setMaterialName(u32 subset, const std::string& name)
	{
		return setMaterialName(name);
	}

	void CMeshNode::calcSortCode()
	{
		/* customed order - 8 bit, mesh - 8 bit, pipeline - 48 bit */

		IVideoDriver* driver = mSceneManager->getVideoDriver();
		E_PIPELINE_USAGE usage = driver->getPipelineUsage();

		u32 meshCode = (mMesh) ? mMesh->getSortCode() : 0;
		u64 pipeCode = (mMaterial && mMaterial->getPipeline(usage)) ? mMaterial->getPipeline(usage)->getSortCode() : 0;

		mSortCode = ((u64)mRenderOrder << 56) | ((u64)meshCode << 48) | pipeCode;
	}

	void CMeshNode::setReflectionPlane(IReflectionPlane* plane)
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->setMapping(this, plane);
	}

	IReflectionPlane* CMeshNode::getReflectionPlane()
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		return mediator->getReflectionPlane(this);
	}

	CMeshNode::~CMeshNode() {
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->removeMapping(this);

		ReleaseReferenceCounted(mMesh);
		ReleaseReferenceCounted(mMaterial);
		ReleaseReferenceCounted(mInputLayout);
	}



}


