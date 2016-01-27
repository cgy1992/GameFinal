#include "stdafx.h"
#include "CAnimatedMeshNode.h"
#include "CShaderVariableInjection.h"
#include "ISceneManager.h"

namespace gf
{

	bool CAnimatedMeshNode::setMaterialName(const std::string& name)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		return setMaterial(material);
	}

	bool CAnimatedMeshNode::setMaterialName(u32 subset, const std::string& name)
	{
		IMaterial* material = mSceneManager->getVideoDriver()->getMaterialManager()->get(name);
		if (material == nullptr)
			return false;

		return setMaterial(subset, material);
	}

	bool CAnimatedMeshNode::setMaterial(u32 subset, IMaterial* material)
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

	bool CAnimatedMeshNode::setMaterial(IMaterial* material)
	{
		for (u32 i = 0; i < mMesh->getSubsetCount(); i++)
		{
			setMaterial(i, material);
		}
		return true;
	}

	IMaterial* CAnimatedMeshNode::getMaterial(u32 subset)
	{
		if (subset >= mMesh->getSubsetCount())
			return nullptr;

		return mMaterials[subset];
	}

	void CAnimatedMeshNode::updateSubsetBones(const std::vector<SModelSubsetBone>& bones)
	{
		for (u32 i = 0; i < bones.size(); i++)
		{
			u32 boneIndex = bones[i].BoneIndex;
			XMMATRIX toRootMatrix = XMLoadFloat4x4(&mAbsoluteBoneTransforms[boneIndex]);
			XMMATRIX offsetMatrix = XMLoadFloat4x4(&bones[i].OffsetTransform);

			XMMATRIX combined = offsetMatrix * toRootMatrix;
			combined = XMMatrixTranspose(combined);

			XMStoreFloat4x4(&mSubsetBoneTransforms[i], combined);
		}
	}

	void CAnimatedMeshNode::render(E_PIPELINE_USAGE usage)
	{
		renderInstanced(usage, 0, nullptr);
	}


	void CAnimatedMeshNode::renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer)
	{
		XMFLOAT4X4 preAbsoluteTransform;
		if (instanceCount == 0)
			mMesh->bind();
		else
			mMesh->bind(instanceBuffer);

		IPipeline* prePipeline = nullptr; /* ��¼ǰһ����ˮ�� */
		IMaterial* preMaterial = nullptr; /* ��¼ǰһ���Ӽ��Ĳ��� */

		u32 subsetCount = mMesh->getSubsetCount();

		updateAbsoluteBoneTransforms();
		const std::vector<SAnimatedModelSubset>& subsets = mMesh->getSubsets();

		for (u32 i = 0; i < subsetCount; i++)
		{
			IMaterial* material = mMaterials[i];
			if (!material)
				continue;

			/* transform subset bones. */
			if (subsets[i].Skinned)
			{
				updateSubsetBones(subsets[i].Bones);
			}
			else
			{
				// ������Ӽ��޶���������Ҫ�ı�����任
				u32 boneId = subsets[i].BoneId;
				if (boneId >= 0)
				{
					preAbsoluteTransform = mAbsoluteTransformation;
					XMFLOAT4X4 boneTransform = mAbsoluteBoneTransforms[boneId];
					XMMATRIX absoluteMatrix = XMLoadFloat4x4(&mAbsoluteTransformation);
					XMMATRIX boneMatrix = XMLoadFloat4x4(&boneTransform);
					XMStoreFloat4x4(&mAbsoluteTransformation, boneMatrix * absoluteMatrix);
				}
			}

			IPipeline* pipeline = material->getPipeline(usage);
			if (!pipeline)
				continue;

			// ������Ӽ��ж����������任������Ҫע���
			if (subsets[i].Skinned)
			{
				CShaderVariableInjection::injectBoneTransforms(pipeline,
					reinterpret_cast<f32*>(&mSubsetBoneTransforms[0]),
					subsets[i].Bones.size());
			}

			//CShaderVariableInjection::inject(this, pipeline, i);
			//pipeline->apply(usage);
			

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

			// �������任�����˸ı䣬����Ҫ�ָ�
			if (!subsets[i].Skinned && subsets[i].BoneId >= 0)
			{
				mAbsoluteTransformation = preAbsoluteTransform;
			}

			preMaterial = material;
		}

	}

	void CAnimatedMeshNode::OnRegisterSceneNode(bool bRecursion)
	{
		if (mVisible)
		{
			calcSortCode();
			mSceneManager->registerNodeForRendering(this);

			if (bRecursion)
				ISceneNode::OnRegisterSceneNode();
		}
	}

	void CAnimatedMeshNode::calcSortCode()
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

	bool CAnimatedMeshNode::setAnimation(u32 id)
	{
		if (id >= mMesh->getAnimationCount())
			return false;

		mCurrAnimationId = id;
		return true;
	}

	bool CAnimatedMeshNode::setAnimation(const std::string& name)
	{
		s32 id = mMesh->getAnimationId(name);
		if (id == -1)
			return false;

		mCurrAnimationId = id;
		return true;
	}

	void CAnimatedMeshNode::addTime(f32 t)
	{
		if (mCurrAnimationId < 0)
			return;

		mTimePos += t;
		mMesh->getRelativeBoneTransforms(mCurrAnimationId, mTimePos, mRelativeBoneTransforms);
	}

	void CAnimatedMeshNode::updateAbsoluteBoneTransforms()
	{
		if (mCurrAnimationId < 0)
			mMesh->getRelativeBoneTransforms(mAbsoluteBoneTransforms);
		else
			mMesh->getAbsoluteBoneTransforms(mRelativeBoneTransforms, mAbsoluteBoneTransforms);
	}

	IReflectionPlane* CAnimatedMeshNode::getReflectionPlane()
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		return mediator->getReflectionPlane(this);
	}

	void CAnimatedMeshNode::setReflectionPlane(IReflectionPlane* plane)
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->setMapping(this, plane);
	}

	CAnimatedMeshNode::~CAnimatedMeshNode()
	{
		IReflectionMediator* mediator = mSceneManager->getReflectionMediator();
		mediator->removeMapping(this);

		ReleaseReferenceCounted(mMesh);
		ReleaseListElementCounted(mMaterials);
	}

}

