#ifndef __IRESOURCE_FACTORY_H_INCLUDE__
#define __IRESOURCE_FACTORY_H_INCLUDE__

#include "IResourceGroupManager.h"
#include "ITexture.h"
#include "IRenderState.h"
#include "gfGIFormat.h"
#include "IShader.h"
#include "IPipeline.h"
#include "IInputLayout.h"
#include "IRenderState.h"
#include "ISimpleMesh.h"
#include "IModelMesh.h"
#include "IAnimatedMesh.h"
#include "IAnimatedMeshBuffer.h"
#include "ITerrainMesh.h"
#include "IRenderTarget.h"
#include "IDepthStencilSurface.h"

namespace gf
{
	class IResourceFactory : public IReferenceCounted
	{
	public:

		virtual ~IResourceFactory(){}

		void setResourceGroupManager(IResourceGroupManager* resourceGroupManager) 
		{ 
			mResourceGroupManager = resourceGroupManager;
		}

		virtual ITexture* loadTextureFromFile(
			const std::string& name,
			const std::string& filepath,
			u32 sortcode) = 0;

		virtual ITexture* createTexture(
			const std::string& name,
			u32 sortcode,
			u32 width,
			u32 height,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0) = 0;

		virtual IRenderTarget* createRenderTarget(
			const std::string& name,
			u32 sortcode,
			u32 width,
			u32 height,
			E_GI_FORMAT format,
			bool multiSampling,
			u32 multiSamplingCount, 
			u32 multiSamplingQuality) = 0;

		virtual IDepthStencilSurface* createDepthStencilSurface(
			const std::string& name,
			u32 sortcode,
			u32 width, 
			u32 height,
			u32 depthBitNum, 
			u32 stencilBitNum,
			bool multiSampling, 
			u32 multiSamplingCount, 
			u32 multiSamplingQuality,
			bool bShaderBound, 
			bool bindDepthToShader) = 0;

		virtual IRenderState* createRenderState(const std::string& name) = 0;

		virtual ISampler* createSampler(
			const std::string& name,
			const SSamplerDesc& desc) = 0;

		virtual IPipeline* createPipeline(const std::string& name,
			IShader** shaders,
			u32 shaderCount,
			IInputLayout* inputlayout,
			E_PRIMITIVE_TYPE primitiveType,
			IRenderState* renderState) = 0;

		virtual IShader* createShaderFromFile(
			E_SHADER_TYPE shaderType,
			const char* szFilename,
			const char* szFunctionName,
			u32 sortCode,
			const std::string& shaderName) = 0;

		virtual IInputLayout* createInputLayout(
			u32 sortCode,
			const std::vector<SInputLayoutElement>& elements,
			IShader* shader,
			u32 hashcode) = 0;

		virtual ISimpleMesh* createSimpleMesh(
			const std::string& name,
			u32 sortcode,
			void* vertices,
			void* indices,
			u32 vertexCount,
			u32 vertexStride,
			u32 indiceCount,
			const math::SAxisAlignedBox& aabb,
			bool bit32Index,
			E_MEMORY_USAGE usage = EMU_STATIC) = 0;

		virtual IModelMesh* createModelMesh(
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
			E_MEMORY_USAGE usage = EMU_STATIC) = 0;

		virtual IAnimatedMesh* createAnimatedModelMesh(
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
			const std::vector<SBoneAnimationClip>& animateClips) = 0;

		virtual ITerrainMesh* createTerrainMesh(
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
			ITextureManager* textureManager) = 0;

		virtual IAnimatedMeshBuffer* createAnimatedMeshBuffer(
			E_MEMORY_USAGE usage,
			void* vertices,
			void* animateVertices,
			void* indices,
			u32 vertexCount,
			u32 animateVertexCount,
			u32 indicesCount,
			u32 vertexStride,
			u32 animateVertexStride,
			bool bit32Index) = 0;

		virtual IMeshBuffer* createMeshBuffer(
			E_MEMORY_USAGE usage,
			void* vertices,
			void* indices,
			u32 vertexCount,
			u32 indicesCount,
			u32 vertexStride,
			bool bit32Index) = 0;


	protected:
		IResourceGroupManager*		mResourceGroupManager;
	};
}

#endif
