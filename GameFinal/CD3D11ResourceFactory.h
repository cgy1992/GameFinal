#ifndef __CD3D11_RESOURCE_FACTORY_H_INCLUDE__
#define __CD3D11_RESOURCE_FACTORY_H_INCLUDE__

#include "CResourceFactory.h"
#include "CD3D11RenderState.h"
#include "CD3D11Sampler.h"
#include "CD3D11Texture3D.h"

namespace gf
{
	class CD3D11Driver;

	class CD3D11ResourceFactory : public CResourceFactory
	{
	public:
		CD3D11ResourceFactory(
			ID3D11Device* pd3dDevice,
			ID3D11DeviceContext* pd3dDeviceContext,
			CD3D11Driver* md3dDriver);

		virtual ITexture* loadTextureFromFile(
			const std::string& name,
			const std::string& filename,
			u32 sortcode);

		virtual ITextureCube* loadCubeTextureFromFile(
			const std::string& name,
			const std::string& filepath,
			u32 sortcode);

		virtual ITexture* createTexture2D(
			const std::string& name,
			u32 sortcode,
			u32 width,
			u32 height,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0);

		virtual ITexture2DArray* createTexture2DArray(
			const std::string& name,
			u32 sortcode,
			u32 width,
			u32 height,
			u32 arraySize,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0);

		virtual ITexture3D* createTexture3D(
			const std::string& name,
			u32 sortcode,
			u32 width,
			u32 height,
			u32 depth,
			u32	bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0,
			u32 slicePitch = 0);

		virtual ITextureCube* createTextureCube(
			const std::string& name,
			u32 sortcode,
			u32 size,
			u32 bindFlags,
			void* rawData,
			u32 miplevel,
			E_GI_FORMAT format,
			u32 pitch = 0);

		virtual ITexture* createDepthStencilTexture(
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
			bool bindDepthToShader);

		/*
		virtual IRenderTarget* createRenderTarget(
			const std::string& name,
			u32 sortcode,
			bool temporay,
			u32 width,
			u32 height,
			E_GI_FORMAT format,
			bool multiSampling,
			u32 multiSamplingCount,
			u32 multiSamplingQuality);

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
			bool bindDepthToShader);
			*/

		virtual IRenderState* createRenderState(const std::string& name);

		virtual ISampler* createSampler(
			const std::string& name,
			const SSamplerDesc& desc);

		virtual IPipeline* createPipeline(const std::string& name,
			IShader** shaders,
			u32 shaderCount,
			IInputLayout* inputlayout,
			E_PRIMITIVE_TYPE primitiveType,
			IRenderState* renderState);

		virtual IShader* createShaderFromFile(
			E_SHADER_TYPE shaderType,
			const char* szFilename,
			const char* szFunctionName,
			u32 id,
			const std::string& shaderName,
			const SShaderMacroSet& macros);

		virtual IInputLayout* createInputLayout(
			u32 sortCode,
			const std::vector<SInputLayoutElement>& elements,
			IShader* shader,
			u32 hashcode);

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
			bool bit32Index);

		virtual IMeshBuffer* createMeshBuffer(
			E_MEMORY_USAGE usage,
			void* vertices,
			void* indices,
			u32 vertexCount,
			u32 indicesCount,
			u32 vertexStride,
			bool bit32Index);

		virtual ~CD3D11ResourceFactory();

	private:
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;

		CD3D11Driver*					md3dDriver;
		CD3D11RenderStateCache*			md3dRenderStateCache;
		CD3D11SamplerStateCache*		md3dSamplerStateCache;
	};
}

#endif
