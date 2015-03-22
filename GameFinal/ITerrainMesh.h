#ifndef __TERRAIN_MESH_INTERFACE_H__
#define __TERRAIN_MESH_INTERFACE_H__

#include "IMesh.h"
#include "IMaterial.h"

namespace gf
{
	class IResourceFactory;
	class ITextureManager;

	class ITerrainMesh : public IMesh
	{
	public:
		ITerrainMesh(const std::string& name,
			u32 sortcode,
			f32 vertexSpace,
			f32 heightScale)
			:IMesh(name, sortcode, math::SAxisAlignedBox(), 0)
			, mVertexSpace(vertexSpace)
			, mHeightScale(heightScale)
			, mRowVertexNum(0)
			, mTotalWidth(0)
			, mTessellated(false)
			, mPatchWidth(0)
			, mCellsPerPatch(0)
			, mTexcoordScale(1.0f)
			, mHeightMapTexture(nullptr)
		{

		}

		virtual bool init(const std::string& heightmapFileName,
			const std::string& heigtmapFullPath,
			IResourceFactory* resourceFactory,
			ITextureManager* textureManager,
			bool bCreateTessellationMesh,
			bool bCreateNormal = false,
			f32 fTexcoordScale = 1.0f,
			u32 cellsPerPatch = 64,
			E_MEMORY_USAGE usage = EMU_STATIC) = 0;

		virtual void bind() = 0;

		virtual void draw() const = 0;

		virtual ITexture* getHeightMapTexture()
		{
			return mHeightMapTexture;
		}

		virtual f32 getHeight(f32 x, f32 z) const = 0;

		virtual bool intersectRay(const math::SRay& ray, XMFLOAT3& intersectPoint, float epsilon = 0.001f) = 0;

		virtual f32 getHeightByRowAndCol(u32 row, u32 col) const = 0;

		f32 getVertexSpace() const
		{
			return mVertexSpace;
		}

		f32 getHeightScale() const
		{
			return mHeightScale;
		}

		u32 getRowVertexNum() const
		{
			return mRowVertexNum;
		}

		u32 getRowCellNum() const
		{
			return mRowVertexNum - 1;
		}

		f32 getTexcoordScale() const
		{
			return mTexcoordScale;
		}

		f32 getTotalWidth() const
		{
			return mTotalWidth;
		}

		f32 getPatchWidth() const
		{
			return mPatchWidth;
		}

		u32 getRowPatchNum() const
		{
			return mPatchNumPerRow;
		}

		bool isTessellated() const
		{
			return mTessellated;
		}

		std::string getHeightMapName() const
		{
			return mHeightMapFileName;
		}

	protected:

		f32							mVertexSpace;
		f32							mHeightScale;
		f32							mTotalWidth;
		f32							mTexcoordScale;

		u32							mRowVertexNum;
		bool						mTessellated;

		u32							mCellsPerPatch;
		f32							mPatchWidth;
		u32							mPatchNumPerRow;

		ITexture*					mHeightMapTexture;

		std::string					mHeightMapFileName;
	};

}

#endif
