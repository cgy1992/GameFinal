#ifndef __CMESHNODE_H__
#define __CMESHNODE_H__

#include "IMeshNode.h"
#include "ISimpleMesh.h"

namespace gf
{


	class CMeshNode : public IMeshNode
	{
	public:
		CMeshNode(ISceneNode* parent,
			ISceneManager* smgr,
			bool bStatic,
			ISimpleMesh* mesh,
			IMaterial* material = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:IMeshNode(parent, smgr, bStatic, position, rotation, scale)
			, mMesh(mesh)
			, mMaterial(material)
		{
			AddReferenceCounted(mMesh);
			AddReferenceCounted(mMaterial);
		}

		virtual ~CMeshNode();


		virtual void render(E_PIPELINE_USAGE usage);

		virtual void renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer);

		virtual void OnRegisterSceneNode(bool bRecursion = true);


		virtual bool setMaterialName(const std::string& name);

		virtual bool setMaterialName(u32 subset, const std::string& name);

		virtual void calcSortCode();

		virtual bool setMaterial(u32 subset, IMaterial* material)
		{
			return setMaterial(material);
		}

		virtual bool setMaterial(IMaterial* material)
		{
			if (mMaterial != material)
			{
				ReleaseReferenceCounted(mMaterial);
				mMaterial = material;
				AddReferenceCounted(mMaterial);
			}
			return true;
		}

		virtual IMaterial* getMaterial(u32 subset = 0)
		{
			return mMaterial;
		}

		virtual u32 getSubsetCount() const
		{
			return 1;
		}

		virtual void updateAbsoluteTransformation()
		{
			ISceneNode::updateAbsoluteTransformation();

			const math::SAxisAlignedBox& aabb = mMesh->getAabb();
			mOBB = computeOrientedBox(aabb);
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_MESH;
		}

		virtual IMesh* getMesh() const { return mMesh; }

		virtual void setReflectionPlane(IReflectionPlane* plane);

		virtual IReflectionPlane* getReflectionPlane();

	private:
		ISimpleMesh*			mMesh;
		IMaterial*				mMaterial;
	};

}

#endif