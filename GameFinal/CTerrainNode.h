#ifndef __TERRAIN_NODE_CLASS_H__
#define __TERRAIN_NODE_CLASS_H__

#include "ITerrainNode.h"
#include "ITerrainMesh.h"
#include "IMaterial.h"

namespace gf
{
	class CGrassLand;

	class CTerrainNode : public ITerrainNode
	{
	public:
		CTerrainNode(ISceneNode* parent,
			ISceneManager* smgr,
			ITerrainMesh* mesh,
			IMaterial* material = nullptr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:ITerrainNode(parent, smgr, position, rotation, scale)
			, mMesh(mesh)
			, mMaterial(material)
			, mGrassLand(nullptr)
		{
			AddReferenceCounted(mesh);
			AddReferenceCounted(mMaterial);
		}

		virtual ~CTerrainNode();

		virtual void render(E_PIPELINE_USAGE usage);

		virtual void OnRegisterSceneNode(bool bRecursion = true);

		virtual void calcSortCode();

		virtual bool setMaterial(u32 subset, IMaterial* material)
		{
			return setMaterial(material);
		}

		virtual bool setMaterial(IMaterial* material)
		{
			if (material != mMaterial)
			{
				ReleaseReferenceCounted(mMaterial);
				mMaterial = material;
				AddReferenceCounted(mMaterial);
			}
			return true;
		}

		virtual bool setMaterialName(const std::string& name);

		virtual bool setMaterialName(u32 subset, const std::string& name);

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
			getLocalAxis(mOBB.Axis);
			mOBB.Center = getAbsolutePosition();
			
			mOBB.Center.x += aabb.Center.x;
			mOBB.Center.y += aabb.Center.y;
			mOBB.Center.z += aabb.Center.z;

			mOBB.Extents = aabb.Extents;
		}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_TERRAIN_MESH;
		}

		virtual ITerrainMesh* getTerrainMesh()
		{
			return mMesh;
		}

		virtual IMesh* getMesh() const { return mMesh; }

		virtual f32 getHeight(f32 x, f32 z, bool localPivot = false) const;

		virtual bool isInsideTerrainScope(f32 x, f32 z, bool localPivot = false) const;

		virtual void renderInstanced(E_PIPELINE_USAGE usage, u32 instanceCount, IMeshBuffer* instanceBuffer){}

		virtual bool addGrassLand(u32 grassNum, f32 grassWidth, f32 grassHeight,
			const std::string& materialName,
			u32 pitchPerRow = 8,
			f32 amplitude = 0.5f,
			XMFLOAT3 windDirection = XMFLOAT3(-1.0f, 0, 1.0f));

		virtual void removeGrassLand();

		virtual void update(f32 dt);

		virtual void setReflectionPlane(IReflectionPlane* plane);

		virtual IReflectionPlane* getReflectionPlane();

	private:
		ITerrainMesh*			mMesh;
		IMaterial*				mMaterial;

		CGrassLand*				mGrassLand;

	};
}

#endif

