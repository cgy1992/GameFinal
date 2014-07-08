#include "stdafx.h"
#include "CResourceFactory.h"
#include "CSimpleMesh.h"
#include "CModelMesh.h"
#include "CAnimatedMesh.h"
#include "CTerrainMesh.h"

namespace gf
{
	ISimpleMesh* CResourceFactory::createSimpleMesh(
		const std::string& name,
		u32 sortcode,
		void* vertices,
		void* indices,
		u32 vertexCount,
		u32 vertexStride,
		u32 indiceCount,
		const math::SAxisAlignedBox& aabb,
		bool bit32Index,
		E_MEMORY_USAGE usage)
	{
		IMeshBuffer* buffer = createMeshBuffer(usage, vertices, indices, vertexCount, indiceCount, vertexStride, bit32Index);
		if (!buffer)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the failure of mesh buffer.\n", name.c_str());
			return nullptr;
		}

		CSimpleMesh* mesh = new CSimpleMesh(name, sortcode, aabb, buffer);
		if (!mesh)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the lack of memory.\n", name.c_str());
			buffer->drop();
			return nullptr;
		}

		return mesh;
	}

	IModelMesh* CResourceFactory::createModelMesh(
		const std::string& name,
		u32 sortcode,
		void* vertices,
		void* indices,
		u32 vertexCount,
		u32 vertexStride,
		u32 indiceCount,
		const math::SAxisAlignedBox& aabb,
		bool bit32Index,
		const std::vector<IModelMesh::SModelSubset>& subsets,
		E_MEMORY_USAGE usage)
	{
		IMeshBuffer* buffer = createMeshBuffer(usage, vertices, indices, vertexCount, indiceCount, vertexStride, bit32Index);
		if (!buffer)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the failure of mesh buffer.\n", name.c_str());
			return nullptr;
		}

		if (subsets.size() < 1)
		{
			GF_PRINT_CONSOLE_INFO("FAIL:The mesh ('%s') create failed!. The subsets count must be greater than 1.\n", name.c_str());
			return nullptr;
		}

		CModelMesh* mesh = new CModelMesh(name, sortcode, aabb, subsets, buffer);
		if (!mesh)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the lack of memory.\n", name.c_str());
			buffer->drop();
			return nullptr;
		}

		return mesh;
	}

	IAnimatedMesh* CResourceFactory::createAnimatedModelMesh(
		const std::string& name,
		u32 sortcode,
		void* vertices,
		void* animateVertices,
		void* indices,
		u32 vertexCount,
		u32 animateVertexCount,
		u32 indicesCount,
		u32 vertexStride,
		u32 animateVertexStride,
		const math::SAxisAlignedBox& aabb,
		bool bit32Index,
		E_MEMORY_USAGE usage,
		const std::vector<SAnimatedModelSubset>& subsets,
		const std::vector<SModelBone>& bones,
		const std::vector<SBoneAnimationClip>& animateClips)
	{
		IAnimatedMeshBuffer* buffer = createAnimatedMeshBuffer(usage, vertices, animateVertices,
			indices, vertexCount, animateVertexCount, indicesCount,
			vertexStride, animateVertexStride, bit32Index);

		if (!buffer)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the failure of mesh buffer.\n", name.c_str());
			return nullptr;
		}

		if (subsets.size() < 1)
		{
			GF_PRINT_CONSOLE_INFO("FAIL:The mesh ('%s') create failed!. The subsets count must be greater than 1.\n", name.c_str());
			return nullptr;
		}

		CAnimatedMesh* mesh = new CAnimatedMesh(name, sortcode, aabb, subsets, bones, animateClips, buffer);

		//CModelMesh* mesh = new CModelMesh(name, sortcode, subsets, buffer);
		if (!mesh)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the lack of memory.\n", name.c_str());
			buffer->drop();
			return nullptr;
		}

		return mesh;

	}


	ITerrainMesh* CResourceFactory::createTerrainMesh(
		const std::string& name,
		u32 sortcode,
		const std::string& szRawFileName,
		f32 vertexSpace,
		f32 heightScale,
		bool bCreateTessellationMesh,
		bool bCreateNormal,
		f32 fTexcoordScale,
		u32 cellsPerPatch,
		E_MEMORY_USAGE usage,
		ITextureManager* textureManager)
	{
		CTerrainMesh* mesh = new CTerrainMesh(name, sortcode, vertexSpace, heightScale);
		if (!mesh)
		{
			GF_PRINT_CONSOLE_INFO("The terrain mesh ('%s') create failed! Due to memory shortage.\n", name.c_str());
			return nullptr;
		}

		// szRawFileName is just the file name without full path
		// so here full path must be obtained through ResourceGroupManager's getFullPath method


		std::string rawFilePath;
		if (!mResourceGroupManager->getFullPath(szRawFileName, rawFilePath))
		{
			GF_PRINT_CONSOLE_INFO("The terrain mesh ('%s') create failed!  Since raw file named '%s' has not been found.\n", name.c_str(), szRawFileName.c_str());
			return nullptr;
		}

		if (!mesh->init(szRawFileName, rawFilePath, this, textureManager, 
			bCreateTessellationMesh, bCreateNormal, 
			fTexcoordScale, cellsPerPatch, usage))
		{
			mesh->drop();
			return nullptr;
		}

		return mesh;
	}
}