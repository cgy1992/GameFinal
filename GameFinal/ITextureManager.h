#ifndef __TEXTURE_MANAGER_INTERFACE_H__
#define __TEXTURE_MANAGER_INTERFACE_H__

#include "IResourceFactory.h"

namespace gf
{
	class ITextureManager : public IReferenceCounted
	{
	public:
		virtual ITexture* load(const std::string& name) = 0;

		virtual ITextureCube* loadCubeTexture(const std::string& name) = 0;

		virtual void updateTemporaryTextures(u32 delta) = 0;

		virtual ITexture* createTexture2D(
			const std::string& name,
			u32 width,
			u32 height,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0) = 0;

		virtual ITexture2DArray* createTexture2DArray(
			const std::string& name,
			u32 width,
			u32 height,
			u32 arraySize,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0) = 0;

		virtual ITexture3D* createTexture3D(
			const std::string& name,
			u32 width,
			u32 height,
			u32 depth,
			u32 bindFlags,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0,
			u32 slicePitch = 0) = 0;

		virtual ITextureCube* createTextureCube(
			const std::string& name,
			u32 size,
			u32 bindFlags,
			void* data,
			u32 miplevel,
			E_GI_FORMAT format,
			u32 pitch = 0) = 0;

		virtual IRenderTarget* createRenderTarget(
			const std::string& name,
			u32  width = 0,
			u32 height = 0,
			E_GI_FORMAT format = EGF_R8G8B8A8_UNORM) = 0;

		virtual IDepthStencilSurface* createDepthStencilSurface(
			const std::string& name,
			u32 width = 0,
			u32 height = 0,
			u32 depthBitNum = 24,
			u32 stencilBitNum = 8,
			bool multiSampling = false,
			u32 multiSamplingCount = 1,
			u32 multiSamplingQuality = 0,
			bool bShaderBound = true,
			bool bindDepthToShader = true) = 0;

		virtual ITexture* get(const std::string& name, bool bLoadIfNotExist = true) = 0;

		virtual ITextureCube* getTextureCube(const std::string& name, bool bLoadIfNotExist = true) = 0;

		virtual IRenderTarget* getRenderTarget(const std::string& name) const = 0;

		virtual IDepthStencilSurface* getDepthStencilSurface(const std::string& name) const = 0;


		virtual IRenderTarget* getTempRenderTarget(
			u32 width = 0,
			u32 height = 0,
			E_GI_FORMAT = EGF_R8G8B8A8_UNORM) = 0;

		virtual bool releaseTempRenderTarget(IRenderTarget* pRenderTarget) = 0;


		virtual IDepthStencilSurface* getTempDepthStencilSurface(
			u32 width = 0,
			u32 height = 0,
			u32 depthBits = 32,
			u32 stencilBits = 0) = 0;

		virtual bool releaseTempDepthStencilSurface(IDepthStencilSurface* pDepthStencilSurface) = 0;


		_DECLARE_SINGLETON_INSTANCE(ITextureManager);

		_DECLARE_RESOURCE_DESTROY_METHODS(ITexture);

		const static std::string SHADOW_MAP_JITTER_TEXTURE;
		const static std::string PL_SHADOW_MAP_JITTER_TEXTURE;
	};
}

#endif

