#ifndef __TEXTURE_MANAGER_INTERFACE_H__
#define __TEXTURE_MANAGER_INTERFACE_H__

#include "IResourceFactory.h"
namespace gf
{
	class ITextureManager : public IReferenceCounted
	{
	public:
		virtual ITexture* load(const std::string& name, const std::string& filepath) = 0;

		virtual ITexture* create(
			const std::string& name,
			u32 width,
			u32 height,
			void* data,
			u32 mipLevel,
			E_GI_FORMAT format,
			u32 pitch = 0) = 0;

		virtual IRenderTarget* createRenderTarget(
			const std::string& name, 
			bool multiSampling = true,
			u32  width = 0,
			u32 height = 0,
			E_GI_FORMAT format = EGF_R32G32B32A32_FLOAT,
			u32 multiSamplingCount = 0,
			u32 multiSamplingQuality = 0) = 0;

		virtual IDepthStencilSurface* createDepthStencilSurface(
			const std::string& name,
			u32 width = 0,
			u32 height = 0,
			u32 depthBitNum = 24,
			u32 stencilBitNum = 8,
			bool multiSampling = false,
			u32 multiSamplingCount = 0,
			u32 multiSamplingQuality = 0,
			bool bShaderBound = false,
			bool bindDepthToShader = true) = 0;

		virtual ITexture* get(const std::string& name) const = 0;

		virtual IRenderTarget* getRenderTarget(const std::string& name) const = 0;

		virtual IDepthStencilSurface* getDepthStencilSurface(const std::string& name) const = 0;

		_DECLARE_RESOURCE_DESTROY_METHODS(ITexture);

	};
}

#endif

